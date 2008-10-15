#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#define STACKTRACE_ROOT_INO 65535
#define STACKTRACE_MAX_FILENAME 16
#define STACKTRACE_MAX_FILESIZE 496
#define STACKTRACE_BLOCK_SIZE (STACKTRACE_MAX_FILENAME+STACKTRACE_MAX_FILESIZE)

struct stacktrace_fs_block {
    char filename[STACKTRACE_MAX_FILENAME];
    __u8    filedata[STACKTRACE_MAX_FILESIZE];
};

static ssize_t stacktrace_fs_read(struct file* f, char __user* buf,
        size_t count, loff_t* ppos) {
    struct dentry* de = f->f_dentry;
    /* ricavo l'inode dal file */
    struct inode* i = de->d_inode;
    struct super_block* sb = i->i_sb;
    struct buffer_head* bh;
    struct stacktrace_fs_block* stackblock;
    if (*ppos != 0)
        return 0;
    bh = sb_bread(sb, i->i_ino);
    if (!bh)
        return -ENOMEM;
    stackblock = (struct stacktrace_fs_block *) bh->b_data;
    memcpy(buf, stackblock->filedata, STACKTRACE_MAX_FILESIZE);
    brelse(bh);
    /* incremento la posizione per il seeking */
    *ppos = (loff_t) STACKTRACE_MAX_FILESIZE-1 ;
    return count ;
}

// static ssize_t hixosfs_write(struct file* f, const char __user* buf,
//         size_t count, loff_t* ppos) {
//     struct dentry* de = F->f_dentry;
//     struct inode* i = de->d_inode;
//     struct super_block* sb = i->i_sb;
//     struct buffer_head* bh;
//     struct stacktrace_fs_block* stackblock;
//     if (*ppos != 0)
//         return 0;
//     bh = sb_write(sb, i->i_ino);
//     if (!bh)
//         return -ENOMEM;
//     stackblock = (struct stacktrace_fs_block *) bh->b_data;
//     memcpy(buf, stackblock->filedata, STACKTRACE_MAX_FILESIZE);
//     brelse(bh);
//     /* incremento la posizione per il seeking */
//     *ppos = (loff_t) STACKTRACE_MAX_FILESIZE-1 ;
//     return count;
// }

static const struct file_operations stacktrace_fs_file_ops = {
    .read = stacktrace_fs_read,
//     .write = hixosfs_write,
};

static struct dentry* stacktrace_fs_lookup(struct inode* i,
        struct dentry* dentry, struct nameidata* nd) {
    int block, totalblocks;
    struct super_block* sb = i->i_sb;
    struct buffer_head* bh;
    struct stacktrace_fs_block* stackblock ;
    const char* name;
    struct inode* inode;
    name = dentry->d_name.name;
    totalblocks = sb->s_bdev->bd_inode->i_size >> 9 ;

    for (block=1; block<totalblocks; block++) {
        bh = sb_bread(sb, block);
        if (!bh)
            return ERR_PTR(-EACCES);
        stackblock = (struct stacktrace_fs_block *) bh->b_data;
        if (!strncmp(name, stackblock->filename, strlen(name))) {
            inode = new_inode(sb) ;
            /* imposto il numero dell'inode */
            inode->i_ino = block;
            inode->i_uid = inode->i_gid = 1001 ;
            inode->i_mode = S_IFREG|0644 ;
            inode->i_size = STACKTRACE_MAX_FILESIZE ;
            /* associo gli hook per le operazioni sul file */
            inode->i_fop = &stacktrace_fs_file_ops ;
            d_add(dentry, inode);
            brelse(bh) ;
            return NULL ;
        }
    }
    return ERR_PTR(-EACCES) ;
}

static const struct inode_operations stacktrace_fs_inode_ops = {
    .lookup = stacktrace_fs_lookup,
};

static int stacktrace_fs_readdir(struct file* f, void* dirent,
        filldir_t filldir) {
    unsigned long offset;
    struct dentry* de = f->f_dentry;
    struct inode* i = de->d_inode;
    struct super_block* sb = i->i_sb;
    struct buffer_head* bh;
    int res, block, totalblocks;
    struct stacktrace_fs_block* stackblock ;
    offset = f->f_pos;
    if (offset >0)
        return 1;
    totalblocks = sb->s_bdev->bd_inode->i_size >> 9 ;
    block = 1;
    /* leggo tutti i blocchi del dispositivo */
    for(;;) {
        if (block >= totalblocks)
            break ;
        bh = sb_bread(sb, block);
        if (!bh)
            break ;
        /* leggo i dati dal blocco */
        stackblock = (struct stacktrace_fs_block *) bh->b_data ;
        /* fill della struttura dirent */
        res = filldir(dirent, stackblock->filename,
                strlen(stackblock->filename), f->f_pos++, block, DT_REG);
        brelse(bh);
        if (res)
            return 0;
        block++ ;
    }
    return 1 ;
}

static const struct file_operations stacktrace_fs_dir_ops = {
    .readdir = stacktrace_fs_readdir,
};

static int stacktrace_sb_fill(struct super_block* sb, void* data, int silent) {
    struct inode* inode;
    struct dentry* root;
    struct buffer_head* bh;
    sb->s_flags |= MS_RDONLY;
    sb->s_maxbytes = 498;
    sb_set_blocksize(sb, 512);
    sb->s_magic = 0x11223344;
    bh = sb_bread(sb, 0);
    if (!bh)
        return -ENOMEM;
    if (strncmp(bh->b_data, "HiXoS_FS", 13)) {
        printk("not a valid stacktrace_fs filesystem !!!\n") ;
        brelse(bh);
        return -EINVAL;
    }
    inode = new_inode(sb) ;
    /* assegno un numero identificativo al root inode */
    inode->i_ino = STACKTRACE_ROOT_INO ;
    /* tipologia di inode, in questo caso e' una directory con permesso 755 */
    inode->i_mode = S_IFDIR | 0755 ;
    /* proprietario e gruppo dell'inode */
    inode->i_uid = inode->i_gid = 0 ;
    inode->i_op = &stacktrace_fs_inode_ops;
    inode->i_fop = &stacktrace_fs_dir_ops;
    root = d_alloc_root(inode);
    if (!root) {
        iput(inode);
        return -ENOMEM;
    }
    sb->s_root = root;
    return 0;
}


int stacktrace_fs_get_sb(struct file_system_type *fs_type, int flags,
        const char *dev_name, void *data, struct vfsmount *mnt) {
    return get_sb_bdev(fs_type, flags, dev_name, data, stacktrace_sb_fill, mnt);
}

static struct file_system_type stacktrace_fs_type = {
    .name = "hixosfs",
    .get_sb = stacktrace_fs_get_sb,
    .kill_sb = kill_block_super,
    .fs_flags = FS_REQUIRES_DEV,
    .owner = THIS_MODULE,
};

static int __init init_stacktrace(void) {
    printk("inizializzo modulo stacktrace...\n") ;
    return register_filesystem(&stacktrace_fs_type);
}

static void __exit free_stacktrace(void) {
    printk("libero il modulo stacktrace...\n") ;
    unregister_filesystem(&stacktrace_fs_type);
}

MODULE_DESCRIPTION("HiXoS FS - Fs versionato - versione 0.0.1-pre-alpha");
MODULE_AUTHOR("Vito De Tullio <vito.detullio@gmail.com");
MODULE_LICENSE("GPL");

module_init(init_stacktrace);
module_exit(free_stacktrace);

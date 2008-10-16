#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

/** Numero dell'inode della directory / */
static const unsigned HIXOS_ROOT_INO = 65535;

/** Dimensione massima del nome di un file */
#define HIXOS_MAX_FILENAME 16

/** Dimensione massima di un file */
#define HIXOS_MAX_FILESIZE 496

/**
    generico blocco del file system; al momento 1 blocco == 1 file
*/
struct hixosfs_block {
    char filename[HIXOS_MAX_FILENAME]; /** nome del file */
    __u8 filedata[HIXOS_MAX_FILESIZE]; /** contenuto del file */
};

/** FanQ! */
static const unsigned HIXOS_BLOCK_SIZE = sizeof(struct hixosfs_block);

/**
    legge da f count dati, partendo da ppos e scrivendoli (memcpy-andoli) su buf
*/
static ssize_t stacktrace_fs_read(struct file* f, char __user* buf,
        size_t count, loff_t* ppos) {
    struct inode* i = f->f_dentry->d_inode; /** ricavo l'inode dal file */
    /** carico il buffer indicato dall'inode */
    struct buffer_head* bh = sb_bread(f->f_dentry->d_inode->i_sb, i->i_ino);
    if (*ppos)  /** se mi è stato chiesto di continuare a leggere un file */
        return 0;   /** gli dico che è finito */
    if (!bh)    /** se ho avuto problemi a leggere il contenuto dell'inode */
        return -ENOMEM; /** piango */
    /** svuoto in buf il contenuto dei dati contenuti dal buffer */
    memcpy(buf, ((struct hixosfs_block*) bh->b_data)->filedata,
            HIXOS_MAX_FILESIZE);
    brelse(bh); /** libero la memoria necessaria per il buffer */
    *ppos = (loff_t) HIXOS_MAX_FILESIZE-1;  /** aggiorno la testina */
    return count;
}

/**
    scrive su f count dati, partendo da ppos e leggendoli (memcpy-andoli) da buf
 */
static ssize_t hixosfs_write(struct file* f, const char __user* buf,
        size_t count, loff_t* ppos) {
    return do_sync_write(f, buf, count, ppos);
}

/**
    operazioni che è possibile fare sui file regolari
    (al momento lettura e scrittura)
    @see stacktrace_fs_lookup()
*/
static const struct file_operations stacktrace_fs_file_ops = {
    .read = stacktrace_fs_read,
    .write = hixosfs_write,
};

/**
    il lookup è l'arte di ricavare l'inode dal nome del file :)

    versione triste: loopa per ogni blocco dell'hd alla ricerca dell'inode con
    nome del file == a quello richiesto
    @param i TODO (non è né target ne from: lo uso solo per prendermi il sb)
    @param dentry struttura che contiene il nome del file di cui si vuole
            l'inode, e nella quale verrà esso associato
    @param nd ???
*/
static struct dentry* stacktrace_fs_lookup(struct inode* i,
        struct dentry* dentry, struct nameidata* nd) {
    struct super_block* sb = i->i_sb;   /** dall'inode ricavo il superblock */
    int block = 1, totalblocks = sb->s_bdev->bd_inode->i_size >> 9; /** TODO */
    const char* name = dentry->d_name.name; /** ora ho il nome del file */

    /**
        Versione banale: per ogni blocco dell'hd (escluso lo 0) cercane uno che
        rappresenta l'inode col nome name
    */
    for (/* ignora blocco 0*/; block<totalblocks; block++) {
        struct hixosfs_block* stackblock = NULL;
        struct buffer_head* bh = sb_bread(sb, block);   /** leggi dall'hd il
                                                            block-esimo blocco
                                                        */
        if (!bh)                        /** se ci sono errori */
            return ERR_PTR(-EACCES);    /** piangi */
        stackblock = (struct hixosfs_block*) bh->b_data;/** metti in stackblock
                                                            i dati estratti */
        /** se il nome coincide con quello cercato */
        if (!strncmp(name, stackblock->filename, strlen(name))) {
            struct inode* inode = new_inode(sb);    /** crea un nuovo inode */
            inode->i_ino = block;   /** imposta un codice univoco (in questo
                                        caso è il numero di blocco), */
            inode->i_uid = inode->i_gid = 0;    /** proprietario e gruppo, */
            inode->i_mode = S_IFREG | 0644;     /** tipo e permessi */
            inode->i_size = HIXOS_MAX_FILESIZE; /** e dimensione dell'inode */
            inode->i_fop = &stacktrace_fs_file_ops; /** operazioni supportate */
            d_add(dentry, inode);   /** aggiungi l'inode a quelli raggiungibili
                                        da questa dentry */
            brelse(bh); /** libera la memoria del buffer */
            return NULL;
        }
    }
    return ERR_PTR(-EACCES) ;
}

/**
    Variabile usata per definire le operazioni da applicare su un inode
    @see hixosfs_sb_fill(), stacktrace_fs_lookup()
*/
static const struct inode_operations stacktrace_fs_inode_ops = {
    .lookup = stacktrace_fs_lookup,
};

/**
    Data una directory ne legge il contenuto
    @param f la directory :)
    @param dirent struttura di output da riempire
    @param filldir funzione che riempie la dir di elementi
*/
static int stacktrace_fs_readdir(struct file* f, void* dirent,
        filldir_t filldir) {
    struct super_block* sb = f->f_dentry->d_inode->i_sb;    /** piglio il sb */
    int res = 0, block = 1, totalblocks = sb->s_bdev->bd_inode->i_size >> 9;
    if (f->f_pos)   /** se sto cercando di continuare a leggere la directory */
        return 1;   /** fermati (readdir in UserSpace continua fin quando ritorno 0) */
    if (filldir(dirent, ".", 2, f->f_pos++, HIXOS_ROOT_INO, DT_DIR))
        return 0;
    if (filldir(dirent, "..", 3, f->f_pos++, HIXOS_ROOT_INO, DT_DIR))
        return 0;
    for( ; ; block += 1) {
        struct buffer_head* bh = NULL;
        struct hixosfs_block* stackblock = NULL;
        if (block >= totalblocks)   /** se ho superato il numero massimo di blocchi */
            break;                  /** fermo il ciclo (e quindi ritorno 1) */
        if (!(bh=sb_bread(sb, block)))  /** tento di caricare nel buffer il */
            break;                      /** block-esimo blocco */
        stackblock = (struct hixosfs_block*) bh->b_data; /** leggo i dati */
        /** e li uso per riempire la dirent */
        res = filldir(dirent, stackblock->filename,
                strlen(stackblock->filename), f->f_pos++, block, DT_REG);
        brelse(bh); /** libero la memoria del buffer */
        if (res)    /** ??? */
            return 0;
    }
    return 1;
}

/**
    Variabile usata per definire le operazioni da applicare su una directory
    @see hixosfs_sb_fill(), stacktrace_fs_readdir()
 */
static const struct file_operations stacktrace_fs_dir_ops = {
    .readdir = stacktrace_fs_readdir,
};

/**
    "Vera" funzione invocata quando si "mount-a" il file system:
*/
static int hixosfs_sb_fill(struct super_block* sb, void* data, int silent) {
    struct inode* inode = NULL;
    struct dentry* root = NULL;
    struct buffer_head* bh = NULL;
    /* fine definizione variabili */
    sb->s_maxbytes = HIXOS_MAX_FILESIZE;
    sb_set_blocksize(sb, HIXOS_BLOCK_SIZE);
    sb->s_magic = 0xF01DAB1E;   /** codice univoco per ogni file system (nel
                                 *  mio caso ho usato la "parola" foldable) */
    if (!(bh=sb_bread(sb, 0)))  /** legge dall'hd il contenuto dell'i-esimo
                                 *  inode a partire dal superblock (i==0) */
        return -ENOMEM;
    /**
        Questo superblock rappresenta un HiXoS_FS se inizia per "HiXoS_FS"
    */
    if (strncmp(bh->b_data, "HiXoS_FS", 13)) {
        printk("Not a valid HiXoS_FS file system !!!\n");
        brelse(bh); /** libera la memoria allocata per bh */
        return -EINVAL;
    }
    /**
        Devo creare almeno un inode per la root directory ("/")
        Ogni inode ha un codice identificativo, nel caso di "/" si usa
        HIXOS_ROOT_INO
    */
    inode = new_inode(sb);  /** mi faccio dare un nuovo inode dal superblock */
    inode->i_ino = HIXOS_ROOT_INO;      /** gli do l'id che uso per "/" */
    inode->i_mode = S_IFDIR | 0755 ;    /** che è una dir con quei permessi */
    inode->i_uid = inode->i_gid = 0 ;   /** che è di root:root */

    inode->i_op = &stacktrace_fs_inode_ops; /** operazione sull'inode */
    inode->i_fop = &stacktrace_fs_dir_ops;  /** operazioni sui file (vabé della
                                                directory "/") che puntano
                                                (punta) questo inode */

    root = d_alloc_root(inode); /** assegna 'st'inode ad una directory */
    if (!root) {                /** se c'è stato qualche problema */
        iput(inode);            /** di che l'inode non ti serve più */
        return -ENOMEM;         /** e piangi */
    }

    sb->s_root = root;  /** assegna 'sta directory come root del file system */
    return 0;
}

/**
    Leggero wrapper attorno get_sb_bdev(): l'unica modifica è nel penultimo
    parametro di quest'ultima funzione, a cui viene passato hixosfs_sb_fill()
*/
int hixosfs_get_sb(struct file_system_type *fs_type, int flags,
        const char *dev_name, void *data, struct vfsmount *mnt) {
    return get_sb_bdev(fs_type, flags, dev_name, data, hixosfs_sb_fill, mnt);
}

/**
    struttura che definisce un file system per il kernel (tant'è che è l'unico
    parametro di register_filesystem() e unregister_filesystem())
*/
static struct file_system_type hixosfs_type = {
    .name = "hixosfs",              /** nome del file system */
    .get_sb = hixosfs_get_sb,       /** funzione invocata al mount */
    .kill_sb = kill_block_super,    /** funzione invocata all'umount */
    .fs_flags = FS_REQUIRES_DEV,    /** HiXoS_FS richiede un dispositivo a blocchi */
    .owner = THIS_MODULE,
};

/**
    funzione invocata quando si installa il modulo (insmod, modprobe)
    @see module_init()
*/
static int __init init_hixosfs(void) {
    printk("Inizializzo HiXoS_FS\n") ;
    return register_filesystem(&hixosfs_type);
}

/**
    funzione invocata quando si rimuove il modulo (rmmod, modprobe -r)
    @see module_exit()
*/
static void __exit free_hixosfs(void) {
    printk("Libero HiXoS_FS\n") ;
    unregister_filesystem(&hixosfs_type);
}

// segue roba auto-esplicativa

MODULE_DESCRIPTION("HiXoS FS - Fs versionato - versione 0.0.1-pre-alpha");
MODULE_AUTHOR("Vito De Tullio <vito.detullio@gmail.com");
MODULE_LICENSE("GPL");

module_init(init_hixosfs);
module_exit(free_hixosfs);

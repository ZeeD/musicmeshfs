ifndef KERNELRELEASE

PWD := $(shell pwd)
KERNELDIR ?= /lib/modules/`uname -r`/build
CFLAGS := -std=c99 -pedantic -Wall -W -g

all:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.o *.ko *.mod.* .*.cmd Module.symvers modules.order
	rm -rf .tmp_versions

install:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules_install

ins:
	su -c 'insmod stacktrace_fs.ko && mount -o loop st.img mnt'

rem:
	su -c 'umount mnt && rmmod stacktrace_fs.ko'

else

obj-m := stacktrace_fs.o

endif

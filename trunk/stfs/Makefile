ifndef KERNELRELEASE

PWD := $(shell pwd)
KERNELDIR ?= /lib/modules/`uname -r`/build

all:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.o *.ko *.mod.* .*.cmd Module.symvers
	rm -rf .tmp_versions

install:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules_install

else

	obj-m := stacktrace_fs.o

endif

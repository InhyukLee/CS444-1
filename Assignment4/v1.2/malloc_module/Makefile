obj-m := malloc_mod.o
KDIR := /scratch/spring2017/13-07/clean-linux2/linux-yocto-3.14/
PWD_C := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) clean



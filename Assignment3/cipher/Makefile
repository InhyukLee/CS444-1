obj-m := sbd_enc.o
KDIR_C := /scratch/spring2017/13-07/clean-linux/linux-yocto-3.14/
PWD_C := $(shell pwd)

KDIR := ../clean-linux2/linux-yocto-3.14/
#PWD := /home/root/staging

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) clean


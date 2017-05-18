obj-m := sbd_enc.o
KDIR_C := /lib/modules/$(shell uname -r)/build
PWD_C := $(shell pwd)

#KDIR := /lib/modules/3.14.26ltsi-yocto-standard/build
#PWD := /home/root/staging

default:
	$(MAKE) -C $(KDIR_C) M=$(PWD_C) modules
clean:
	$(MAKE) -C $(KDIR_C) M=$(PWD_C) clean
disk:
	sudo insmod sbd_enc.ko
	sudo mkfs.ext2 /dev/sbd_enc0
	sudo mount /dev/sbd_enc0 /mnt
undisk:
	sudo umount /mnt
	sudo rmmod sbd_enc

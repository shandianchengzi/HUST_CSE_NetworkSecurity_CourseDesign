MODULE_NAME := nltest_kmod

obj-m := $(MODULE_NAME).o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)


all: mod app

mod:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

app:
	gcc -o nltest_uapp nltest_uapp.c

clean:
	rm -fr *.ko *.o *.cmd nltest_uapp $(MODULE_NAME).mod.c modules.order Module.symvers


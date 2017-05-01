ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabi-

ifneq ($(KERNELRELEASE),)
obj-m := nunchuk.o
else
KDIR := $(HOME)/nunchukdrv/linux
all:
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C $(KDIR) M=$$PWD
endif

clean:
	-@rm *.o *.ko *.mod.c *.symvers *.order *.cmd 2>/dev/null || true

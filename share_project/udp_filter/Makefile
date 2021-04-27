CROSS_COMPILE:= /home/peter/lede/staging_dir/toolchain-x86_64_gcc-8.4.0_musl/bin/x86_64-openwrt-linux-musl-

CC:= $(CROSS_COMPILE)gcc
LD:= $(CROSS_COMPILE)ld
obj-m += udpFilter.o
KERNELDIR:= /home/peter/lede/build_dir/target-x86_64_musl/linux-x86_64/linux-5.4.105
PWD:= $(shell pwd) 

modules: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 

modules_install: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install 

clean:
#	rm -f *.o
#	rm -f *.symvers
#	rm -f *.order
#	rm -f *.ko
#	rm -f *.mod.c
#	rm -f *.mod
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

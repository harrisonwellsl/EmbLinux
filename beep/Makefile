KERN_DIR := /home/harrison/work/zynq/linux/kernel/linux-xlnx-xilinx-v2018.3

obj-m := beep.o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=`pwd` modules

clean:
	make -C $(KERN_DIR) M=`pwd` clean
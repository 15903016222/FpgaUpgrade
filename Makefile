obj-m += gpio.o
obj-m += one_register_modu.o
all:
	make -C /home/ws/project/phascan-mini/linux SUBDIRS=$(PWD) modules
clean:
	make -C /home/ws/project/phascan-mini/linux SUBDIRS=$(PWD) clean 

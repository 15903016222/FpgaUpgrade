fpga_upgrade:fpga_upgrade.o spi.o
	arm-angstrom-linux-gnueabi-g++ fpga_upgrade.o spi.o -o fpga_upgrade
fpga_upgrade.o:fpga_upgrade.cpp
	arm-angstrom-linux-gnueabi-g++ -c fpga_upgrade.cpp
spi.o:spi.cpp
	arm-angstrom-linux-gnueabi-g++ -c spi.cpp
clean:
	rm *.o fpga_upgrade

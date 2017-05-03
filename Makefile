FpgaUpgrade: spi.o FpgaUpgrade.o
	arm-angstrom-linux-gnueabi-gcc spi.o FpgaUpgrade.o -o FpgaUpgrade
spi.o: spi.c
	arm-angstrom-linux-gnueabi-gcc -c spi.c -o spi.o
FpgaUpgrade.o: FpgaUpgrade.c
	arm-angstrom-linux-gnueabi-gcc -c FpgaUpgrade.c -o FpgaUpgrade.o
clean:
	rm *.o FpgaUpgrade


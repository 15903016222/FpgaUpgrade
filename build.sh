#!/bin/sh

make

arm-angstrom-linux-gnueabi-g++ fpga_upgrade.cpp -o fpga_upgrade
arm-angstrom-linux-gnueabi-gcc one_register.c   -o one_register



#CCPATH=<path_to_your_toolchain>
CCPATH=/home/th/armv4vm-cc/arm-unknown-eabi/

CC=$(CCPATH)bin/arm-unknown-eabi-gcc
CXX=$(CCPATH)bin/arm-unknown-eabi-g++
LD=$(CCPATH)bin/arm-unknown-eabi-ld
OBJCOPY=$(CCPATH)bin/arm-unknown-eabi-objcopy

OPTIONS=-O0 -Wall -marm -mcpu=arm7tdmi -mfloat-abi=soft -mlittle-endian -mno-unaligned-access -mno-thumb-interwork
OPTIONSC=$(OPTIONS) -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
OPTIONSCXX=$(OPTIONS) -std=c++17 -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
LDFLAGS=-L$(CCPATH)/arm-unknown-eabi/lib/ -L$(CCPATH)/lib/gcc/arm-unknown-eabi/9.2.0/ -lstdc++

all:game.bin primeN.bin

main.o:main.cpp
	$(CXX) $(OPTIONSCXX) -c main.cpp -o main.o

syscalls.o:syscalls.cpp
	$(CC) $(OPTIONSCXX) -c syscalls.cpp -o syscalls.o

startup.o:startup.s
	$(CC) $(OPTIONSC) -c startup.s -o startup.o

game.elf: game.ld main.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld main.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

game.bin:game.elf
	$(OBJCOPY) -O binary $< $@

mainPrimeN.o:mainPrimeN.cpp
	$(CXX) $(OPTIONSCXX) -c mainPrimeN.cpp -o mainPrimeN.o

primeN.elf: armv4vm.ld mainPrimeN.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld mainPrimeN.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

primeN.bin:primeN.elf
	$(OBJCOPY) -O binary $< $@


clean:
	rm -rf main.o startup.o syscalls.o game.elf game.bin mainPrimeN.o mainPrimeN.elf mainPrimeN.bin *~



#CCPATH=<path_to_your_toolchain>

CC=$(CCPATH)bin/arm-unknown-eabi-gcc
CXX=$(CCPATH)bin/arm-unknown-eabi-g++
LD=$(CCPATH)bin/arm-unknown-eabi-ld
OBJCOPY=$(CCPATH)bin/arm-unknown-eabi-objcopy

OPTIONS=-O0 -Wall -marm -mcpu=arm7tdmi -mfloat-abi=soft -mlittle-endian -mno-unaligned-access -mno-thumb-interwork
OPTIONSC=$(OPTIONS) -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
OPTIONSCXX=$(OPTIONS) -std=c++17 -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
LDFLAGS=-L$(CCPATH)/arm-unknown-eabi/lib/ -L$(CCPATH)/lib/gcc/arm-unknown-eabi/9.2.0/ -lstdc++

all:hello.bin primeN.bin

mainHello.o:mainHello.cpp
	$(CXX) $(OPTIONSCXX) -c mainHello.cpp -o mainHello.o

syscalls.o:syscalls.cpp
	$(CC) $(OPTIONSCXX) -c syscalls.cpp -o syscalls.o

startup.o:startup.s
	$(CC) $(OPTIONSC) -c startup.s -o startup.o

hello.elf:mainHello.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld mainHello.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

hello.bin:hello.elf
	$(OBJCOPY) -O binary $< $@

mainPrimeN.o:mainPrimeN.cpp
	$(CXX) $(OPTIONSCXX) -c mainPrimeN.cpp -o mainPrimeN.o

primeN.elf: armv4vm.ld mainPrimeN.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld mainPrimeN.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

primeN.bin:primeN.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf mainHello.o startup.o syscalls.o hello.elf hello.bin mainPrimeN.o primeN.elf primeN.bin *~


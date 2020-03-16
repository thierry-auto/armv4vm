
CCPATH=<path_to_your_toolchain>

CC=$(CCPATH)arm-unknown-eabi-gcc
CXX=$(CCPATH)arm-unknown-eabi-g++
LD=$(CCPATH)arm-unknown-eabi-ld
OBJCOPY=$(CCPATH)arm-unknown-eabi-objcopy

OPTIONS=-O0 -Wall -marm -mcpu=arm7tdmi -mfloat-abi=soft -mlittle-endian -mno-unaligned-access -mno-thumb-interwork
OPTIONSC=$(OPTIONS) -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
OPTIONSCXX=$(OPTIONS) -std=c++17 -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
LDFLAGS=-L$(CCPATH)/arm-unknown-eabi/lib/ -L$(CCPATH)/lib/gcc/arm-unknown-eabi/9.2.0/ -lstdc++

all:game.bin

main.o:main.cpp
        $(CXX) $(OPTIONSCXX) -c main.cpp -o main.o

syscalls.o:syscalls.cpp
        $(CC) $(OPTIONSCXX) -c syscalls.cpp -o syscalls.o

startup.o:startup.s
	$(CC) $(OPTIONSC) -c startup.s -o startup.o

game.elf: game.ld main.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T game.ld main.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

game.bin:game.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf main.o startup.o syscalls.o game.elf game.bin *~


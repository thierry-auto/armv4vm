#CCPATH=<path_to_your_toolchain>

CC=$(CCPATH)/bin/arm-unknown-eabi-gcc
CXX=$(CCPATH)/bin/arm-unknown-eabi-g++
LD=$(CCPATH)/bin/arm-unknown-eabi-ld
OBJCOPY=$(CCPATH)/bin/arm-unknown-eabi-objcopy

OPTIONS=-O0 -Wall -marm -mcpu=arm7tdmi -mfloat-abi=soft -mlittle-endian -mno-unaligned-access -mno-thumb-interwork
OPTIONSC=$(OPTIONS) -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
OPTIONSCXX=$(OPTIONS) -std=c++17 -fno-exceptions -fno-unwind-tables -ffreestanding -fno-use-cxa-atexit -fomit-frame-pointer
LDFLAGS=-L$(CCPATH)/arm-unknown-eabi/lib/ -L$(CCPATH)/lib/gcc/arm-unknown-eabi/9.3.0/ -lstdc++
#LDFLAGS=-L$(CCPATH)/arm-unknown-eabi/lib/ -L$(CCPATH)/lib/gcc/arm-unknown-eabi/9.2.0/ -lstdc++


all:hello.bin primen.bin float.bin printf.bin bench.bin

hello.o:hello.cpp
	$(CXX) $(OPTIONSCXX) -c hello.cpp -o hello.o

syscalls.o:syscalls.cpp
	$(CC) $(OPTIONSCXX) -c syscalls.cpp -o syscalls.o

startup.o:startup.s
	$(CC) $(OPTIONSC) -c startup.s -o startup.o

hello.elf:hello.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld hello.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

hello.bin:hello.elf
	$(OBJCOPY) -O binary $< $@

primen.o:primen.cpp
	$(CXX) $(OPTIONSCXX) -c primen.cpp -o primen.o

primen.elf: armv4vm.ld primen.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld primen.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

primen.bin:primen.elf
	$(OBJCOPY) -O binary $< $@

float.o:float.cpp
	$(CXX) $(OPTIONSCXX)  -c float.cpp -o float.o

float.elf: armv4vm.ld float.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld float.o startup.o syscalls.o -lstdc++ -lc -lgcc -o $@

float.bin:float.elf
	$(OBJCOPY) -O binary $< $@

printf.o:printf.cpp
	$(CXX) $(OPTIONSCXX)  -c printf.cpp -o printf.o

printf.elf: armv4vm.ld printf.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld printf.o startup.o syscalls.o -lstdc++ -lc -lgcc -lm -o $@

printf.bin:printf.elf
	$(OBJCOPY) -O binary $< $@

bench.o:bench.cpp
	$(CXX) $(OPTIONSCXX)  -c bench.cpp -o bench.o

bench.elf: armv4vm.ld bench.o startup.o syscalls.o
	$(LD) $(LDFLAGS) -T armv4vm.ld bench.o startup.o syscalls.o -lstdc++ -lc -lgcc -lm -o $@

bench.bin:bench.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf *.o *.elf *.bin *~


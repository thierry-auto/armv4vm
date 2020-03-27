armv4vm
=======

**armv4vm** is an incomplete but usable implementation of an ARMv4 processor. Although this implementation is based on booklet 4 of the ARM7TDMI-S instructions set, no particular feature to this processor has been developed. So this implemenation is probably common to a wide range of ARM cpu.

Because used in a larger project and it was not necessary, software interruptions, coprocessor operations, and thumb mode were not implemented..

Please send your feedback, bug reports, feature requests.

## Requirements

+ Qt5 is optional but recommended to run unit tests.
+ A cross toolchain. Do not use crosstoll-ng for a while. I didn't find the config to generate a suitable toolchain.
In the meantime, follow the instructions in the toolchain.txt file.

## Usage

Instantiate a VmProperties class and initialize there. The size of the memory is given in bytes.

```cpp
struct VmProperties vmProperties;

vmProperties.m_memsize = 1024 * 1024 * 24; // 24 MiB
vmProperties.m_bin = "/path/to/your/binaryFile";
```

Instantiate a VirtualMachine class with your VmProperties structure.

```cpp
VirtualMachine vm (& vmProperties);
```

Define an unsigned char pointer. It will be valued by the memory address of the virtual machine.

```cpp
uint8_t * mem = nullptr;
```

Initialize your vm and load the binary.

```cpp
mem = vm.init ();
vm.load ();
```

Finally, launch the virtual machine.

```cpp
vm.run (5000); // 5000 arm instructions will be executed.
```

Exchange information between your virtual machine and your host through a dedicated memory space and however it suits you. (uart, queue, etc.)
This space is located at address 0x00C00000 and is 0x00100000 bytes long. These arbitrary values are set during the generation of the binary. (cf. game.ld). Take a look at the unit tests `testProgram1()`. It's really basic but you will find inspiration.

## Nota bene

There is no memory protection. Illegal memory access by the guest program will likely cause the host program to crash.

## Contributions

Debugging the guest program through gdb or a homemade debugger would be great ! Any help about this or any other subject will be appreciated !

## Todo

Obviously a lot of things but maybe the first improvement would be to write computational tests and get a reference benchmark result. This would condition future developments.

- Writing benchmark
- Implement software interrupt to get a better control on vm (finish, standy-by, resume, etc.)
- Implement a memory protection.
- Implementing the thumb mode would be a good thing to reduce memory usage.
- Register and Getting statistics.
- Positioning the Qt signals `started`, `finished`, `suspended`, `crashed`, etc. to easily embedded into a Qt application.
- Setting Valgrind
- ...

## Reference

ARM Instruction Set ARM7TDMI-S Data Sheet

## License

This project is free software and released under
the [GNU General Public License][gpl].

 [gpl]: https://www.gnu.org/licenses/gpl-3.0.html
 [cng]: https://crosstool-ng.github.io/

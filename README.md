armv4vm
=======

**armv4vm** is minimalist but usable implementation of an ARMv4 processor. Although this implementation is based on booklet 4 of the ARM7TDMI-S instructions set, no particular feature to this processor has been developed. So this implementation is probably common to a wide range of ARM cpu.

Because used in a larger project and not necessary, coprocessor operations and thumb mode were not implemented.

Please send your feedback, bug reports or feature requests.

## Requirements

+ Qt5 is optional but recommended to run unit tests.
+ A GCC cross toolchain. A [crosstool-ng][cng] config file is provide and allow to build a suitable toolchain.
Some instructions are also given to manually build your toolchain.

## Usage

Exchange informations between your host and guest through a dedicated memory space and however it suits you. (uart, queue, etc.)
So, set your ld script (armv4vm.ld) and more especially the address for exchanges.

```cpp
...
uart      : ORIGIN = 0x05000000, LENGTH = 0x00100000 /* 1 mio */
...
uart_base    = ORIGIN(uart);
```

And use VirtualMachine classe as follow :


```cpp
#define MEMSIZE 256
#define UARTPOS 0x05000000

    vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
    vmProperties.m_bin     = "/path/to/your/binfile";
    uint8_t *mem           = nullptr;
    uint8_t *uart          = nullptr;
    bool     running       = true;

    
    // Instanciate and init your vm.
    VirtualMachine vm(&vmProperties);
    mem = vm.init();
    
    // Set Uart localisation
    uart = mem + UARTPOS;

    // Load vm
    if(vm.load()) {

        while (running) {

            // Run infinite loop and get out when a swi occurs.
            switch (vm.run()) {

            case VirtualMachine::Resume:
                break;

            case VirtualMachine::Stop:
                running = false;
                break;

            case VirtualMachine::Suspend:
                std::cout << (char)*uart;
                break;

            default:
                break;
            }
        }
    }
```

## Nota bene

There is no memory protection. Illegal memory access by the guest program will probably cause a host program fault.

## Contributions

Debugging the guest program through gdb or a homemade debugger would be great ! Any help about this or any other subject will be appreciated !

## Todo

Obviously a lot of things but maybe the first improvement would be to write computational tests and get a reference benchmark results. This would condition future developments.

- Writing benchmark
- Implement software interrupt to get a better control on vm (finish, standy-by, resume, etc.)
- Implement a memory protection.
- Implementing the thumb mode would be a good thing to reduce memory usage.
- Register and Getting statistics.
- Positioning the Qt signals `started`, `finished`, `suspended`, `crashed`, etc. to easily embedded into a Qt application.
- Setting gcov, gprof and valgrind configs.
- ...

## Reference

ARM Instruction Set ARM7TDMI-S Data Sheet

## License

This project is free software and released under
the [GNU General Public License][gpl].

 [gpl]: https://www.gnu.org/licenses/gpl-3.0.html
 [cng]: https://crosstool-ng.github.io/


armv4vm
=======

**armv4vm** is a minimalist yet functional implementation of an ARMv4 ALU. While this implementation is based on booklet 4 of the ARM7TDMI-S instruction set, no processor-specific features have been developed. As such, it is likely compatible with a wide range of ARMv4 ALUs.

**Note:** The THUMB mode and PSR instructions have not been implemented. This project is currently intended only for server-side game logic processing.

## Requirements

+ Qt6 is optional but recommended to run unit tests.
+ A GCC cross toolchain. A [crosstool-ng][cng] config file is provide and allow to build a suitable toolchain.
Some instructions are also given to manually build your toolchain.

## Memory Configuration and usage

Two types of VMs can be instantiated:

### 1. **Protected Memory Access**
Memory access permissions must be defined using `MemoryProperties`:

```cpp
    MemoryProperties properties;
    properties.m_layout.push_back({
        0 /*offset*/,
        32 /*size*/,
        AccessPermission::READ_WRITE /*permission*/});
    properties.m_layout.push_back({32, 32, AccessPermission::READ});
    properties.m_layout.push_back({64, 32, AccessPermission::WRITE});
```

### 2. **Unprotected Memory Access**
Simply set the total memory size:

```cpp
    MemoryProperties properties;
    properties.m_memorySizeBytes = 2_kb;
```

Warning: Illegal memory access by the guest program may cause a host program fault.

### 3.Usage
Exchange informations between your host and guest through a dedicated memory space and however it suits you. (uart, queue, etc.)
So, set your ld script (armv4vm.ld) and more especially the address for exchanges.

```cpp
    ...
    uart      : ORIGIN = 0x05000000, LENGTH = 0x00100000 /* 1 mio */
    ...
    uart_base    = ORIGIN(uart);
```

And for example, use VirtualMachine classe as follow :


```cpp
    VmProperties properties;
    uint8_t *mem           = nullptr;
    uint8_t *uart          = nullptr;
    bool     running       = true;

	properties.m_memoryProperties.m_layout.push_back({0 , 8_mb , AccessPermission::READ_WRITE});
	properties.m_bin     = "/path/to/your/binfile";
	
    std::unique_ptr<Vm> vm = Vm::build(properties);
	
    // Reset and get mem entry
    mem = vm->reset();
    
    // Set Uart localisation
    uart = mem + UARTPOS;

    // Load vm
    if(vm->load()) {

        while (running) {

            // Run infinite loop and get out when a swi occurs.
            switch (vm->run()) {

            case VirtualMachine::Resume:
                break;

            case VirtualMachine::Stop:
                running = false;
                break;

            case VirtualMachine::Suspend:
                std::cout << static_cast<char>(*uart);
                break;

            default:
                break;
            }
        }
    }
```

## Compiling Guest Programs

Guest programs must be compiled with GCC using at least the following flags:

```cpp
    -marm 
    -mcpu=arm7tdmi 
    -mfloat-abi=soft 
    -mlittle-endian 
    -mno-unaligned-access 
    -mno-thumb-interwork 
    -fno-unwind-tables 
    -fno-use-cxa-atexit 
    -fomit-frame-pointer
```

## Todo

- Comprehensive cleanup (refactoring, code hygiene, c++26, etc.).
- Remove all bitfield structures for better maintainability and avoiding maximum UB.
- Set up gcov, gprof, and Valgrind configurations for testing and profiling.
- Implement single/double-precision floating-point support (currently stubbed). Consider evolving toward ARMv5TE compatibility.
- Debugging the guest program through gdb.
- Write computational tests and establish reference benchmarks to guide future development
- Clarification of repository.

## Nota bene

The CMakeLists.txt file contains traces of providing the library in both header-only or static build. Currently, only the static build is possible and will remain so in future versions. In other words, do not attempt to implement header-only functionality. Header only install will probably and definitivly removed.

## Reference

ARM Instruction Set ARM7TDMI-S Data Sheet

## License

This project is free software and released under
the [GNU General Public License][gpl].

 [gpl]: https://www.gnu.org/licenses/gpl-3.0.html
 [cng]: https://crosstool-ng.github.io/





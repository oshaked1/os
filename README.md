# My OS (still needs a name)

:warning: This project is not actively developed anymore. For information regarding the current state, see [current features](./CURRENT_FEATURES.md).

Welcome to my humble OS. It's still not much, but we're getting there 😉

## Project Description

The objective of this project is simple -  create a fully-functioning operating system from scratch.

What does "from scratch" mean? The following assumptions are made:

- The operating system code will be written in a high-level language (C) which we already have a compiler for
- We also have an assembler for (you guessed it) assembly code
- We are running in a standard environment - we are using all standard platform features of the architecture we are using (e.g. BIOS services and standard I/O devices)
- We have external debugging capabilities (using QEMU)
- We are **not** using a premade bootloader (like GRUB) - we are creating one especially for this OS
- We are **not** using other people's code (except for a few lines here and there, mainly for device drivers)
- We **are** using other people's ideas and tutorials

#### Platform Selection

Initially, I wanted to be special and make the OS on a RISC architecture, anything that is not the ultra-popular and overused x86. I tried to find information about OS development for ARM, PowerPC, and even RISC-V. Very quickly I realized that there are 2 very big issues with anything that is not x86:

- There is hardly any beginner-friendly information available (let alone tutorials)
- Many aspects of the execution environment are platform dependent and not standardized (or at least widely used)

These problems made me realize that at least as a first OS project, I have to choose x86. Porting the OS to another platform later is always possible if I would really like too.

#### Goals for the Project

The current milestones that I set are divided into 4 major stages of development. This is just a rough idea and may be subject to changes.

###### Stage 1

- User mode with a basic shell
- 64-bit capable
- GDB stub

###### Stage 2

- Fully functioning file system
- Functional preemptive scheduling (no process threads)
- Virtual memory for user-mode processes

###### Stage 3

- Virtual files for kernel I/O
- Process thread management and scheduling
- Support for multiple CPU cores
- Basic networking (socket API)

###### Stage 4

- Syscall POSIX compliance
- Runtime environment for third party applications
- Functional Python port

## Setting up the Environment

My build environment is WSL1 but any Linux/Unix environment should be suitable. Running and debugging is done using QEMU, which requires a graphical user interface (afaik). WSL1 does not have any graphical capabilities so I use QEMU for Windows and run it from within WSL.

The following setup instructions are for Ubuntu 20.04 on WSL1 (should be identical for real Ubuntu). Setting up the environment and building on WSL1 is **HORRIBLY SLOW** but very convenient for me. WSL2 should be much faster.

Before setting up the build environment, make sure you have **gcc** and **make** installed: `apt install gcc make`

#### Binutils

Linking is done using GNU Binutils. Binutils also comes with miscellaneous tools like **readelf**. Binutils needs to be built separately for each target architecture. Obtain the source as following:

```bash
mkdir /tmp/src
cd /tmp/src
curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.37.tar.xz
tar xf binutils-2.37.tar.xz
```

After obtaining the source, build it for the needed targets as following:

###### i386-elf (32-bit x86)

```bash
mkdir binutils-build
cd binutils-build
export PREFIX="/usr/local/i386elfgcc" # installation directory (Makefile uses this directory by default)
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"
../binutils-2.37/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
sudo make all install 2>&1 | tee make.log
cd ..
rm -rf binutils-build
```

###### x86_64-elf (64-bit x86)

```bash
mkdir binutils-build
cd binutils-build
export PREFIX="/usr/local/x86_64elfgcc" # installation directory (Makefile uses this directory by default)
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
../binutils-2.37/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
sudo make all install 2>&1 | tee make.log
cd ..
rm -rf binutils-build
```

#### GCC Cross Compilers

Compiling is done using a GCC cross compiler for the relevant architecture. GCC also comes with GDB for debugging. Obtain the source as following:

```bash
mkdir /tmp/src
cd /tmp/src
curl -O http://ftp.gnu.org/gnu/gcc/gcc-11.2.0/gcc-11.2.0.tar.gz
tar xf gcc-11.2.0.tar.gz
```

After obtaining the source, build it for the needed targets as following:

###### i386-elf

```bash
mkdir gcc-build
cd gcc-build
export PREFIX="/usr/local/i386elfgcc" # installation directory (Makefile uses this directory by default)
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"
../gcc-11.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c --without-headers
make all-gcc
make all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
cd ..
rm -rf gcc-build
```

###### x86_64-elf

```bash
mkdir gcc-build
cd gcc-build
export PREFIX="/usr/local/x86_64elfgcc" # installation directory (Makefile uses this directory by default)
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
../gcc-11.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c --without-headers
make all-gcc
make all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
cd ..
rm -rf gcc-build
```

:warning: If the `configure` script fails and says that building GCC requires GMP, MPFR and MPC, do as following:

```bash
cd ../gcc-11.2.0
./contrib/download_prerequisites
cd ../gcc-build
```

#### NASM

NASM is used for assembling boot sector code and some other assembly code here and there. Install it using `apt install nasm`

#### QEMU

QEMU is an emulator for many different CPU architectures, including x86. It is used for running the OS. QEMU comes with built-in debugging, so we can debug our OS even if we don't have a GDB stub implemented. QEMU can be installed on Windows and Linux (and MacOS if that's your thing), but requires a graphical interface (so it cannot be installed on WSL1). For Linux, install it using `apt install qemu`. For Windows, download and run the installer.

There are a few alternatives to QEMU:

- **Bochs** - a full x86 emulator. It is considered more accurate than QEMU, but I decided not to use it because I have some experience with QEMU (also QEMU can emulate many other architectures, which means it will be easier porting the OS if I'd ever want to).
- **VMware** - Limited to host architecture, no built-in debugging.
- **VirtualBox** - Same as VMware.
- **A Physical Machine** - obviously much harder to test and debug. Physical machines are needed for testing platform-specific code for compatibility on real operating systems, but not really needed for an educational project...

## Building and Running

Before building, make sure you have finished setting up the environment. Open the Makefile and make sure all executable paths are correct (in particular QEMU which is currently set to the Windows executable).

To build only, use `make`. This will assemble, compile, link everything and create the OS disk image.

To build and run with QEMU, use `make run`. The serial port output (with debugging messages) will be redirected to stdout and `serial.log`.

To build, run and debug with GDB, use `make debug`. This will automatically start GDB and freeze the CPU before any code is executed. This option creates a symbol file `kernel.elf` which is used by GDB. This file can also be used for disassembly (for example using IDA).

To clean all build products, use `make clean`.

## Directory Map

- `arch` - Architecture specific code. Currently only x86 is supported. Other locations also contain some architecture specific code.
  - `arch/x86/realmode` - Real mode code that is used as a 2nd stage bootloader and also for servicing interrupts that use real mode BIOS utilities.
- `boot` - First stage bootloader code that makes up the 512-byte boot sector. `kernel_entry.asm` is not part of the boot sector.
- `drivers` - Hardware drivers.
- `kernel` All kernel core code.
  - `kernel/init` OS initialization code
- `lib` C standard library functions. All implemented functions are compliant to the specification (I hope). Some extra functions that are not part of the C standard are implemented.

## Current Features

The OS currently has a 2-stage boot loader. The first stage simply loads the second stage from the disk into memory and transfers control to it.

The second stage, written mostly in C, loads the entire kernel into memory, enters 32-bit protected mode using a minimal GDT, and transfers control to the kernel.

The kernel currently does a few initialization tasks:

- Set up the IDT (Interrupt Descriptor Table) for interrupt handling
- Install high-level ISR (Interrupt Service Routine) and IRQ (Interrupt Request) handlers (only a single ISR and a single IRQ are implemented as of now)
- Set up the PIC (Programmable Interrupt Controller) for receiving hardware interrupts
- Obtain a memory map

The kernel also features a logging mechanism based on Syslog principles (and heavily inspired by the logging mechanism in Cisco IOS). Log messages are currently printed on the screen and sent through the serial port (which gets written to a file on the host with the magic of QEMU). Each log has a priority value between 0 (emergency) and 7 (debug). Each log destination only receives logs of a configured priority and higher (lower value).

The OS has a few device drivers, most of them are partial and only the necessary features are implemented:

- **Screen** - standard text-mode VGA driver with limited support for shell-like control (moving the cursor, locking rows, scrolling the screen)
- **Serial** - serial port driver, only able to send data (no input or control mechanisms implemented), currently used only for logging
- **Keyboard** - very partial, can only handle printable characters, SHIFT/CAPSLOCK and arrow keys
- **PIC** - used to initialize the PIC

## What's Next?

The following are the next things that need to be worked on.

##### Better printing

The implementation of vsnprintf needs to be extended to support printing 64-bit integers. Also, support for tabs needs to be added to the screen.

##### Kernel memory management

The kernel cannot perform some more advanced tasks without dynamic memory allocation. A kernel heap needs to be implemented, along with memory management functions such as malloc and friends.

## Read List & Technical References

The following are the articles, books and technical references that I used or plan to use.

- *[Writing a Simple Operating System — from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)* - A great introduction to the world of OS development. It's not finished (and probably never will be), but it currently provides a very in depth yet interesting guide to writing a simple bootloader and launching a kernel written in C.
- *[Roll your own toy UNIX-clone OS](http://www.jamesmolloy.co.uk/tutorial_html/)* - A great guide to understanding and implementing some of the core features of an OS kernel. It has concise explanations and many code examples.
- *[os-tutorial](https://github.com/cfenollosa/os-tutorial)* - A Github repository that serves as a tutorial for building an OS. It has 24 lessons, each featuring a README with an explanation, and complete code of the example OS so far. It is very much a compilation and refactor of the 2 previous sources.
- *[The little book about OS development](https://littleosbook.github.io/)* - Another great guide to building an operating system. It covers many of the same topics as *Roll your own UNIX-clone OS*, but does so differently and is not based on it. It focuses more on theory than on code examples.
- *[Many But Finite](https://manybutfinite.com/archives/)* - A blog with excellent posts about many operating system concepts. I very much recommend reading all posts (except the occasional off-topic post), from oldest to newest.
- *[OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page)* - The bible of OS development. It contains an abundance of articles on many subjects. It is very technical in nature, and is not very friendly at first, especially if you do not know the theory behind the topic you're reading on. It also has a forum where veteran OS developers answer questions.
- *[C Standard Library Reference](https://www.tutorialspoint.com/c_standard_library/index.htm)* - A reference for C standard library functions.
- *[Linux Kernel Source](https://github.com/torvalds/linux)* - a real OS source can be a very useful asset for understanding how OS features are implemented.
- *[Linux Kernel v1.0 Source](https://mirrors.edge.kernel.org/pub/linux/kernel/v1.0/)* - First version of the Linux kernel. There is **significantly** less code than the newest version, and it is much more concise. Often using the newest version as a reference reveals bloated, hard to comprehend code because it has so much compatibility considerations and extra features. This early release code is usually straight to the point (and much easier to navigate).
- *[Understanding the Linux Kernel](https://doc.lagout.org/operating%20system%20/linux/Understanding%20Linux%20Kernel.pdf)* - A book that covers many aspects of the Linux kernel (as of version 2.6). I haven't had a chance to use it yet, but it seems like it could provide insights on how to implement many kernel functions.
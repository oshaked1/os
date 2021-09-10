# My OS (still needs a name)

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

- There is barely any beginner-friendly information available (let alone tutorials)
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
- Build environment with C standard libraries (like **libc**)
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
export TARGET=i386-elf
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

#### IA16 Binutils and GCC

To compile code for 16-bit real mode, we will need to build Binutils and GCC for the IA16 target. This target is not supported by standard GCC, so we will need to obtain and build an IA16 fork of GCC. Before building, make sure you have **bison** and **autoconf** installed: `apt install bison autoconf`

Obtain and build Binutils and GCC for IA16 as following:

```bash
mkdir /tmp/src
cd /tmp/src
git clone https://github.com/tkchia/build-ia16.git # repository with build scripts
cd build-ia16
./fetch.sh # clones gcc-ia16 from github along with other repositories needed for building
./build.sh clean
./build.sh binutils
./build.sh prereqs
./build.sh gcc1
./build.sh newlib
./build.sh libi86
./build.sh gcc2
mv prefix /usr/local/ia16elfgcc # move the output directory to the same place as the rest of our installation dirs
cd ..
rm -rf build-ia16
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

- `OS/arch` - Architecture specific code. Currently only x86 is supported. Other locations also contain some architecture specific code.
  - `OS/arch/x86/realmode` - Real mode code that is used as a 2nd stage bootloader and also for servicing interrupts that use real mode BIOS utilities.
- `OS/boot` - First stage bootloader code that makes up the 512-byte boot sector. `kernel_entry.asm` is not part of the boot sector.
- `OS/drivers` - Hardware drivers.
- `OS/kernel` All kernel core code.
  - `OS/kernel/init` OS initialization code
- `OS/libc` C standard library functions. All implemented functions are compliant to the specification (I hope). Some extra functions that are not part of the C standard are implemented.

## Current Features

The OS currently has a minimal boot loader. All it does is load the kernel into memory, load a minimal GDT, enter 32 bit protected mode and launch the kernel. It is a single stage boot loader and fits entirely into the boot sector of the disk.

The kernel currently does a few initialization tasks:

- Set up the IDT (Interrupt Descriptor Table) for interrupt handling
- Install high-level ISR (Interrupt Service Routine) and IRQ (Interrupt Request) handlers (only a single ISR and a single IRQ are implemented as of now)
- Set up the PIC (Programmable Interrupt Controller) for receiving hardware interrupts

The kernel also features a logging mechanism based on Syslog principles (and heavily inspired by the logging mechanism in Cisco IOS). Log messages are currently printed on the screen and sent through the serial port (which gets written to a file on the host with the magic of QEMU). Each log has a priority value between 0 (emergency) and 7 (debug). Each log destination only receives logs of a configured priority and higher (lower value).

The OS has a few device drivers, most of them are partial and only the necessary features are implemented:

- **Screen** - standard text-mode VGA driver
- **Serial** - serial port driver, only able to send data (no input or control mechanisms implemented), currently used only for logging
- **Keyboard** - very partial, can only handle printable characters (not even modifiers like CTRL, SHIFT)
- **PIC** - used to initialize the PIC

## What's Next?

The following are the next things that need to be worked on.

##### More robust boot loader

One major problem I currently have, is that the entire kernel is loaded from the 16-bit real mode boot loader, which has only 1MB of addressable memory. This means that the theoretical maximum kernel size I can load is 1MB (in practice significantly less, more like ~500KB). While this sounds like enough, there is an additional limitation - in 16-bit real mode, only 0xFFFF bytes of memory (64KB) can be accessed at once, without using complex segment register manipulations (which allow accessing the full 1MB address space, 64KB at a time). The current kernel size is 9KB, and growing. This means that very soon, a more sophisticated kernel loading mechanism will be needed. Such a mechanism will probably not fit into the 512 byte boot sector, which means we will need a 2-stage boot loader.

The 1st stage (which is contained in the boot sector) will load the 2nd stage (which will be larger) into memory, initialize a stack for it, and pass control to it. The 2nd stage will be written in C (and compiled to 16-bit code using a fork of GCC), and along with some more complex initialization tasks (like obtaining a full memory map), will load the actual kernel into memory, and pass control to it.

##### 16-bit real mode kernel services

Many platform services (like obtaining a memory map) are only available in real mode, which means that the kernel must transition into real mode, perform whatever it needs, and go back into protected mode.

Instead of compiling different parts of the kernel for different targets (32-bit and 16-bit), I plan to provide all real mode services as a real mode IRQ handler. The real mode code that is loaded as the 2nd stage boot loader will also include many real mode services for the kernel to use.

Whenever the kernel needs one of these services, it issues a predefined interrupt with parameters (like the requested service) passed on some registers. The IRQ for that interrupt will transition into real mode, and jump to the beginning of the 2nd stage boot loader, with all the service request parameters passed on to its stack. The boot loader code will check its stack for parameters and will thus know if it is executing as a boot loader, or as a service provider (with the requested service in the given parameters).

Output from real mode services will be written to memory at a location given as a parameter.

##### Kernel memory management

The kernel cannot perform some more advanced tasks without dynamic memory allocation. A kernel heap needs to be implemented, along with memory management functions such as malloc and friends.
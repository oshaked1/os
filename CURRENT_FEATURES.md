The following are the currently implemented features of the OS.

# Build System

- Fully automated build system using a Makefile

- External debugging using QEMU/GDB

- Build produces a raw disk image that is used by QEMU and outputs ELF files for the kernel and realmode code for debugging

# Bootloader

- Stage 1 bootloader (in the disk's boot sector) that loads the stage 2 bootloader into memory (immediately following the boot sector on disk)

- Stage 2 bootloader which loads the kernel code from disk, enters 32-bit protected mode and jumps to the kernel entry

# Real Mode Services

- Even after starting the kernel, BIOS services may still be desirable (e.g. obtaining a memory map). The real mode services system offers a syscall-like interface to transition into 16-bit real mode and perform a desired service.

- The real mode services code is contained in the 2nd stage bootloader image, but its code is independent from the actual bootloader code.

- Whenever the kernel wants to use a real mode service, it builds a service packet which details its request and performs an `int 0x81`.

- The interrupt handler jumps to a static location in the 2nd stage bootloader image, which transitions into 16-bit real mode and performs the requested service.

- After servicing the request, the real mode code transitions back into 32-bit protected mode and resumes execution in the kernel.

# Kernel

The kernel has some groundwork laid down, but it doesn't really do anything yet.

## Initialization

- Initialize the screen for printing

- Install interrupt handling capabilities

- Call real mode services to obtain a memory map and initialize the kernel memory layout

## Memory Management

- Divide the available memory into regions for different purposes (kernel code, heap, stack, etc.)

- Initialize a kernel heap (based on a buddy allocator)

- Provides `kmalloc()` and `kfree()` functions for simple memory allocation on the kernel heap

## Logging Mechanism

- Syslog-like logging mechanism for information and debugging

- Logs are printed to the screen and sent through the serial port (for external debugging)

- Each log destination can be configured to receivce logs of a specified severity (or higher)

## Device Drivers

- Simple PIC driver for setting up interrupt handling

- Screen driver (standard VGA text-mode) for printing log messages and keyboard input. Supports async printing of log messages so that keyboard input is not interrupted.

- Keyboard driver (PS/2) which prints the input onto the screen (supports moving the cursor and scrolling when writing past the end of the screen)

## Library Functions

- Various C standard library functions are implemented (they follow the specification as much as possible)

- Format printing capabilities with a subset of the C standard library printf format capabilites

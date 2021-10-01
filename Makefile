C_SOURCES = $(wildcard kernel/*.c drivers/*.c lib/*.c arch/x86/*.c mm/*.c)
C_REALMODE_SOURCES = $(wildcard arch/x86/realmode/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h lib/*.h arch/x86/*.h mm/*.h)
REALMODE_HEADERS = $(wildcard arch/x86/realmode/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}
OBJ16 = ${C_REALMODE_SOURCES:.c=.o16}

# Debug settings
DEBUG          = TRUE  # extra debugging messages (can be very verbose)
REALMODE_DEBUG = FALSE # print real mode service messages to screen

# Size of binary files after padding - MUST BE DIVISIBLE BY 512 (sector size)
REALMODE_SIZE = 4096
KERNEL_SIZE   = 32768

# Binary file loading addresses
REALMODE_LOAD_ADDRESS = 0x1000
DISK_LOAD_ADDRESS     = 0x10000
KERNEL_LOAD_ADDRESS   = 0x100000

# Realmode and Protected mode stacks
REALMODE_STACK       = 0x9000
PROTECTED_MODE_STACK = 0x80000

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb

# -g: Use debugging symbols in gcc
CFLAGS = -g

### Preprocessor definitions

# Debug definitions based on debug settings
ifeq (${strip ${DEBUG}}, TRUE)
DEBUG_DEFS = -D __DEBUG__
endif
ifeq (${strip ${REALMODE_DEBUG}}, TRUE)
DEBUG_DEFS := ${DEBUG_DEFS} -D __REALMODE_DEBUG__
endif

# Full definition lists
CC_DEFS = ${DEBUG_DEFS} -D REALMODE_SECTORS=${shell expr ${REALMODE_SIZE} / 512} -D KERNEL_SIZE=${KERNEL_SIZE} -D DISK_LOAD_ADDRESS=${DISK_LOAD_ADDRESS} -D KERNEL_LOAD_ADDRESS=${KERNEL_LOAD_ADDRESS} -D REALMODE_LOAD_ADDRESS=${REALMODE_LOAD_ADDRESS} -D PROTECTED_MODE_STACK=${PROTECTED_MODE_STACK}
NASM_DEFS = -D REALMODE_LOAD_ADDRESS=${REALMODE_LOAD_ADDRESS} -D REALMODE_SECTORS=${shell expr ${REALMODE_SIZE} / 512} -D DISK_LOAD_ADDRESS=${DISK_LOAD_ADDRESS} -D KERNEL_LOAD_ADDRESS=${KERNEL_LOAD_ADDRESS} -D REALMODE_STACK=${REALMODE_STACK} -D PROTECTED_MODE_STACK=${PROTECTED_MODE_STACK}

# QEMU
# My environment is WSL1 which has no graphic capabilites.
# I use the Windows executable from within WSL (see https://docs.microsoft.com/en-us/windows/wsl/interop)
QEMU = qemu-system-i386.exe
# Redirect serial output to stdout and a logfile, don't reboot (so triple faults don't create an infinite boot loop)
QEMUFLAGS = -chardev stdio,id=char0,logfile=serial.log,signal=off -serial chardev:char0 -no-reboot -m 128M

# First rule is run by default
os-image.bin: boot/bootsector.bin realmode.bin kernel.bin
	cat $^ > $@

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually
kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext ${KERNEL_LOAD_ADDRESS} $^ --oformat binary
	dd if=/dev/zero of=$@ bs=1 count=1 seek=${shell expr ${KERNEL_SIZE} - 1}

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext ${KERNEL_LOAD_ADDRESS} $^

realmode.bin: arch/x86/realmode/realmode_entry.o ${OBJ16}
	${LD} -o $@ -Ttext ${REALMODE_LOAD_ADDRESS} $^ --oformat binary
	dd if=/dev/zero of=$@ bs=1 count=1 seek=${shell expr ${REALMODE_SIZE} - 1}

# Used for debugging purposes
realmode.elf: arch/x86/realmode/realmode_entry.o ${OBJ16}
	${LD} -o $@ -Ttext ${REALMODE_LOAD_ADDRESS} $^

run: os-image.bin
	${QEMU} ${QEMUFLAGS} -drive format=raw,file=$<
	
# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf realmode.elf
	${QEMU} ${QEMUFLAGS} -s -S -drive format=raw,file=$< &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

debug-realmode: os-image.bin kernel.elf realmode.elf
	${QEMU} ${QEMUFLAGS} -s -S -drive format=raw,file=$< &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file realmode.elf"

# Place release binaries in a dedicated directory
release: os-image.bin kernel.elf realmode.elf
	cp os-image.bin release/os-image.bin
	cp kernel.elf release/kernel.elf
	cp realmode.elf release/realmode.elf

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} ${CC_DEFS} -ffreestanding -c $< -o $@

%.o16: %.c ${REALMODE_HEADERS}
	${CC} ${CFLAGS} ${CC_DEFS} -m16 -ffreestanding -c $< -o $@

%.o: %.asm
	nasm ${NASM_DEFS} $< -f elf -o $@

%.bin: %.asm
	nasm ${NASM_DEFS} $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o lib/*.o arch/x86/*.o mm/*.o
	rm -rf arch/x86/realmode/*.o arch/x86/realmode/*.o16 arch/x86/realmode/*.bin
	rm -f serial.log

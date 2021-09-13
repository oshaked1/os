C_SOURCES = $(wildcard kernel/*.c drivers/*.c lib/*.c arch/x86/*.c kernel/init/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h lib/*.h arch/x86/*.h kernel/init/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
CC16 = /usr/local/ia16elfgcc/bin/ia16-elf-gcc
LD16 = /usr/local/ia16elfgcc/bin/ia16-elf-ld
# -g: Use debugging symbols in gcc
CFLAGS = -g

# QEMU
# My environment is WSL1 which has no graphic capabilites.
# We use the Windows executable from within WSL (see https://docs.microsoft.com/en-us/windows/wsl/interop)
QEMU = qemu-system-i386.exe
# redirect serial output to stdout and a logfile
QEMUFLAGS = -chardev stdio,id=char0,logfile=serial.log,signal=off -serial chardev:char0 -no-reboot

# First rule is run by default
os-image.bin: boot/bootsector.bin kernel.bin
	cat $^ > $@
	dd if=/dev/zero of=$@ bs=1 count=1 seek=1048575

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	${QEMU} ${QEMUFLAGS} -drive format=raw,file=$<
	
# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	${QEMU} ${QEMUFLAGS} -s -S -drive format=raw,file=$< &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o lib/*.o arch/x86/*.o kernel/init/*.o
	rm -f serial.log

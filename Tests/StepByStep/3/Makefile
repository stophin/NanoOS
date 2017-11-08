# Automatically generate lists of sources using wildcard
SOURCES = $(wildcard kernel/*.c lib/*.c )
SYSTEMC = $(wildcard kernel/*.asm)
HEADERS = $(wildcard kernel/*.h lib/*.h)

# Convert the *.c filenames to *.o to give a  list of object files to build
# please notice the order of the object
# you can add external object which created by other tools
ENREY =  kernel/kernel_entry.o
EXTERNAL =  
OBJECT = $(SYSTEMC:.asm=.o) ${SOURCES:.c=.o}

# Output image directory
IMAGE_DIR = ./

# OS name
IMAGE = ${IMAGE_DIR}nano

# Default build target
all: image

# This is the actual disk image that the computer loads
# which is the combination of our compiled bootsector and kernel
image : boot/boot.bin boot/loader.bin ${IMAGE_DIR}kernel.bin
	cp $< ${IMAGE_DIR}
	cat $^ > ${IMAGE}.bin
	dd if=${IMAGE}.bin of=${IMAGE}.img bs=1440K count=1 conv=notrunc

# This builds the binary of our kernel from two object files:
# - the kernel entry, which jumps into main() in our kernel
# - the compiled C kernel
${IMAGE_DIR}kernel.bin : ${ENREY} ${EXTERNAL} ${OBJECT}
	ld -o ${IMAGE_DIR}kernel.elf -s -Ttext 0x9000 -e main -m elf_i386 $^
	objcopy -I elf32-i386 -O binary -R .note -R .comment -S ${IMAGE_DIR}kernel.elf $@

# Generic rule for compiling C code to an object file
# For simplicity, we C files depend on all header files
%.o : %.c ${HEADERS}
	gcc -O0 -ffreestanding -m32 -c $< -o $@

# Assemble the kernel entry
%.o : %.asm
	nasm $< -f elf32 -o $@

# Assemble binary
%.bin : %.asm
	nasm $< -f bin -o $@ -I boot/

# Clean
clean:
	rm -rf ${IMAGE_DIR}*.bin ${IMAGE_DIR}*.elf boot/boot.bin boot/loader.bin
	rm -rf ${ENTRY} ${OBJECT}


# White image
raw :
	dd if=/dev/zero of=${IMAGE}.img bs=1440K count=1
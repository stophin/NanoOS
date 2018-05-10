# Automatically generate lists of sources using wildcard
SOURCES = $(wildcard kernel/*.c memory/*.c drivers/*.c interrupt/*.c lib/*.c res/*.c tiny/*.c)
SYSTEMC = $(wildcard kernel/*.asm)
HEADERS = $(wildcard kernel/*.h memory/*.h drivers/*.h interrupt/*.h lib/*.h res/*.h tiny/*.h)

# Convert the *.c filenames to *.o to give a  list of object files to build
# please notice the order of the object
# you can add external object which created by other tools
ENTRY =  kernel/kernel_entry.o 
EXTERNAL =  font/_font.o
OBJECT = $(SYSTEMC:.asm=.o) ${SOURCES:.c=.o}
LOADER =  boot/loader.bin
BOOT = boot/boot.bin

# Output image directory
IMAGE_DIR = image/

# OS name
IMAGE = ${IMAGE_DIR}nano

# Default build target
all: image

# This is the actual disk image that the computer loads
# which is the combination of our compiled bootsector and kernel
image : ${BOOT} ${LOADER} ${IMAGE_DIR}kernel.bin
	cp ${BOOT} ${LOADER} ${IMAGE_DIR}
	cat $^ > ${IMAGE}.bin
	dd if=${IMAGE}.bin of=${IMAGE}.img bs=1440K count=1 conv=notrunc

# This builds the binary of our kernel from two object files:
# - the kernel entry, which jumps into main() in our kernel
# - the compiled C kernel
${IMAGE_DIR}kernel.bin : ${ENTRY} ${OBJECT}
	#ld -Ttext 0x1000 --oformat binary -e main -m elf_i386 -o $@ $^
	ld -o ${IMAGE_DIR}kernel.elf -s -Ttext 0x1000 -e main -m elf_i386  ${EXTERNAL} $^
	objcopy -I elf32-i386 -O binary -R .note -R .comment -S ${IMAGE_DIR}kernel.elf $@

# Generic rule for compiling C code to an object file
# For simplicity, we C files depend on all header files
%.o : %.c ${HEADERS}
	gcc -O0 -ffreestanding -m32 -c $< -o $@

# Assemble the kernel entry
%.o : %.asm
	nasm $< -f elf32 -o $@ -I boot/

# Assemble binary
%.bin : %.asm
	nasm $< -f bin -o $@ -I boot/

# White image
raw :
	dd if=/dev/zero of=${IMAGE}.img bs=1440K count=1

# Inspect ndisasm
asm : ${IMAGE_DIR}kernel.bin
	ndisasm -b 32 $<

# Inspect objdump
dump: kernel/kernel.o
	objdump -d $<

# Clean
clean:
	rm -rf ${IMAGE_DIR}*.bin ${IMAGE_DIR}*.elf ${BOOT} ${LOADER}
	rm -rf ${ENTRY} ${OBJECT} ${LOADER} ${LOADER_ELF}

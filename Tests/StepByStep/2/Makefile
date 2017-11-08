IMAGE_DIR = 
IMAGE = ${IMAGE_DIR}nano
image : boot.bin loader.bin
	cat $^ > ${IMAGE}.bin
	dd if=${IMAGE}.bin of=${IMAGE}.img bs=1440K count=1 conv=notrunc

%.bin : %.asm
	nasm $< -f bin -o $@ -I boot/

# White image
raw :
	dd if=/dev/zero of=${IMAGE}.img bs=1440K count=1
	
clean:
	rm -rf *.bin
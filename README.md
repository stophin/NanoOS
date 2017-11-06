# NanoOS  
NanoOS: An opertating system from scratch, using TinyCore and Self-Made memory management  
  
HOWTO:  
1. Can only compiled with Linux OS. Needed tools:  
 gcc, nasm, dd  
  Use the following commands to build an img:  
  $make clean  
  $make raw  
  $make  
    
2. Can run image in Linux or Windows.   
 Linux:  
 	Needed VM tools: bochs, and X-system  
	$cd image  
	$bochs -f nano_linux.bxrc  
	if you changed the location or configuration of bochs then you need to change .bxrc  to fit your system  
 Window:  
 	Change image/nano.bxrc (path of romimage or vgaromimage) to fit your system,   
 	then double click the .bxrc.  
 	  
ABOUT:  
  
1. TinyCore's source is in tiny/ folder. It's using Time slice polling techineque.  
  It depends on Intel's TSS mechanism, so it's lack of platform compatibility.  
    
2. Memory management is also in tiny/ folder. It's managing a pre-defined space.   
 This pre-fefined space is the address of the memory, so it can be tested given an  
 char array and it's address. You can run the test in Tests/ folder.  
  
TODO:  
1. Known BUG:  
 It can also run in qemu, but the floppy read BIOS program in qemu seems so different  
 that I can't get the kernel readed into memory, so I've no ideal.  
   
2. I don't quite understand the paging mechanism, so this is what to do and learn next.
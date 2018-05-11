#include "../memory/memory.h"
#include "../kernel/system_call.h"

MEMMAN memman;

void memman_init(MEMMAN * man) {
	man->blocks = 0;		// available blocks' count
	man->maxblocks = 0;		// high water level of blocks' count
	man->lostsize = 0;		// release failed size
	man->losts = 0;			// release failed times

	return;
}

// all memory size
DWORD memman_total(MEMMAN * man) {
	DWORD i, t = 0;
	for (i = 0; i < man->blocks; i ++) {
		t += man->block[i].size;
	}
	return t;
}

// allocate memory of size
DWORD memman_alloc(MEMMAN * man, DWORD size) {
	DWORD i, a;
	for ( i = 0; i < man->blocks; i++) {
		if (man->block[i].size >= size) {
			// found enough size of memory
			a = man->block[i].addr;
			man->block[i].addr += size;
			man->block[i].size -= size;
			if (man->block[i].size == 0) {
				// this block's size becomes 0 , delete it
				man->blocks--;
				// the rest of the blocks shift forward
				for (; i < man->blocks; i++) {
					man->block[i] = man->block[i + 1];
				}
			}
			return a;
		}
	}
	return 0; // no avaliable block
}

// release memory
DWORD memman_free(MEMMAN * man, DWORD addr, DWORD size) {
	DWORD i, j;
	// order by address
	for (i = 0; i < man->blocks; i++) {
		if (man->block[i].addr > addr) {
			break;
		}
	}
	// block[i-1].addr < addr < block[i].addr
	if (i > 0) {
		if (man->block[i - 1].addr + man->block[i - 1].size == addr) {
			// conjunct with the block forward
			man->block[i - 1].size += size;
			if (i < man->blocks) {
				if (addr + size == man->block[i].addr) {
					// conjunct with the block afterward
					man->block[i - 1].size += man->block[i].size;
					// delete block man->block[i]
					man->blocks --;
					for (; i < man->blocks; i ++) {
						man->block[i] = man->block[i + 1];
					}
				}
			}
			return 0;
		}
	}
	// couldn't conjunct with the block forward
	if (i < man->blocks) {
		if (addr + size == man->block[i].addr) {
			// conjunct with the block afterward
			man->block[i].addr = addr;
			man->block[i].size += size;
			return 0;
		}
	}
	// couldn't conjunct with either
	if (man->blocks < MEMMAN_BLOCKS) {
		// move afterward to make space for new block
		for (j = man->blocks; j > i; j --) {
			man->block[j] = man->block[j - 1];
		}
		man->blocks ++;
		if (man->maxblocks < man->blocks) {
			man->maxblocks = man->blocks;	// hight water level
		}

		man->block[i].addr = addr;
		man->block[i].size = size;
		return 0;
	}
	// couldn't move afterward
	man->losts ++;
	man->lostsize += size;
	return -1;
}

DWORD memman_alloc_4k(MEMMAN * man, DWORD size) {
	DWORD a;
	size = (size * 32 + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

DWORD memman_free_4k(MEMMAN * man, DWORD addr, DWORD size) {
	DWORD i;
	size = (size * 32 + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}

DWORD mem_test(DWORD start, DWORD end) {
	BYTE flag486 = 0;
	DWORD eflags, cr0, i;

	// confirm 386 or > 486
	eflags = io_load_eflags();
	eflags |= 0x00040000;		// AC bit = 1
	io_store_eflags(eflags);
	
	eflags = io_load_eflags();
	if ((eflags & 0x00040000) != 0) {// AC bit will return to 0 if 386
		flag486 = 1;
	}
	eflags &= ~0x00040000;		// AC bit = 0
	io_store_eflags(eflags);

	if (flag486) {
		cr0 = load_cr0();
		cr0 |= 0x60000000;		// disable cache using cr0
		store_cr0(cr0);
	}

	// use asmembly to avoid optimization of the c code
	//i = mem_test_sub(start, end);
	i = check_mem(start, end);

	if (flag486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~0x60000000;		// enable cache using cr0
		store_cr0(cr0);
	}

	return i;
}

DWORD mem_test_sub(DWORD start, DWORD end) {
	DWORD i, *p, old, pat0 = 0x0aa55aa55, pat1 = 0x55aa55aa;

	for (i = start; i <= end; i+= 0x1000) {	// check every 4KB
		p = (DWORD *) (i + 0xffc);	// check at the end of 4KB
		old = *p;			// remember value in test address
		*p = pat0;			// write
		*p ^= 0xffffffff;	// reverse
		if (*p != pat1) {	// check result
			*p = old;		// return value
			break;
		}
		*p ^= 0xffffffff;	// reverse again
		if (*p != pat0) {	// check recoverage
			*p = old;
			break;
		}
		*p = old;
	}
	return i;
}
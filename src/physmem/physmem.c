#include "physmem.h"

#include "std/mem.h"

extern uintptr_t __ld_kernel_end;

static size_t oxy_data freePages = 0;
static size_t oxy_data usedPages = 0;
static size_t oxy_data totalPages = 0;
static uint8_t oxy_data bitmap[MAX_PAGES / 8];

#define isFree(idx) !(bitmap[(idx) >> 3] & (1 << ((idx) & 7)))
#define markFree(idx) bitmap[(idx) >> 3] &= ~(1 << ((idx) & 7))
#define markUsed(idx) bitmap[(idx) >> 3] |= (1 << ((idx) & 7))

static uintptr_t* fetchPte(void* addr) {
	uintptr_t cr3;
	asm volatile("mov %%cr3, %0" : "=r"(cr3));

	uintptr_t* pml4 = (uintptr_t*)cr3;
	if (!pml4)
		return 0;

	uintptr_t va = (uintptr_t)addr;
	size_t id0 = (va >> 39) & 0x1FF;
	size_t id1 = (va >> 30) & 0x1FF;
	size_t id2   = (va >> 21) & 0x1FF;
	size_t id3   = (va >> 12) & 0x1FF;

	uintptr_t* pdpt = (uintptr_t*)(pml4[id0] & ~0xFFF);
	if (!pdpt) return 0;

	uintptr_t* pd = (uintptr_t*)(pdpt[id1] & ~0xFFF);
	if (!pd) return 0;

	uintptr_t* pt = (uintptr_t*)(pd[id2] & ~0xFFF);
	if (!pt) return 0;

	return &pt[id3];
}

void pmm_init() {
	totalPages = MAX_PAGES;
	memset(bitmap, 0, sizeof(bitmap));

	freePages = totalPages;
	usedPages = 0;

	for (size_t i = 0; i < (((uintptr_t)&__ld_kernel_end + 0xFFF) & ~0xFFF) / 0x1000; i++) {
		markUsed(i);
		freePages--; usedPages++;
	}
}

void* allocpage(uint32_t prot) {
	if (freePages == 0)
		return 0;

	for (size_t i = 0; i < totalPages; i++) {
		if (isFree(i)) {
			markUsed(i);
			pmm_protect((void*)(i * 0x1000), prot);
			freePages--;
			usedPages++;
			return (void*)(i * 0x1000);
		}
	}
	return 0;
}

void* allocpages(uint32_t npages, uint32_t prot) {
	if (npages == 0 || freePages < npages)
		return 0;

	for (size_t i = 0; i <= totalPages - npages; i++) {
		size_t j;
		for (j = 0; j < npages; j++) {
			if (!isFree(i + j))
				break;
		}

		if (j == npages) {
			for (size_t k = 0; k < npages; k++) {
				markUsed(i + k);
				pmm_protect((void*)((i + k) * 0x1000), prot);
			}

			freePages -= npages;
			usedPages += npages;

			return (void*)(i * 0x1000);
		}
	}

	return 0;
}

void freepage(void* addr) {
	size_t page = (uintptr_t)addr / 0x1000;
	if (page >= totalPages || isFree(page))
		return;

	pmm_protect(addr, PAGE_PROT_NOACCESS);
	markFree(page);
	freePages++;
	usedPages--;
}

void freepages(void* addr, uint32_t npages) {
	for (uint32_t i = 0; i < npages; i++)
		freepage((char*)addr + i * 0x1000);
}

uint32_t pmm_protect(void* addr, uint32_t prot) {
	uintptr_t* pte = fetchPte(addr);
	if (!pte)
		return 0;

	uintptr_t entry = *pte;
	uint32_t old = pmm_getProtection(addr);

	if (prot & PAGE_PROT_WRITE)		entry |= PAGE_PROT_WRITE;
	else							entry &= ~PAGE_PROT_WRITE;
	if (prot & PAGE_PROT_EXECUTE)	entry &= ~(1uLL<<63);
	else							entry |= 1uLL<<63;

	// we can't make a page write-only :C
	if (prot != PAGE_PROT_NOACCESS)
		entry |= PAGE_PROT_READ;

	*pte = entry;
	asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
	return old;
}

uint32_t pmm_getProtection(void* addr) {
	uintptr_t* pte = fetchPte(addr);
	if (!pte)
		return 0;
	
	uint32_t prot = *pte & 0xFFF;
	if (*pte & (1ULL << 63))
		prot |= PAGE_PROT_EXECUTE;
	return prot;
}

size_t pmm_getFreePageCount() { return freePages; }
size_t pmm_getUsedPageCount() { return usedPages; }
size_t pmm_getTotalPageCount() { return totalPages; }

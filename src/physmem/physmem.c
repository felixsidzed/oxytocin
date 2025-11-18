#include "physmem.h"

#include "std/mem.h"

extern uintptr_t __ld_kernel_end;

static size_t oxy_nobss freeBytes = 0;
static size_t oxy_nobss usedBytes = 0;
static size_t oxy_nobss totalBytes = 0;
static uint8_t oxy_nobss bitmap[MAX_PAGES / 8];

#define isFree(idx) !(bitmap[(idx) >> 3] & (1 << ((idx) & 7)))
#define markFree(idx) bitmap[(idx) >> 3] &= ~(1 << ((idx) & 7))
#define markUsed(idx) bitmap[(idx) >> 3] |= (1 << ((idx) & 7))

void pmm_init(size_t size) {
	totalBytes = size / 0x1000;
	if (totalBytes > MAX_PAGES)
		totalBytes = MAX_PAGES;

	memset(bitmap, 0, sizeof(bitmap));

	freeBytes = totalBytes;
	usedBytes = 0;

	for (size_t i = 0; i < ((uintptr_t)&__ld_kernel_end / 0x1000); i++) {
		markUsed(i);
		freeBytes--; usedBytes++;
	}
}

void* allocpage() {
	if (freeBytes == 0)
		return 0;

	for (size_t i = 0; i < totalBytes; i++) {
		if (isFree(i)) {
			markUsed(i);
			freeBytes--;
			usedBytes++;
			return (void*)(i * 0x1000);
		}
	}
	return 0;
}

void* allocpages(uint32_t npages) {
	if (npages == 0 || freeBytes < npages)
		return 0;

	for (size_t i = 0; i <= totalBytes - npages; i++) {
		uint32_t j;
		for (j = 0; j < npages; j++)
			if (!isFree(i + j)) break;

		if (j == npages) {
			for (uint32_t k = 0; k < npages; k++) {
				markFree(i + k);
				freeBytes--;
				usedBytes++;
			}
			return (void*)(i * 0x1000);
		}
	}
	return 0;
}

void freepage(void* addr) {
	size_t page = (uintptr_t)addr / 0x1000;
	if (page >= totalBytes || isFree(page))
		return;

	markFree(page);
	freeBytes++;
	usedBytes--;
}

void freepages(void* addr, uint32_t npages) {
	for (uint32_t i = 0; i < npages; i++)
		freepage((char*)addr + i * 0x1000);
}

size_t pmm_getFreeBytes() { return freeBytes; }
size_t pmm_getUsedBytes() { return usedBytes; }
size_t pmm_getTotalBytes() { return totalBytes; }

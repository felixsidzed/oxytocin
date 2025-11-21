#include "kheap.h"

#include <string.h>
#include "mmu/heap.h"

Heap* oxy_data kheap = nullptr;

void kheap_init(size_t size) {
	kheap = heap_create(size);
}

void* kmalloc(size_t size) {
	return heap_alloc(kheap, size);
}

void* kcalloc(size_t n, size_t size) {
	return heap_alloc(kheap, n * size);
}

void* krealloc(void* ptr, size_t size) {
	if (!ptr)
		return kmalloc(size);
		
	if (size == 0) {
		heap_free(kheap, ptr);
		return nullptr;
	}

	void* new = kmalloc(size);
	if (!new)
		return nullptr;

	size_t old = ((HeapEntry*)((char*)ptr - sizeof(HeapEntry)))->size;
	memcpy(new, ptr, old < size ? old : size);
	
	heap_free(kheap, ptr);
	return new;
}

void kfree(void* ptr) {
	heap_free(kheap, ptr);
}

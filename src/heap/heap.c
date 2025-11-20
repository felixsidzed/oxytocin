#include "heap.h"

#include "std/mem.h"
#include "physmem/physmem.h"

Heap* heap_create(size_t size) {
	if (size == 0)
		return nullptr;

	size = (size + 0xFFF) & ~0xFFF;

	Heap* heap = allocpages(size / 0x1000, PAGE_PROT_READ | PAGE_PROT_WRITE);
	if (!heap)
		return nullptr;

	heap->data = (char*)heap + sizeof(Heap);
	heap->totalBytes = size - sizeof(Heap);

	heap->head = (HeapEntry*)heap->data;
	heap->head->size = size - sizeof(Heap) - sizeof(HeapEntry);
	heap->head->used = 0;
	heap->head->next = nullptr;

	heap->usedBytes = 0;
	heap->freeBytes = heap->head->size;

	return heap;
}

void heap_destroy(Heap* heap) {
	freepages((char*)heap->data - sizeof(Heap), (heap->totalBytes + sizeof(Heap)) / 0x1000);
}

static HeapEntry* findFit(Heap* heap, size_t size) {
	HeapEntry* cur = heap->head;

	size_t bestSize = (size_t)-1;
	HeapEntry* bestEntry = nullptr;

	while (cur) {
		if (!cur->used && cur->size >= size && cur->size < bestSize) {
			bestEntry = cur;
			bestSize = cur->size;
		}
		cur = cur->next;
	}

	return bestEntry;
}

static void split(HeapEntry* entry, size_t size) {
	if (entry->size < size + sizeof(HeapEntry) + MIN_BLOCK_SIZE)
		return;

	HeapEntry* newEntry = (HeapEntry*)((uint8_t*)entry + sizeof(HeapEntry) + size);
	newEntry->size = entry->size - size - sizeof(HeapEntry);
	newEntry->used = 0;
	newEntry->next = entry->next;

	entry->size = size;
	entry->next = newEntry;
}

static void merge(Heap* heap) {
	HeapEntry* cur = heap->head;

	while (cur && cur->next) {
		if (!cur->used && !cur->next->used) {
			cur->size += sizeof(HeapEntry) + cur->next->size;
			cur->next = cur->next->next;
		} else
			cur = cur->next;
	}
}

void* heap_alloc(Heap* heap, size_t size) {
	if (!heap || size == 0)
		return nullptr;
	size = (size + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1);

	HeapEntry* entry = findFit(heap, size);
	if (!entry)
		return nullptr;

	split(entry, size);
	entry->used = true;

	heap->freeBytes -= entry->size;
	heap->usedBytes += entry->size;

	void* ptr = (void*)((uint8_t*)entry + sizeof(HeapEntry));
	memset(ptr, 0x0, size);

	return ptr;
}

void heap_free(Heap* heap, void* addr) {
	if (!heap || !addr)
		return;

	HeapEntry* entry = (HeapEntry*)((char*)addr - sizeof(HeapEntry));
	if (!entry->used)
		return;

	entry->used = false;
	heap->freeBytes += entry->size;
	heap->usedBytes -= entry->size;

	merge(heap);
}

size_t heap_getFreeBytes(Heap* heap) { return heap->freeBytes; }
size_t heap_getUsedBytes(Heap* heap) { return heap->usedBytes; }
size_t heap_getTotalBytes(Heap* heap) { return heap->totalBytes; }

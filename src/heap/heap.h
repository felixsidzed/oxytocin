#pragma once

#define MIN_BLOCK_SIZE 32

typedef struct HeapEntry {
	uint64_t size : 63;
	uint64_t used : 1;
	struct HeapEntry* next;
} HeapEntry;

typedef struct {
	void* data;
	size_t totalBytes;
	HeapEntry* head;
	size_t freeBytes;
	size_t usedBytes;
} Heap;

/// @brief Allocates and initializes a new heap structure with a given size
/// @param size The initial size (in bytes) of the heap to allocate
/// @return A pointer to the newly created `Heap` structure, or `nullptr` if allocation fails
Heap* heap_create(size_t size);

/// @brief Destroys a heap created with `heap_create`, freeing all associated memory
/// @param heap A pointer to the heap to destroy
void heap_destroy(Heap* heap);

/// @brief Allocate a block of memory from the specified heap
/// @param heap Pointer to the heap to allocate from
/// @param size The number of bytes to allocate
/// @return Pointer to the allocated memory block, or `nullptr` if allocation fails
void* heap_alloc(Heap* heap, size_t size);

/// @brief Free a previously allocated block of memory from the specified heap
/// @param heap The heap from which the memory was allocated
/// @param addr A pointer to the memory block to free
void heap_free(Heap* heap, void* addr);

/// @brief Get the total number of free bytes remaining in the heap
/// @param heap Pointer to the heap to query
/// @return The number of bytes currently available for allocation
size_t heap_getFreeBytes(Heap* heap);

/// @brief Get the total number of bytes currently in use in the heap
/// @param heap Pointer to the heap to query
/// @return The number of bytes currently allocated and in use
size_t heap_getUsedBytes(Heap* heap);

/// @brief Get the total size of the heap
/// @param heap Pointer to the heap to query
/// @return The total number of bytes available in the heap, including used and free memory
size_t heap_getTotalBytes(Heap* heap);

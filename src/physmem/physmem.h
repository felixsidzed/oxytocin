#pragma once

#define MAX_PAGES 65536

/// @brief Initializes the physical memory manager with the given memory size
/// @param size The total size of physical memory to manage
void pmm_init(size_t size);

/// @brief Allocates a single physical memory page
/// @return Pointer to the allocated physical page, or `nullptr` if allocation fails
void* allocpage();

/// @brief Allocates multiple contiguous physical memory pages
/// @param npages The number of contiguous pages to allocate
/// @return Pointer to the first page of the allocated block, or `nullptr` if allocation fails
void* allocpages(uint32_t npages);

/// @brief Frees a previously allocated physical memory page
/// @param addr Pointer to the physical page to free
void freepage(void* addr);

/// @brief Frees multiple previously allocated contiguous physical memory pages
/// @param addr Pointer to the first page of the block to free
/// @param npages The number of pages to free
void freepages(void* addr, uint32_t npages);

/// @brief Returns the total number of free bytes in physical memory
/// @return The number of bytes currently available for allocation
size_t pmm_getFreeBytes();

/// @brief Returns the total number of used bytes in physical memory
/// @return The number of bytes currently allocated
size_t pmm_getUsedBytes();

/// @brief Returns the total size of physical memory managed by the allocator
/// @return The total number of bytes of physical memory
size_t pmm_getTotalBytes();

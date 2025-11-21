#pragma once

#define MAX_PAGES 0x200

#define PAGE_NOACCESS	0
#define PAGE_READ		1
#define PAGE_WRITE		2
#define PAGE_EXECUTE	4

#define PAGE_READ_EXECUTE		PAGE_EXECUTE
#define PAGE_READWRITE			(PAGE_WRITE | PAGE_EXECUTE)
#define PAGE_READWRITE_EXECUTE	(PAGE_READ | PAGE_WRITE | PAGE_EXECUTE)

/// @brief Initializes the physical memory manager with the given memory size
void pmm_init();

/// @brief Allocates a single physical memory page
/// @return Pointer to the allocated physical page, or `nullptr` if allocation fails
void* allocpage(uint32_t prot);

/// @brief Allocates multiple contiguous physical memory pages
/// @param npages The number of contiguous pages to allocate
/// @return Pointer to the first page of the allocated block, or `nullptr` if allocation fails
void* allocpages(uint32_t npages, uint32_t prot);

/// @brief Frees a previously allocated physical memory page
/// @param addr Pointer to the physical page to free
void freepage(void* addr);

/// @brief Frees multiple previously allocated contiguous physical memory pages
/// @param addr Pointer to the first page of the block to free
/// @param npages The number of pages to free
void freepages(void* addr, uint32_t npages);

/// @brief Sets the memory protection attributes for a specific memory region
/// @param addr Pointer to the starting address of the memory region
/// @param prot The desired protection flags
/// @return Returns the previous protection flags of the memory region
uint32_t pmm_protect(void* addr, uint32_t prot);

/// @brief Retrieves the current memory protection attributes of a specific memory region
/// @param addr Pointer to the address within the memory region to query
/// @return Returns the current protection flags of the memory region
uint32_t pmm_getProtection(void* addr);

/// @brief Retrieves the number of free pages in physical memory
/// @return The total number of bytes currently available for allocation
size_t pmm_getFreePageCount();

/// @brief Retrieves the number of used pages in physical memory
/// @return The total number of bytes currently allocated
size_t pmm_getUsedPageCount();

/// @brief Retrieves the total size of physical memory managed by the allocator
/// @return The total number of bytes of physical memory
size_t pmm_getTotalPageCount();

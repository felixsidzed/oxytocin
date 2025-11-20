#pragma once

/// @brief Initializes the kernel heap with a given size
/// @param size The total size in bytes to allocate for the kernel heap
void kheap_init(size_t size);

/// @brief Allocates a block of memory of the given size and initializes it to zero
/// @param size The number of bytes to allocate
/// @return A pointer to the allocated memory, or `nullptr` if allocation fails
void* kmalloc(size_t size);

/// @brief Allocates memory for an array of `n` elements of `size` bytes each and initializes all bytes to zero
/// @param size The size of each element in bytes
/// @param n The number of elements to allocate
/// @return A pointer to the allocated memory, or `nullptr` if allocation fails.
void* kcalloc(size_t size, size_t n);

/// @brief Resizes the memory block pointed to by `ptr` to `size` bytes. Contents are preserved up to the minimum of the old and new sizes
/// @param ptr Pointer to the memory block to resize. Can be `nullptr`, in which case behaves like `kmalloc`
/// @param size The new size in bytes for the memory block
/// @return A pointer to the reallocated memory, or `nullptr` if reallocation fails
void* krealloc(void* ptr, size_t size);

/// @brief Frees a previously allocated memory block
/// @param ptr Pointer to the memory block to free. Passing `nullptr` has no effect
void kfree(void* ptr);

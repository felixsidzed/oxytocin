#pragma once

/// @brief Reads a single character from the input
/// @return The character read
int getchar();

/// @brief Reads a line of input into a buffer
/// @param buf Pointer to the buffer to store input
/// @param size Maximum number of characters to read
/// @return The number of characters actually read
size_t gets(char* buf, size_t size);

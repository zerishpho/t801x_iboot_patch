#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
typedef uint64_t size_t;

#ifndef NULL
#define NULL ((void*)0) 
#endif

void *memset(void *s, int c, size_t n);
void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen);
int memcmp(const void *b1, const void *b2, size_t len);
void *memcpy(void *dst, const void *src, size_t len);
void *memmove(void *dest, const void *src, size_t n);

#endif

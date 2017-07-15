#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stddef.h>
#include <stdio.h>

int isign (int value);

void strtcpy (char *dest, char *src, size_t dest_size);
void strtcat (char *dest, char *src, size_t dest_size);

void strtwrite (char *src, size_t max_size, FILE * stream);
void strtread (char *dest, size_t dest_size, FILE * stream);

#endif

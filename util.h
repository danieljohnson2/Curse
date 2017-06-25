#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stddef.h>

int isign (int value);

void strtcpy (char *dest, char *src, size_t dest_size);
void strtcat (char *dest, char *src, size_t dest_size);

#endif

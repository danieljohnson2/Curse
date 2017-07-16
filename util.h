#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stddef.h>
#include <stdio.h>

void error_abort (char *format, ...);
void message_abort (char *format, ...);

int isign (int value);

void strtcpy (char *dest, char *src, size_t dest_size);
void strtcat (char *dest, char *src, size_t dest_size);

void named_writef (FILE * stream, char *name, char *format, ...);
void write_bytes (char *name, unsigned char *bytes, int bytes_size,
                  FILE * stream);

void read_str (char *name, char *dest, size_t dest_size, FILE * stream);
int readf (FILE * stream, char *name, char *format, ...);
void read_bytes (char *name, unsigned char *bytes, int bytes_count,
                 FILE * stream);

#endif

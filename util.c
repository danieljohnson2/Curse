#include "util.h"

#include <string.h>

/* Returns the sign of the value givne */
int
isign (int value)
{
    if (value > 0)
        return 1;
    else if (value < 0)
        return -1;
    else
        return 0;
}

/*
A safer alternative for strcpy() which truncates rather that
exceeding the size of 'dest', given in 'dest_size'.
'dest_size' include the spae for the null terminate, which
this function will always provide.
*/
void
strtcpy (char *dest, char *src, size_t dest_size)
{
    if (dest_size > 0)
    {
        strncpy (dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

/*
A safer alternative for strcat() which truncates rather that
exceeding the size of 'dest', given in 'dest_size'.
'dest_size' include the spae for the null terminate, which
this function will always provide.
*/
void
strtcat (char *dest, char *src, size_t dest_size)
{
    if (dest_size > 0)
    {
        int len = strlen (dest);
        int remaining = dest_size - len;
        strtcpy (dest + len, src, remaining);
    }
}

/*
Write a string to a file, including a null terminator.
Writes as most 'max_size', truncating if necessary
(but never omitting then null termitor if max_size is
more than 0!
*/
void
strtwrite (char *src, size_t max_size, FILE * stream)
{
    if (max_size <= 0)
        return;

    int len = strlen (src);

    if (len >= max_size)
    {
        fwrite (src, sizeof (char), len - 1, stream);
        fputc ('\0', stream);
    }
    else
        fwrite (src, sizeof (char), len + 1, stream);
}

/*
Reads a string from a file, up to its null terminator. Or
EOF. Writes this stream into 'dest', but won't write beyond
'dest_size' chars there, and always null terminates.
If dest_size is 0 or negative, this instead reads and discards
the string.
*/
void
strtread (char *dest, size_t dest_size, FILE * stream)
{
    char *end = dest + dest_size;
    char *cur = dest;

    for (;;)
    {
        int ch = fgetc (stream);

        if (ch == EOF)
            ch = '\0';

        if (cur < end)
            *cur++ = ch;

        if (ch == '\0')
            break;
    }

    // If we fell off the end of 'dest', we must ensure it is null terminated.
    if (cur == end)
        dest[dest_size - 1] = '\0';
}

#include "util.h"

#include <string.h>
#include <stdlib.h>

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
Write a string to a file with an initial name.
*/
void
write_str (char *name, char *src, FILE * stream)
{
    fprintf (stream, "%s:%s\n", name, src);
}

void
write_double (char *name, double value, FILE * stream)
{
    fprintf (stream, "%s:%lf\n", name, value);
}

void
write_int (char *name, int value, FILE * stream)
{
    fprintf (stream, "%s:%d\n", name, value);
}

void
write_bytes (char *name, unsigned char *bytes, int bytes_size, FILE * stream)
{
    fprintf (stream, "%s:", name);

    unsigned char *end = bytes + bytes_size;
    for (unsigned char *cur = bytes; cur < end; ++cur)
    {
        int byte = *cur;
        fprintf (stream, "%02X", byte);
    }
    fputc ('\n', stream);
}

static void
read_expected_name (char *name, FILE * stream)
{
    char *cur = name;

    for (;;)
    {
        int ch = fgetc (stream);

        if (ch == ':')
            ch = '\0';

        if (*cur++ != ch)
        {
            fprintf (stderr, "Failure to read; name %s not found.", name);
            abort ();
        }
        else if (ch == '\0')
            break;
    }
}

/*
Reads a string from a file, up to its null terminator. Or
EOF. Writes this stream into 'dest', but won't write beyond
'dest_size' chars there, and always null terminates.
If dest_size is 0 or negative, this instead reads and discards
the string.
*/
void
read_str (char *name, char *dest, size_t dest_size, FILE * stream)
{
    read_expected_name (name, stream);

    char *end = dest + dest_size;
    char *cur = dest;

    for (;;)
    {
        int ch = fgetc (stream);

        if (ch == EOF || ch == '\n')
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

double
read_double (char *name, FILE * stream)
{
    char buffer[128];
    double result = 0.0;
    read_str (name, buffer, 128, stream);
    sscanf (buffer, "%lf", &result);
    return result;
}

int
read_int (char *name, FILE * stream)
{
    char buffer[128];
    int result = 0;
    read_str (name, buffer, 128, stream);
    sscanf (buffer, "%d", &result);
    return result;
}

void
read_bytes (char *name, unsigned char *bytes, int bytes_count, FILE * stream)
{
    read_expected_name (name, stream);

    char buffer[3] = { 0 };
    unsigned char *cur = bytes;
    unsigned char *end = bytes + bytes_count;

    while (cur < end)
    {
        int first = fgetc (stream);

        if (first == '\n' || first == EOF)
            break;

        int second = fgetc (stream);

        if (first == '\n' || first == EOF)
            break;

        buffer[0] = first;
        buffer[1] = second;

        int i;
        sscanf (buffer, "%02X", &i);
        *cur++ = i;
    }
}

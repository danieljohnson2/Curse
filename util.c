#include "util.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/*
Aborts the process printing an error to standard error. This also gives
the error for the current error in errno.
*/
void
error_abort (char *format, ...)
{
    va_list ap;
    va_start (ap, format);

    char buffer[4096];
    vsnprintf (buffer, 4096, format, ap);
    va_end (ap);

    perror (buffer);
    abort ();
}

/*
Aborts the process printing an error to standard error.
*/
void
message_abort (char *format, ...)
{
    va_list ap;
    va_start (ap, format);

    vfprintf (stderr, format, ap);
    va_end (ap);
    abort ();
}

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
Writes a named element out using the format given. The functio
wites the name, a colon, the payload data, and then a newline.
On error, this method aborts with a message.
*/
void
named_writef (FILE * stream, char *name, char *format, ...)
{
    va_list ap;
    va_start (ap, format);

    if (fprintf (stream, "%s:", name) < 0)
        error_abort ("Failed to write named value");

    if (vfprintf (stream, format, ap) < 0)
        error_abort ("Failed to write named value");

    if (fputc ('\n', stream) == EOF)
        error_abort ("Failed to write named value");

    va_end (ap);
}

/*
Write a byte-array to a file with an initial name. The
byte array is encoded as a sequence of hex bytes. Base64
is for people with tiny hard drives.
*/
void
write_bytes (char *name, unsigned char *bytes, int bytes_size, FILE * stream)
{
    if (fprintf (stream, "%s:", name) < 0)
        error_abort ("Failed to write bytes");

    unsigned char *end = bytes + bytes_size;
    for (unsigned char *cur = bytes; cur < end; ++cur)
    {
        int byte = *cur;
        if (fprintf (stream, "%02X", byte) < 0)
            error_abort ("Failed to write bytes");
    }

    if (fputc ('\n', stream) == EOF)
        error_abort ("Failed to write bytes");
}

/*
Reads the name given and then a ':' from the stream;
aborts with an error if that's not what it finds.
We use this to validate the input as we read it.
*/
static void
read_expected_name (char *name, FILE * stream)
{
    char *cur = name;

    for (;;)
    {
        int ch = fgetc (stream);

        if (ch == EOF && ferror (stream))
            error_abort ("Failure to read expected name");

        if (ch == ':')
            ch = '\0';

        if (*cur++ != ch)
            message_abort ("Failure to read; name %s not found.", name);
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

        if (ch == EOF && ferror (stream))
            error_abort ("Failure to read string");

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

/*
Reads a named value off the stream using a format string. returns
the number of matches, but never 0 or EOF: errors or failure to
matching anything will lead to an abort instead.
*/
int
readf (FILE * stream, char *name, char *format, ...)
{
    va_list ap;
    va_start (ap, format);

    char buffer[4096];
    read_str (name, buffer, 4096, stream);
    int count = vsscanf (buffer, format, ap);

    if (count == EOF)
        error_abort ("Failure to read value");
    else if (count == 0)
        message_abort ("Failure to parse value");

    va_end (ap);
    return count;
}

/*
Reads a byte-array from the stream.
*/
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
        if (sscanf (buffer, "%02X", &i) == EOF)
            error_abort ("Failure to read bytes");

        *cur++ = i;
    }

    if (ferror (stream))
        error_abort ("Failure to read bytes");
}

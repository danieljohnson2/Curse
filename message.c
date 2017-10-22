#include "message.h"
#include "util.h"
#include "ui.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

static char pending_message[MESSAGE_SIZE];

/*
Utility for consolidate_message().
This appends 'prior' to buffer, and if *repeats is >1, appends
a repeat indicator and resets *repeats to 1.

This then copies current into prior and clears current.

'buffer', 'prior' and 'current' should be MESSAGE_SIZE in size.
*/
static void
flush_current_buffer (char *buffer, char *prior, char *current, int *repeats)
{
    strtcat (buffer, prior, MESSAGE_SIZE);

    if (*repeats > 1)
    {
        char rpt[MESSAGE_SIZE];
        sprintf (rpt, " (x%d)", *repeats);
        strtcat (buffer, rpt, MESSAGE_SIZE);
        *repeats = 1;
    }

    strtcpy (prior, current, MESSAGE_SIZE);
    current[0] = '\0';
}

/*
This copies the message to the result buffer given; if there are
duplicate messages this will consolidate them.

The buffer should be MESSAGE_SIZE chars long, including the null
terminator.
*/
void
read_message (char *result)
{
    // An extra initial space makes matching more
    // reliable. This way every message is preceeded by
    // a space.

    char input[MESSAGE_SIZE + 1];
    input[0] = ' ';
    strtcpy (input + 1, pending_message, MESSAGE_SIZE);

    char buffer[MESSAGE_SIZE] = "";
    char prior[MESSAGE_SIZE] = "";
    char current[MESSAGE_SIZE] = "";
    char *curr_out = current;
    int repeats = 1;

    for (char *c = input; *c != '\0'; ++c)
    {
        *(curr_out++) = *c;

        if (*c == '!' || *c == '.')
        {
            if (strcmp (prior, current) == 0)
                repeats++;
            else
                flush_current_buffer (buffer, prior, current, &repeats);

            // must clear entire buffer to ensure
            // curent stays null terminated.
            memset (current, 0, MESSAGE_SIZE);
            curr_out = current;
        }
    }

    flush_current_buffer (buffer, prior, current, &repeats);
    flush_current_buffer (buffer, prior, current, &repeats);

    // skip that extra space at the start of buffer
    strtcpy (result, buffer + 1, MESSAGE_SIZE);
}

/* Removes the pending message */
void
clear_message (char *message)
{
    pending_message[0] = '\0';
}

/* Appends a message to the current message. */
void
write_message (char *message)
{
    if (pending_message[0] == '\0')
        strtcpy (pending_message, message, MESSAGE_SIZE);
    else
    {
        strtcat (pending_message, " ", MESSAGE_SIZE);
        strtcat (pending_message, message, MESSAGE_SIZE);
    }
}

/* Appends a message formatted using printf-style formatting. */
void
write_messagef (char *format, ...)
{
    va_list ap;
    va_start (ap, format);

    char msg[MESSAGE_SIZE];
    vsnprintf (msg, MESSAGE_SIZE, format, ap);
    write_message (msg);

    va_end (ap);
}

/* Displays a multi-line message, then waits for the user to hit escape */
void
long_message (char *title, char **lines)
{
    WINDOW *w =
        display_multiline_window (NULL, lines, "(esc to close)", 1, 1, 1, 1,
                                  true);

    while (wgetch (w) != ESCAPE_KEY)
        continue;

    delwin (w);
}

#include "ui.h"

#include <string.h>

/* Creates a window and populates it with text. You provide the lines in a NULL
terminated array; you can provide a title and a prompt that appear in the
borders. You also provide the margins that provide space around the lines.

You can write you own stuff into thee margins, but note that the outer edge of
the window is populated with a border in any case.

In any case, the caller must delwin() the window. */
WINDOW *
display_multiline_window (char *title, char **lines, char *prompt,
                          int top_margin, int left_margin, int bottom_margin,
                          int right_margin, bool centered)
{
    int rows, columns;
    getmaxyx (stdscr, rows, columns);

    int v_extra = top_margin + bottom_margin + 2;
    int h_extra = left_margin + right_margin + 2;

    int linecount = 0;
    int maxlen = title != NULL ? strlen (title) : 0;

    if (prompt != NULL)
    {
        int prompt_len = strlen (prompt) - h_extra + 2;

        if (prompt_len > maxlen)
            maxlen = prompt_len;
    }

    for (char **l = lines; *l != NULL; ++l)
    {
        ++linecount;
        int len = strlen (*l);
        if (len > maxlen)
            maxlen = len;
    }


    WINDOW *w = newwin (linecount + v_extra, maxlen + h_extra,
                        (rows - linecount - v_extra) / 2,
                        (columns - maxlen - h_extra) / 2);
    box (w, 0, 0);

    if (title != NULL)
        mvwaddstr (w, 0, (maxlen - strlen (title) + h_extra) / 2, title);

    if (prompt != NULL)
        mvwaddstr (w, linecount + v_extra - 1,
                   maxlen - strlen (prompt) + h_extra - 1, prompt);

    int y = top_margin + 1;

    for (char **l = lines; *l != NULL; ++l)
    {
        int len = strlen (*l);
        int placement = centered ? (maxlen - len) / 2 : 0;
        mvwaddstr (w, y, placement + left_margin + 1, *l);
        ++y;
    }

    return w;
}

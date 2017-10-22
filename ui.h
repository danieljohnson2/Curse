#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <ncurses.h>

#define ESCAPE_KEY 27

WINDOW *display_multiline_window (char *title, char **lines, char *prompt,
                                  int top_margin, int left_margin,
                                  int bottom_margin, int right_margin,
                                  bool centered);

#endif

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "map.h"

static int
get_terrain_char (Terrain terrain)
{
    switch (terrain)
    {
    case deep_sea:
        return '/' | COLOR_PAIR (1) | A_BOLD;
    case water:
        return '~' | COLOR_PAIR (1) | A_BOLD;
    case beach:
        return '.' | COLOR_PAIR (2);
    case grass:
        return ':' | COLOR_PAIR (3) | A_BOLD;
    case woods:
        return '%' | COLOR_PAIR (3);
    case hills:
        return '^' | COLOR_PAIR (4);
    case mountains:
        return '^' | COLOR_PAIR (4) | A_BOLD;
    }
}

static void
paint (WINDOW * w, Map * map, int y, int x, double scale)
{
    int maxrow, maxcol;
    getmaxyx (w, maxrow, maxcol);

    for (int row = 0; row < maxrow; ++row)
    {
        for (int col = 0; col < maxcol; ++col)
        {
            int px = (col + x) * scale;
            int py = (row + y) * scale;

            Terrain t = read_map (map, px, py);
            int ch = get_terrain_char (t);
            mvwaddch (w, row, col, ch);
        }
    }
}

int
main (int argc, char **argv)
{
    initscr ();
    cbreak ();
    noecho ();

    start_color ();
    init_pair (1, COLOR_CYAN, COLOR_BLUE);
    init_pair (2, COLOR_YELLOW, COLOR_BLACK);
    init_pair (3, COLOR_GREEN, COLOR_BLACK);
    init_pair (4, COLOR_WHITE, COLOR_BLACK);

    int rows, columns;
    getmaxyx (stdscr, rows, columns);

    box (stdscr, 0, 0);
    refresh ();

    WINDOW *w = newwin (rows - 2, columns - 2, 1, 1);
    keypad (w, TRUE);

    int seed = 1;
    int x = -40, y = -20;
    double scale = 1.0;
    int speed = 8;

    for (;;)
    {
        Map map = make_map (16, round_shape,
                            make_perlin (1.0 / 8.0, 2, seed));

        paint (w, &map, y, x, scale);
        
        switch (wgetch (w))
        {
        case 'q':
            endwin ();
            return 0;
        case 's':
            seed++;
            break;
        case KEY_HOME:
            scale = 1.0;
            break;
        case KEY_UP:
            y -= speed;
            break;
        case KEY_DOWN:
            y += speed;
            break;
        case KEY_LEFT:
            x -= speed;
            break;
        case KEY_RIGHT:
            x += speed;
            break;
        case KEY_PPAGE:
            scale += 0.125;
            break;
        case KEY_NPAGE:
            scale -= 0.125;
            break;
        }
    }
}

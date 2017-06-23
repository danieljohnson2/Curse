#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#include "perlin.h"

static double
pin (double value, double min, double max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

#define TERRAIN_COUNT 7
#define SOFT_MAP_SIZE 128.0

static int
pickchar (double depth, double dist)
{
    static int a[TERRAIN_COUNT] = {
        '/' | COLOR_PAIR (1) | A_BOLD,
        '~' | COLOR_PAIR (1) | A_BOLD,
        '.' | COLOR_PAIR (2),
        ':' | COLOR_PAIR (3) | A_BOLD,
        '%' | COLOR_PAIR (3),
        '^' | COLOR_PAIR (4),
        '^' | COLOR_PAIR (4) | A_BOLD
    };

    double adjusted = depth;

    if (dist > SOFT_MAP_SIZE)
        dist -= SOFT_MAP_SIZE;
    else if (dist < -SOFT_MAP_SIZE)
        dist += SOFT_MAP_SIZE;
    else
        dist = 0.0;

    adjusted += pin (dist / SOFT_MAP_SIZE, -.65, .65);
    double unpinned = (adjusted * TERRAIN_COUNT - 1) + 0.5;
    int index = pin (unpinned, 0, TERRAIN_COUNT - 1);

    if (index < 0 || index >= TERRAIN_COUNT)
    {
        printf ("Bad depth: %f", depth);
        abort ();
    }

    return a[index];
}

static void
paint (WINDOW * w, PERLIN * perlin, int y, int x, double scale)
{
    int maxrow, maxcol;
    getmaxyx (w, maxrow, maxcol);

    for (int row = 0; row < maxrow; ++row)
    {
        for (int col = 0; col < maxcol; ++col)
        {
            int px = (col + x) * scale;
            int py = (row + y) * scale;

            double p = perlin2d (perlin, px, py);
            mvwaddch (w, row, col, pickchar (p, px + py));
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
	getmaxyx(stdscr, rows, columns);

    box (stdscr, 0, 0);
    refresh ();

    WINDOW *w = newwin (rows-2, columns-2, 1, 1);
    keypad (w, TRUE);
	
    int seed = 1;
    int x = -40, y = -20;
    double scale = 1.0;
    int speed = 8;

    for (;;)
    {
        PERLIN perlin;
        init_perlin (&perlin, 1.0 / 8.0, 2, seed);

        paint (w, &perlin, y, x, scale);
        wrefresh (w);

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

#include "map.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
        return '.' | COLOR_PAIR (2) | A_BOLD;
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

typedef struct _Game
{
    Map map;
    int viewx, viewy;
    int playerx, playery;
} Game;

static void
update_view (Game * game, int margin, int width, int height)
{
    int left = game->viewx + margin;
    int right = game->viewx + width - margin;
    int top = game->viewy + margin;
    int bottom = game->viewy + height - margin;

    if (game->playerx < left)
        game->viewx = game->playerx - margin;
    else if (game->playerx > right)
        game->viewx = game->playerx - width + margin;

    if (game->playery < top)
        game->viewy = game->playery - margin;
    else if (game->playery > bottom)
        game->viewy = game->playery - height + margin;
}

static void
paint (WINDOW * w, Game * game)
{
    int maxrow, maxcol;
    getmaxyx (w, maxrow, maxcol);

    update_view (game, 8, maxcol, maxrow);

    int finalcursorrow = 0, finalcursorcolumn = 0;

    for (int row = 0; row < maxrow; ++row)
    {
        for (int col = 0; col < maxcol; ++col)
        {
            int px = col + game->viewx;
            int py = row + game->viewy;

            if (px == game->playerx && py == game->playery)
            {
                mvwaddch (w, row, col, '@' | A_BOLD);
                finalcursorrow = row;
                finalcursorcolumn = col;
            }
            else
            {
                Terrain t = read_map (&game->map, px, py);
                int ch = get_terrain_char (t);
                mvwaddch (w, row, col, ch);
            }
        }
    }

    if (finalcursorrow >= 0)
        wmove (w, finalcursorrow, finalcursorcolumn);
}

static void
move_player (Game * game, int dx, int dy)
{
    int nx = game->playerx + dx;
    int ny = game->playery + dy;

    Terrain t = read_map (&game->map, nx, ny);

    if (is_passable (t))
    {
        game->playerx = nx;
        game->playery = ny;
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

    Game game = { 0 };
    game.map = make_map (16, round_shape, make_perlin (1.0 / 8.0, 2, 4));
    game.playery = -8;
    for (;;)
    {
        paint (w, &game);
        switch (wgetch (w))
        {
        case 'q':
            endwin ();
            return 0;
        case KEY_UP:
            move_player (&game, 0, -1);
            break;
        case KEY_DOWN:
            move_player (&game, 0, +1);
            break;
        case KEY_LEFT:
            move_player (&game, -1, 0);
            break;
        case KEY_RIGHT:
            move_player (&game, +1, 0);
            break;
        }
    }
}

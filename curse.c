#include "map.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

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

#define THING_COUNT 16
#define MESSAGE_MAX 512
#define PLAYER_INDEX 0

typedef struct _Thing
{
    char appearance;
    int x, y;
} Thing;

static Thing
make_thing (char appearance, int x, int y)
{
    Thing th = { 0 };
    th.appearance = appearance;
    th.x = x;
    th.y = y;
    return th;
}

typedef struct _Game
{
    Map map;
    int viewx, viewy;

    Thing things[THING_COUNT];
    char message[MESSAGE_MAX + 1];
} Game;

static void
update_view (Game * game, Thing * thing, int margin, int width, int height)
{
    int left = game->viewx + margin;
    int right = game->viewx + width - margin;
    int top = game->viewy + margin;
    int bottom = game->viewy + height - margin;

    if (thing->x < left)
        game->viewx = thing->x - margin;
    else if (thing->x > right)
        game->viewx = thing->x - width + margin;

    if (thing->y < top)
        game->viewy = thing->y - margin;
    else if (thing->y > bottom)
        game->viewy = thing->y - height + margin;
}

static void
paint (WINDOW * map_w, WINDOW * message_w, Game * game)
{
    int maxrow, maxcol;
    getmaxyx (map_w, maxrow, maxcol);

    update_view (game, &game->things[PLAYER_INDEX], 8, maxcol, maxrow);

    int finalcursorrow = 0, finalcursorcolumn = 0;

    for (int row = 0; row < maxrow; ++row)
    {
        for (int col = 0; col < maxcol; ++col)
        {
            int px = col + game->viewx;
            int py = row + game->viewy;

            Terrain t = read_map (&game->map, px, py);
            int ch = get_terrain_char (t);
            mvwaddch (map_w, row, col, ch);
        }
    }

    for (int i = 0; i < THING_COUNT; ++i)
    {
        Thing *thing = &game->things[i];

        if (thing->appearance != '\0')
        {
            int col = thing->x - game->viewx;
            int row = thing->y - game->viewy;

            if (col >= 0 && row >= 0 && col < maxcol && row < maxrow)
            {
                mvwaddch (map_w, row, col, thing->appearance | A_BOLD);

                if (i == PLAYER_INDEX)
                {
                    finalcursorrow = row;
                    finalcursorcolumn = col;
                }
            }
        }
    }

    if (game->message[0] != '\0')
        mvwaddstr (message_w, 0, 0, game->message);
    else
        werase (message_w);

    if (finalcursorrow >= 0)
        wmove (map_w, finalcursorrow, finalcursorcolumn);
}

bool
is_game_passable (Game * game, int x, int y)
{
    Terrain t = read_map (&game->map, x, y);

    if (!is_terrain_passable (t))
        return false;

    for (int i = 0; i < THING_COUNT; ++i)
    {
        Thing *thing = &game->things[i];

        if (thing->appearance != '\0' && thing->x == x && thing->y == y)
        {
            return false;
        }
    }

    return true;
}

static void
move_player (Game * game, int dx, int dy)
{
    Thing *player = &game->things[PLAYER_INDEX];

    int nx = player->x + dx;
    int ny = player->y + dy;

    if (is_game_passable (game, nx, ny))
    {
        player->x = nx;
        player->y = ny;
    }
    else
        strcpy (game->message, "Blocked!");
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

    refresh ();

    WINDOW *border_w = newwin (rows - 1, columns, 1, 0);
    box (border_w, 0, 0);
    wrefresh (border_w);

    WINDOW *map_w = newwin (rows - 3, columns - 2, 2, 1);
    keypad (map_w, TRUE);

    WINDOW *message_w = newwin (1, columns, 0, 0);

    Game game = { 0 };
    game.map = make_map (16, round_shape, make_perlin (1.0 / 8.0, 2, 4));
    game.things[PLAYER_INDEX] = make_thing ('@', 0, -8);

    // fake monsters!
    game.things[1] = make_thing ('g', -10, -7);
    game.things[2] = make_thing ('h', -20, 8);

    for (;;)
    {
        paint (map_w, message_w, &game);
        wrefresh (message_w);

        memset (game.message, 0, MESSAGE_MAX);

        switch (wgetch (map_w))
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

#include "display.h"
#include "loc.h"
#include "thing.h"
#include "player.h"
#include "map.h"
#include "game.h"
#include "util.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

static WINDOW *map_w;
static WINDOW *message_w;
static WINDOW *status_w;

/* Initialize the display and creates the windows. */
void
init_windows (void)
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

    // first, draw the decorative border.
    refresh ();
    WINDOW *border_w = newwin (rows - 2, columns, 1, 0);
    box (border_w, 0, 0);
    wrefresh (border_w);
    delwin (border_w);

    map_w = newwin (rows - 4, columns - 2, 2, 1);
    message_w = newwin (1, columns, 0, 0);
    status_w = newwin (1, columns, rows - 1, 0);

    keypad (map_w, TRUE);
}

/* Shuts down ncurses, and may restore the terminal to its initial state */
void
end_windows (void)
{
    endwin ();
}

/* Reads a keystroke, as long as it is 'q' */
void
wait_keystroke (void)
{
    while (wgetch (map_w) != 'q')
        continue;
}

static int
get_terrain_char (Terrain terrain)
{
    switch (terrain)
    {
    case DEEP_SEA:
        return '/' | COLOR_PAIR (1) | A_BOLD;
    case WATER:
        return '~' | COLOR_PAIR (1) | A_BOLD;
    case BEACH:
        return '.' | COLOR_PAIR (2) | A_BOLD;
    case GRASS:
        return ':' | COLOR_PAIR (3) | A_BOLD;
    case WOODS:
        return '%' | COLOR_PAIR (3);
    case HILLS:
        return '^' | COLOR_PAIR (4);
    case MOUNTAINS:
        return '^' | COLOR_PAIR (4) | A_BOLD;
    default:
        return ' ';
    }
}

static int
get_appearance_char (Appearance appearance)
{
    switch (appearance)
    {
    case PLAYER:
        return '@' | A_BOLD;
    case GOBLIN:
        return 'g' | A_BOLD;
    case HALFLING:
        return 'h' | A_BOLD;
    case SMALL_TREASURE:
        return '*' | COLOR_PAIR (2);
    case MEDIUM_TREASURE:
        return '*' | A_BOLD | COLOR_PAIR (2);
    case LARGE_TREASURE:
        return '*' | A_BOLD;
    default:
        return '?';
    }
}

static void
show_message (Game * game)
{
    consolidate_game_messages (game);

    int rows, columns;
    getmaxyx (message_w, rows, columns);
    (void) rows;                // suppress warning

    char buffer[MESSAGE_SIZE];
    strtcpy (buffer, game->message, MESSAGE_SIZE);

    char *start = buffer;

    werase (message_w);

    char more_text[] = " <more>";
    int more_length = 7;
    int line_limit = columns - more_length;

    while (start[0] != '\0')
    {
        if (start > buffer)
        {
            mvwaddstr (message_w, 0, line_limit, more_text);
            wgetch (message_w);
        }

        int len = strlen (start);
        char *next = start + len;

        if (len > line_limit)
        {
            char *end = start + line_limit;
            while (end > start && *end != ' ')
                --end;

            if (end > start)
            {
                next = end + 1;
                *end = '\0';
            }
        }

        werase (message_w);
        mvwaddstr (message_w, 0, 0, start);
        start = next;
    }

    wrefresh (message_w);
}

static Loc
update_view (Game * game, Thing * thing, int marginx, int marginy, int width,
             int height)
{
    Loc offset = make_loc (width / 2, height / 2);
    Loc origin = subtract_locs (game->view_center, offset);

    int left = origin.x + marginx;
    int right = origin.x + width - marginx;
    int top = origin.y + marginy;
    int bottom = origin.y + height - marginy;

    Loc l = thing->loc;

    if (l.x < left)
        origin.x = l.x - marginx;
    else if (l.x > right)
        origin.x = l.x - width + marginx;

    if (l.y < top)
        origin.y = l.y - marginy;
    else if (l.y > bottom)
        origin.y = l.y - height + marginy;

    game->view_center = add_locs (origin, offset);
    return origin;
}

/* Updates the screen with the state of the game. */
void
paint (Game * game, bool messages)
{
    Thing *player = &game->things[PLAYER_INDEX];

    int maxrow, maxcol;
    getmaxyx (map_w, maxrow, maxcol);

    Loc origin =
        update_view (game, &game->things[PLAYER_INDEX], 16, 4, maxcol,
                     maxrow);

    int finalcursorrow = 0, finalcursorcolumn = 0;

    for (int row = 0; row < maxrow; ++row)
    {
        for (int col = 0; col < maxcol; ++col)
        {
            Loc map_loc = offset_loc (origin, col, row);
            Terrain t = read_map (&game->map, map_loc);
            int ch = get_terrain_char (t);
            mvwaddch (map_w, row, col, ch);
        }
    }

    for (int i = 0; i < THING_COUNT; ++i)
    {
        Thing *thing = &game->things[i];

        if (is_thing_alive (thing))
        {
            int col = thing->loc.x - origin.x;
            int row = thing->loc.y - origin.y;

            if (col >= 0 && row >= 0 && col < maxcol && row < maxrow)
            {
                int ch = get_appearance_char (thing->appearance);
                mvwaddch (map_w, row, col, ch);

                if (i == PLAYER_INDEX)
                {
                    finalcursorrow = row;
                    finalcursorcolumn = col;
                }
            }
        }
    }

    werase (status_w);
    mvwprintw (status_w, 0, 0, "HP: %d Gold: %d", player->hp, player->gold);

    wrefresh (map_w);
    wrefresh (status_w);

    show_message (game);

    if (finalcursorrow >= 0)
    {
        wmove (map_w, finalcursorrow, finalcursorcolumn);
        wrefresh (map_w);
    }
}

/*
Reads a keystroke and maps it to a PlayerAction. Unknown keys
are mapped to None.
*/
PlayerAction
read_player_action (void)
{
    int ch = wgetch (map_w);

    switch (ch)
    {
    case 'q':
        return Quit;
    case KEY_UP:
        return Up;
    case KEY_DOWN:
        return Down;
    case KEY_LEFT:
        return Left;
    case KEY_RIGHT:
        return Right;
    case ' ':
        return Pass;
    default:
        return None;
    }
}
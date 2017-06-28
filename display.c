#include "display.h"
#include "loc.h"
#include "thing.h"
#include "player.h"
#include "map.h"
#include "message.h"
#include "game.h"
#include "util.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

static WINDOW *map_w;
static WINDOW *message_w;
static WINDOW *status_w;

static Loc view_center;

static void
show_error (char *error)
{
    werase (message_w);
    mvwaddstr (message_w, 0, 0, error);
    wrefresh (message_w);
}

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

/* Displays the game over message and waits for a keystroke */
void
game_over (void)
{
    int rows, columns;
    getmaxyx (stdscr, rows, columns);

    WINDOW *w = newwin (6, 40, rows - 12, (columns - 40) / 2);
    box (w, 0, 0);
    mvwaddstr (w, 2, 20 - 4, "Game Over");
    mvwaddstr (w, 3, 20 - 8, "(press q to exit)");

    while (wgetch (w) != 'q')
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
show_message ()
{
    int rows, columns;
    getmaxyx (message_w, rows, columns);
    (void) rows;                // suppress warning

    char message[MESSAGE_SIZE];
    read_message (message);
    char *start = message;

    werase (message_w);

    char more_text[] = " <more>";
    int more_length = 7;
    int line_limit = columns - more_length;

    while (start[0] != '\0')
    {
        if (start > message)
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
    Loc origin = subtract_locs (view_center, offset);

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

    view_center = add_locs (origin, offset);
    return origin;
}

/*
Updates the screen with the state of the game.
This can also display the game's pending messages,
if you pass true in 'messages'.
*/
void
paint (Game * game, bool messages)
{
    Thing *player = get_player (game);

    int maxrow, maxcol;
    getmaxyx (map_w, maxrow, maxcol);

    Loc origin = update_view (game, player, 16, 4, maxcol, maxrow);

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

    for (Thing * thing = NULL; next_thing (game, &thing);)
    {
        int col = thing->loc.x - origin.x;
        int row = thing->loc.y - origin.y;

        if (col >= 0 && row >= 0 && col < maxcol && row < maxrow)
        {
            int ch = get_appearance_char (thing->appearance);
            mvwaddch (map_w, row, col, ch);

            if (thing == player)
            {
                finalcursorrow = row;
                finalcursorcolumn = col;
            }
        }
    }

    werase (status_w);
    mvwprintw (status_w, 0, 0, "HP: %d Gold: %d/%d", player->hp, player->gold,
               get_total_gold (game));

    wrefresh (map_w);
    wrefresh (status_w);

    show_message ();

    if (finalcursorrow >= 0)
    {
        wmove (map_w, finalcursorrow, finalcursorcolumn);
        wrefresh (map_w);
    }
}

void
center_view (Loc target)
{
    view_center = target;
}

/*
Reads a keystroke and maps it to a PlayerAction. Unknown keys
are mapped to None.
*/
PlayerAction
read_player_action (void)
{
    for (;;)
    {
        int ch = wgetch (map_w);

        PlayerAction action = { 0 };

        switch (ch)
        {
        case 'q':
            endwin ();
            exit (0);
            break;

        case KEY_UP:
            action.dy = -1;
            return action;
        case KEY_DOWN:
            action.dy = 1;
            return action;
        case KEY_LEFT:
            action.dx = -1;
            return action;
        case KEY_RIGHT:
            action.dx = 1;
            return action;
        case ' ':
            return action;

        default:
            {
                char msg[MESSAGE_SIZE];
                sprintf (msg, "Invalid key! (%d)", ch);
                show_error (msg);
            }
            break;
        }
    }
}

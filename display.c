#include "display.h"
#include "map.h"
#include "message.h"
#include "game.h"
#include "thing.h"
#include "treasure.h"
#include "item.h"
#include "util.h"
#include "ui.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
    keypad (stdscr, false);

    start_color ();
    init_pair (1, COLOR_CYAN, COLOR_BLUE);
    init_pair (2, COLOR_YELLOW, COLOR_BLACK);
    init_pair (3, COLOR_GREEN, COLOR_BLACK);
    init_pair (4, COLOR_WHITE, COLOR_BLACK);
}

/* Create and place windows according to the screen's size */
void
arrange_windows (void)
{
    int rows, columns;
    getmaxyx (stdscr, rows, columns);

    if (message_w != NULL)
        delwin (message_w);
    if (map_w != NULL)
        delwin (map_w);
    if (status_w != NULL)
        delwin (status_w);

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
    case ORC:
        return 'o' | A_BOLD;
    case WOLF:
        return 'w' | A_BOLD;
    case HALFLING:
        return 'h' | A_BOLD;
    case ELF:
        return 'e' | A_BOLD;
    case WEAPON:
        return '/' | A_BOLD;
    case ARMOR:
        return '[' | A_BOLD;
    case POTION:
        return '?' | A_BOLD;
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

static void
show_inventory ()
{
    char *lines[26] = { "(nothing)" };
    Thing *items[26] = { 0 };
    int item_count = 0;

    Thing *player = get_player ();

    for (Thing * item = NULL; item_count < 26 && next_thing (player, &item);)
    {
        items[item_count] = item;
        lines[item_count++] = item->name;
    }

    WINDOW *w =
        display_multiline_window ("Inventory", lines, "(esc to close)", 0, 3,
                                  0, 1, false);

    for (;;)
    {
        char prefix[] = "?) ";
        for (int i = 0; i < item_count; ++i)
        {
            prefix[0] = 'a' + i;
            prefix[2] = items[i]->equipped ? '*' : ' ';
            mvwaddstr (w, i + 1, 1, prefix);
        }

        int ch = wgetch (w);

        if (ch == ESCAPE_KEY)
            break;

        if (ch >= 'a' && ch <= 'z')
        {
            int index = ch - 'a';

            if (index < item_count)
            {
                if (items[index]->equipped)
                    items[index]->equipped = false;
                else
                {
                    UseAction use_action = get_use_action (items[index]);

                    if (use_action != NULL)
                    {
                        use_action (items[index], player);
                        break;
                    }
                    else
                        equip_item (player, items[index]);
                }
            }
        }
    }

    delwin (w);
}

static Loc
update_view (Thing * thing, int marginx, int marginy, int width, int height)
{
    Loc offset = make_loc (width / 2, height / 2);
    Loc origin = subtract_locs (view_center, offset);

    int left = origin.x + marginx;
    int right = origin.x + width - marginx;
    int top = origin.y + marginy;
    int bottom = origin.y + height - marginy;

    Loc l = thing->loc;

    if (l.x < left || l.x > right || l.y < top || l.y > bottom)
    {
        center_view (l);
        origin = subtract_locs (view_center, offset);
    }

    return origin;
}

/*
Updates the screen with the state of the game.
This can also display the game's pending messages,
if you pass true in 'messages'.
*/
void
paint (bool messages)
{
    Thing *player = get_player ();

    int maxrow, maxcol;
    getmaxyx (map_w, maxrow, maxcol);

    Loc origin = update_view (player, 16, 4, maxcol, maxrow);

    int finalcursorrow = 0, finalcursorcolumn = 0;

    for (int row = 0; row < maxrow; ++row)
    {
        for (int col = 0; col < maxcol; ++col)
        {
            Loc map_loc = offset_loc (origin, col, row);
            Terrain t = get_map_terrain (get_map (), map_loc);
            int ch = get_terrain_char (t);
            mvwaddch (map_w, row, col, ch);
        }
    }

    /* Use appareance ordering so monsters appear on top of items */
    for (Appearance a = LARGE_TREASURE; a > DEAD; --a)
    {
        for (Thing * thing = NULL; next_thing (NULL, &thing);)
        {
            int col = thing->loc.x - origin.x;
            int row = thing->loc.y - origin.y;

            if (col >= 0 && row >= 0 && col < maxcol && row < maxrow &&
                thing->appearance == a)
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
    }

    werase (status_w);
    mvwprintw (status_w, 0, 0, "HP: %d XP: %d Gold: %d/%d", player->hp,
               player->xp, player->gold, get_total_gold ());

    wrefresh (map_w);
    wrefresh (status_w);

    if (messages)
    {
        show_message ();
        clear_message ();
    }

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

static char *
get_save_file_name (void)
{
    static char file_name[PATH_MAX];

    if (file_name[0] == '\0')
    {
        strtcpy (file_name, getenv ("HOME"), PATH_MAX);
        strtcat (file_name, "/.curse_save_game", PATH_MAX);
    }

    return file_name;
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

        case 'i':
            show_inventory ();
            paint (false);
            break;

        case 's':
            save_game (get_save_file_name ());
            write_message ("Game saved.");
            paint (true);
            break;

        case 'l':
            restore_game (get_save_file_name ());
            write_message ("Game restored.");
            paint (true);
            break;

        case KEY_RESIZE:
            arrange_windows ();
            paint (false);
            break;

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

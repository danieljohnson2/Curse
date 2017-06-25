#include "game.h"
#include "monster.h"
#include "treasure.h"
#include "player.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

/*
Builds the entire game structure.
This function copies a lot of bytes, but it only happens once.
*/
Game
make_game (Map map, Thing player)
{
    Game game = { 0 };
    game.map = map;
    game.view_center = player.loc;

    game.things[PLAYER_INDEX] = player;

    for (int i = 0; i < TREASURE_COUNT; ++i)
        place_thing (&game, make_loc (0, 0), make_random_treasure ());

    return game;
}

/* Clears the current message */
void
clear_game_message (Game * game)
{
    memset (game->message, '\0', MESSAGE_SIZE);
}

/* Appends a message to the current message. */
void
write_game_message (Game * game, char *message)
{
    if (game->message[0] == '\0')
        strtcpy (game->message, message, MESSAGE_SIZE);
    else
    {
        strtcat (game->message, " ", MESSAGE_SIZE);
        strtcat (game->message, message, MESSAGE_SIZE);
    }
}

/*
Utiltity for consolidate_game_messages().
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
This updates the game's 'message' buffer to remove duplicate message
and insert repetition indicators.
*/
void
consolidate_game_messages (Game * game)
{
    // An extra initial space makes matching more
    // reliable. This way every message is preceeded by
    // a space.

    char input[MESSAGE_SIZE + 1];
    input[0] = ' ';
    strtcpy (input + 1, game->message, MESSAGE_SIZE);

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
    strtcpy (game->message, buffer + 1, MESSAGE_SIZE);
}

/*
Finds the thing (or things) at the location specified. You must
pass a buffer to 'found' to receive the found thing. It should
initially contain NULL. If we find a thing, this method places
it there and returns true.

If you call it again, we use the thing still in the buffer to
start the search, and find the next thing after it.

when we can find nothing more, we we 'found' to NULL again
and return false.

This function can only find things that are alive.
*/
bool
find_thing_at (Game * game, Loc where, Thing ** found)
{
    Thing *end = &game->things[THING_COUNT];
    Thing *cur = *found == NULL ? game->things : (*found) + 1;

    for (; cur != end; ++cur)
    {
        if (is_thing_alive (cur) && equal_locs (cur->loc, where))
        {
            *found = cur;
            return true;
        }
    }

    *found = NULL;
    return false;
}

int
new_thing (Game * game, Thing thing)
{
    for (int i = PLAYER_INDEX + 1; i < THING_COUNT; ++i)
    {
        Thing *candidate = &game->things[i];
        if (!is_thing_alive (candidate))
        {
            *candidate = thing;
            return i;
        }
    }

    return -1;
}

int
place_thing (Game * game, Loc origin, Thing thing)
{
    thing.loc = find_empty_place (game, origin);
    return new_thing (game, thing);
}

Loc
find_empty_place (Game * game, Loc origin)
{
    for (;;)
    {
        Loc c = find_passable_place (&game->map, origin);

        Thing *start = NULL;
        if (!find_thing_at (game, c, &start))
            return c;
    }
}

/* Decides if the thing given is the player. */
bool
is_thing_player (Game * game, Thing * thing)
{
    return thing == &game->things[PLAYER_INDEX];
}

/*
This is the ain loop; this iterates over the things and
invokes their turn action. User input is handled in the
player's turn action; if the player is dead then this
function immediately exits.
*/
void
perform_turns (Game * game)
{
    Thing *player = &game->things[PLAYER_INDEX];

    int thing_index = 0;
    while (is_thing_alive (player))
    {
        Thing *actor = &game->things[thing_index];

        if (is_thing_alive (actor))
        {
            if (actor->remaining_wait <= 0)
            {
                actor->turn_action (game, actor);
                actor->remaining_wait += SPEED_MAX;
            }

            actor->remaining_wait -= actor->speed;
        }

        ++thing_index;
        if (thing_index >= THING_COUNT)
            thing_index = 0;
    }
}

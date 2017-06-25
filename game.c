#include "game.h"
#include "monster.h"
#include "treasure.h"
#include "player.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
    memset (game->message, '\0', MESSAGE_MAX + 1);
}

/* Appends a message to the current message. */
void
write_game_message (Game * game, char *message)
{
    if (game->message[0] == '\0')
        strncpy (game->message, message, MESSAGE_MAX);
    else
    {
        strncat (game->message, " ", MESSAGE_MAX);
        strncat (game->message, message, MESSAGE_MAX);
        game->message[MESSAGE_MAX] = '\0';      // ensure null termination
    }
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
This function checks to see if any actor is ready
for its turn. If so, it returns and some actor
that is alive has a remaining_wait of or less.

If not, this function deducts the speed for every live
actor from its wait and tries again. This will eventually
produce a suitable actor, unless nothing is alive.

If nothing is alive, this function exits and does nothing
else.

When an actor takes its turn, we add WAIT_MAX to the wait
of that actor, so this function may be called again.
*/
static void
adjust_remaining_wait (Game * game)
{
    for (;;)
    {
        bool any_active = false;

        for (int i = 0; i < THING_COUNT; ++i)
        {
            Thing *actor = &game->things[i];
            if (is_thing_alive (actor))
            {
                if (actor->speed > 0)
                    any_active = true;

                if (actor->remaining_wait <= 0)
                    return;
            }
        }

        // If nothing lives or has a positive speed,
        // there is nthing we can do that will help.
        if (!any_active)
            break;

        for (int i = 0; i < THING_COUNT; ++i)
        {
            Thing *actor = &game->things[i];

            if (is_thing_alive (actor))
                actor->remaining_wait -= actor->speed;
        }
    }
}

/*
This is the ain loop; this iterates over the things and
invokes their turn action. User input is handled in the
player's turn action; if the playe ris dead then this
function immediately exits.
*/
void
perform_turns (Game * game)
{
    Thing *player = &game->things[PLAYER_INDEX];

    while (is_thing_alive (player))
    {
        adjust_remaining_wait (game);

        for (int thing_index = 0; thing_index < THING_COUNT; ++thing_index)
        {
            if (!is_thing_alive (player))
                break;

            Thing *actor = &game->things[thing_index];

            if (is_thing_alive (actor))
            {
                if (actor->remaining_wait <= 0)
                {
                    actor->turn_action (game, actor);
                    actor->remaining_wait += SPEED_MAX;
                }
                else
                    actor->skipped_turn_action (game, actor);
            }
        }
    }
}

#include "game.h"
#include "map.h"
#include "monster.h"
#include "thing.h"
#include "player.h"
#include "treasure.h"
#include "util.h"
#include "loc.h"

#define PLAYER_INDEX 0

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

bool
next_thing (Game * game, Thing ** thing)
{
    if (*thing == NULL)
    {
        *thing = game->things;
        return true;
    }

    (*thing)++;

    if (*thing == &game->things[THING_COUNT])
    {
        *thing = NULL;
        return false;
    }
    else
        return true;
}

bool
next_live_thing (Game * game, Thing ** thing)
{
    while (next_thing (game, thing))
    {
        if (is_thing_alive (*thing))
            return true;
    }

    return false;
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
next_thing_at (Game * game, Loc where, Thing ** found)
{
    while (next_live_thing (game, found))
    {
        if (equal_locs ((*found)->loc, where))
            return true;
    }

    return false;
}

/* Returns the player (who might be dead!) */
Thing *
get_player (Game * game)
{
    return &game->things[PLAYER_INDEX];
}

/* Adds a new thing to the game, and returns a pointer to the thing in
the game itself, or NULL if there is no room for it. */
Thing *
new_thing (Game * game, Thing thing)
{
    Thing *player = get_player (game);
    for (Thing * candidate = NULL; next_thing (game, &candidate);)
    {
        if (!is_thing_alive (candidate) && candidate != player)
        {
            *candidate = thing;
            return candidate;
        }
    }

    return NULL;
}

/* Adds a new thing to the game, and places it randomly
using find_empty_place(). */
Thing *
place_thing (Game * game, Loc origin, Thing thing)
{
    thing.loc = find_empty_place (game, origin);
    return new_thing (game, thing);
}

/* Returns a random location that is passable and contains no things. */
Loc
find_empty_place (Game * game, Loc origin)
{
    for (;;)
    {
        Loc c = find_passable_place (&game->map, origin);

        Thing *start = NULL;
        if (!next_thing_at (game, c, &start))
            return c;
    }
}

/* Returns the total gold in all live things */
int
get_total_gold (Game * game)
{
    int total = 0;
    for (Thing * th = NULL; next_live_thing (game, &th);)
        total += th->gold;
    return total;
}

/*
This is the main loop; this iterates over the things and
invokes their turn action. User input is handled in the
player's turn action; if the player is dead then this
function immediately exits.
*/
void
perform_turns (Game * game)
{
    Thing *player = get_player (game);

    Thing *actor = NULL;
    while (is_thing_alive (player))
    {
        if (next_live_thing (game, &actor))
        {
            if (actor->remaining_wait <= 0)
            {
                actor->turn_action (game, actor);
                actor->remaining_wait += SPEED_MAX;
            }

            actor->remaining_wait -= actor->speed;
        }

        // if next_life_thing fails, we'll just start again
        // with NULL.
    }
}

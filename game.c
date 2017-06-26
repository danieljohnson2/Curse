#include "game.h"
#include "map.h"
#include "monster.h"
#include "thing.h"
#include "player.h"
#include "treasure.h"
#include "util.h"
#include "loc.h"

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
    game.player_index = 0;
    game.things[game.player_index] = player;

    for (int i = 0; i < TREASURE_COUNT; ++i)
        place_thing (&game, make_loc (0, 0), make_random_treasure ());

    return game;
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
    for (int i = 0; i < THING_COUNT; ++i)
    {
        Thing *candidate = &game->things[i];
        if (!is_thing_alive (candidate) && i != game->player_index)
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

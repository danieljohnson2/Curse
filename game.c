#include "game.h"

#include <stdlib.h>
#include <string.h>

/*
Builds the entire game structure.
This function copies a lot of bytes, but it only happens once.
*/
Game
make_game (Map map, TurnAction player_turn_action)
{
    Game game = { 0 };
    game.map = map;
    game.things[PLAYER_INDEX] =
        make_thing ('@', "Player", 0, 0, attack_bump_action,
                    player_turn_action);

    // fake monsters! Not as fake as they used to be!
    game.things[1] =
        make_thing ('g', "Goblin", 0, 0, attack_bump_action,
                    chase_player_turn_action);
    game.things[2] =
        make_thing ('h', "Halfling", 0, 8, attack_bump_action,
                    chase_player_turn_action);

    for (int i = 0; i < THING_COUNT; ++i)
    {
        Thing *thing = &game.things[i];
        if (is_thing_alive (thing))
            place_thing (&game, thing);
    }

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
find_thing_at (Game * game, int x, int y, Thing ** found)
{
    Thing *end = &game->things[THING_COUNT];
    Thing *cur = *found == NULL ? game->things : (*found) + 1;

    for (; cur != end; ++cur)
    {
        if (is_thing_alive (cur) && cur->x == x && cur->y == y)
        {
            *found = cur;
            return true;
        }
    }

    *found = NULL;
    return false;
}

/*
Sets the position of the thing to an unused, passable space
inside the game world. This does not count as movement; bump
actions will not be triggered.
*/
void
place_thing (Game * game, Thing * thing)
{
    Map *map = &game->map;
    int size = map->soft_size;

    for (;;)
    {
        int x = (rand () % (size * 2)) - size;
        int y = (rand () % (size * 2)) - size;
        Terrain t = read_map (map, x, y);

        if (is_terrain_passable (t))
        {
            Thing *start = NULL;
            if (!find_thing_at (game, x, y, &start))
            {
                thing->x = x;
                thing->y = y;
                return;
            }
        }
    }
}

bool
is_thing_player (Game * game, Thing * thing)
{
    return thing == &game->things[PLAYER_INDEX];
}

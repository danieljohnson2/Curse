#include "game.h"
#include "player.h"
#include "treasure.h"
#include "monster.h"
#include "action.h"
#include "weapon.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define THING_COUNT 32
#define INVENTORY_COUNT 8
#define PLAYER_INDEX 0

static Map game_map;
static Thing game_things[THING_COUNT];
static Thing inventory_things[THING_COUNT * INVENTORY_COUNT];
static SpawnSettings game_spawn;

/*
Initializes the game. This clears all the thing data,
copies in the map data and the player, then adds some
treasures to it.
*/
static void
init_game (Map map, Thing player, SpawnSettings spawn)
{
    game_map = map;
    game_spawn = spawn;

    memset (&game_things, 0, sizeof (game_things));
    memset (&inventory_things, 0, sizeof (inventory_things));
    game_things[PLAYER_INDEX] = player;

    for (int i = 0; i < TREASURE_COUNT; ++i)
        place_thing (make_loc (0, 0), make_random_treasure ());

    for (int i = 0; i < WEAPON_COUNT; ++i)
        place_thing (make_loc (0, 0), make_random_weapon ());
}

/* Starts a new game with the map given. */
void
start_game (Map map)
{
    SpawnSettings spawn;
    spawn.max_monsters = 2;
    spawn.turns_per_spawn = 64;
    spawn.max_monster_level = 2;

    Thing player = make_player (&map);
    init_game (map, player, spawn);
}

/*
Re-initalizes the game for the next level.
*/
void
next_level (void)
{
    SpawnSettings spawn = game_spawn;
    MapShape shape = game_map.shape;
    int seed = rand ();
    Thing player = *get_player ();

    if (spawn.turns_per_spawn > 1)
        --spawn.turns_per_spawn;

    spawn.max_monsters *= 2;
    spawn.max_monster_level += 1;

    Map map = make_map (16, shape, make_perlin (1.0 / 8.0, 2, seed));
    player.loc = find_passable_place (&map, make_loc (0, 0));
    init_game (map, player, spawn);
}

// This returns a pointer to the first item slot in 'owners'
// inventory; if 'owner' is null, this returns the first top level
// thing instead.
static Thing *
first_possible_thing (Thing * owner)
{
    if (owner == NULL)
        return game_things;
    else
    {
        int index = owner - game_things;
        if (index >= 0 && index < THING_COUNT)
            return &inventory_things[index * INVENTORY_COUNT];
        else
            return NULL;
    }
}

// This returns a pointer to the next item slot after the last one
// in'owners' inventory; if 'owner' is null, this returns the slot after
// the last one for the top level items. This is used to know when we've
// passed the end of the array of things we are iterating.
static Thing *
after_possible_things (Thing * owner)
{
    if (owner == NULL)
        return &game_things[THING_COUNT];
    else
    {
        int index = owner - game_things;
        if (index >= 0 && index < THING_COUNT)
            return &inventory_things[(index + 1) * INVENTORY_COUNT];
        else
            return NULL;
    }
}

/*
Returns the next slot in the things array. Start by passing
a NULL-initialized 'thing' buffer, and this will give you a pointer
to the first slot. Each call after that moves to the next slot.
Then, this method returns false and clears the buffer.
*/
static bool
next_possible_thing (Thing * owner, Thing ** thing)
{
    if (*thing == NULL)
    {
        *thing = first_possible_thing (owner);
        return true;
    }

    (*thing)++;

    if (*thing == after_possible_things (owner))
    {
        *thing = NULL;
        return false;
    }
    else
        return true;
}

/*
Returns the next thing in the things array, skipping any dead things.
Start by passing a NULL-initialized 'thing' buffer, and this will give
you a pointer to the first live thing. Each call after that moves to the
next slot. Then, this method returns false and clears the buffer.
*/
bool
next_thing (Thing * owner, Thing ** thing)
{
    while (next_possible_thing (owner, thing))
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
next_thing_at (Loc where, Thing ** found)
{
    while (next_thing (NULL, found))
    {
        if (equal_locs ((*found)->loc, where))
            return true;
    }

    return false;
}

/* Returns the map the game is played in. */
Map *
get_map (void)
{
    return &game_map;
}

/* Returns the player (who might be dead!) */
Thing *
get_player (void)
{
    return &game_things[PLAYER_INDEX];
}

/* Adds a new thing to the game, and returns a pointer to the thing in
the game itself, or NULL if there is no room for it. */
Thing *
new_thing (Thing thing)
{
    Thing *player = get_player ();
    for (Thing * candidate = NULL; next_possible_thing (NULL, &candidate);)
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
place_thing (Loc origin, Thing thing)
{
    thing.loc = find_empty_place (origin);
    return new_thing (thing);
}

/* Adds a (copy of) thing to the inventory of an owner, if there's
space. Returns a pointer to the inventory copy, or NULL if there's
no room. */
Thing *
copy_to_inventory (Thing * owner, Thing thing)
{
    for (Thing * th = NULL; next_possible_thing (owner, &th);)
    {
        if (!is_thing_alive (th))
        {
            *th = thing;
            return th;
        }
    }

    return NULL;
}

/* Returns a random location that is passable and contains no things. */
Loc
find_empty_place (Loc origin)
{
    for (;;)
    {
        Loc c = find_passable_place (get_map (), origin);

        Thing *start = NULL;
        if (!next_thing_at (c, &start))
            return c;
    }
}

/*
Removes a thing by clearing its appearance; we do not draw
a thing so treated, nor does it get a turn, nor can it be bumped
into.
*/
void
remove_thing (Thing * thing)
{
    thing->appearance = '\0';
}

/*
Tests to see if the thing given is still alive.
*/
bool
is_thing_alive (Thing * thing)
{
    return thing->appearance != DEAD;
}

/* Returns the total gold in all live things */
int
get_total_gold (void)
{
    int total = 0;
    for (Thing * th = NULL; next_thing (NULL, &th);)
    {
        total += th->gold;

        for (Thing * item = NULL; next_thing (th, &item);)
            total += th->gold;
    }

    return total;
}

/*
This is the main loop; this iterates over the things and
invokes their turn action. User input is handled in the
player's turn action; if the player is dead then this
function immediately exits.
*/
void
perform_turns (void)
{
    Thing *player = get_player ();

    Thing *actor = NULL;
    while (is_thing_alive (player))
    {
        if (next_thing (NULL, &actor))
        {
            if (actor->remaining_wait <= 0)
            {
                get_turn_action (actor) (actor);
                actor->remaining_wait += SPEED_MAX;
            }

            actor->remaining_wait -= actor->speed;

            if (actor == player)
                try_spawn_monster (game_spawn);
        }
    }
}

/*
This writes the game ot a file; this includes all the things
and the game-map as well.
*/
void
save_game (char *file_name)
{
    FILE *f = fopen (file_name, "w");

    write_spawn_settings (&game_spawn, f);

    for (int i = 0; i < THING_COUNT; ++i)
        write_thing (&game_things[i], f);

    for (int i = 0; i < THING_COUNT * INVENTORY_COUNT; ++i)
        write_thing (&inventory_things[i], f);

    write_map (&game_map, f);
    fclose (f);
}

/*
This restores the game from a file written by save_game().
*/
void
restore_game (char *file_name)
{
    FILE *f = fopen (file_name, "r");

    game_spawn = read_spawn_settings (f);

    for (int i = 0; i < THING_COUNT; ++i)
        game_things[i] = read_thing (f);

    for (int i = 0; i < THING_COUNT * INVENTORY_COUNT; ++i)
        inventory_things[i] = read_thing (f);

    game_map = read_map (f);
    fclose (f);
}

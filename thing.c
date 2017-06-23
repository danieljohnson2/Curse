#include "thing.h"
#include "game.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Returns the sign of the value givne */
static int
isign (int value)
{
    if (value > 0)
        return 1;
    else if (value < 0)
        return -1;
    else
        return 0;
}

/*
Create a thing in a single call.
*/
Thing
make_thing (char appearance, char *name, int x, int y, BumpAction bump_action,
            TurnAction turn_action)
{
    Thing th = { 0 };
    th.appearance = appearance;
    strcpy (th.name, name);
    th.x = x;
    th.y = y;
    th.bump_action = bump_action;
    th.turn_action = turn_action;
    return th;
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
    return thing->appearance != '\0';
}

/*
Executes a move for a monster; it moves horizontal or vertically
towards 'target', and will execute a bump-action if it reaches
it.
*/
void
move_thing_towards (Game * game, Thing * mover, Thing * target)
{
    int dx = target->x - mover->x;
    int dy = target->y - mover->y;

    if (abs (dx) > abs (dy))
    {
        if (!try_move_thing_by (game, mover, isign (dx), 0))
            try_move_thing_by (game, mover, 0, isign (dy));
    }
    else
    {
        if (!try_move_thing_by (game, mover, 0, isign (dy)))
            try_move_thing_by (game, mover, isign (dx), 0);
    }
}

/*
Moves the mover indicates in the direct indicated; this can
execute a bump action.

This returns false if the terrain is impassible where the thing
is moving, and true in any other case. It returns true if it
bumps something, in particular.

If this returns false, we can try to move in another direction.
*/
bool
try_move_thing_by (Game * game, Thing * mover, int dx, int dy)
{
    int nx = mover->x + dx;
    int ny = mover->y + dy;
    return try_move_thing_to (game, mover, nx, ny);
}

/*
Moves the player by an offset; this may trigger bump actions,
and writes a message (rather than retrying) if the player moves
into impassible terrain.
*/
void
move_player_by (Game * game, int dx, int dy)
{
    Thing *player = &game->things[PLAYER_INDEX];
    if (!try_move_thing_by (game, player, dx, dy))
        write_game_message (game, "Impassible!");
}

/*
Moves the 'mover' to the location indicated if possible.
If this location is impassible, this function returns false
and does nothing.
If there's something already there, this function invokes
its bump action, which may block the movement or may perform
other actions. In any of these cases, this method returns true.

True means 'something happend' here. We can't just try a
different location in such a case, lest two things happen.
*/
bool
try_move_thing_to (Game * game, Thing * mover, int x, int y)
{
    Terrain t = read_map (&game->map, x, y);

    if (!is_terrain_passable (t))
        return false;

    for (int i = 0; i < THING_COUNT && is_thing_alive (mover); ++i)
    {
        Thing *thing = &game->things[i];

        if (thing != mover && is_thing_alive (thing) &&
            thing->x == x && thing->y == y)
        {
            if (!thing->bump_action (game, mover, thing))
                return true;
        }
    }

    mover->x = x;
    mover->y = y;
    return true;
}

/* A turn action function that does nothing at all. */
void
null_turn_action (Game * game, Thing * actor)
{
}

/* A turn action function for monsters; they chse the player. */
void
chase_player_turn_action (Game * game, Thing * actor)
{
    Thing *player = &game->things[PLAYER_INDEX];

    if (is_thing_alive(player))
        move_thing_towards (game, actor, player);
}

/* A bump-action that does nothing and allows movement */
bool
null_bump_action (Game * game, Thing * actor, Thing * target)
{
    return true;
}

/* A bump-action that triggers combat */
bool
attack_bump_action (Game * game, Thing * actor, Thing * target)
{
    remove_thing (target);

    char msg[MESSAGE_MAX];
    sprintf (msg, "%s killed %s!", actor->name, target->name);
    write_game_message (game, msg);
    return false;
}

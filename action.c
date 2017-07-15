#include "action.h"
#include "map.h"
#include "game.h"
#include "thing.h"
#include "util.h"

#include <stdlib.h>
#include <limits.h>

/*
Executes a move for a monster; it moves horizontal or vertically
towards 'target', and will execute a bump-action if it reaches
it.
*/
void
move_thing_towards (Thing * mover, Thing * target)
{
    int dx = target->loc.x - mover->loc.x;
    int dy = target->loc.y - mover->loc.y;

    if (abs (dx) > abs (dy))
    {
        if (!try_move_thing_by (mover, isign (dx), 0))
            try_move_thing_by (mover, 0, isign (dy));
    }
    else
    {
        if (!try_move_thing_by (mover, 0, isign (dy)))
            try_move_thing_by (mover, isign (dx), 0);
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
try_move_thing_by (Thing * mover, int dx, int dy)
{
    Loc dest = offset_loc (mover->loc, dx, dy);
    return try_move_thing_to (mover, dest);
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
try_move_thing_to (Thing * mover, Loc dest)
{
    Terrain t = read_map (get_map (), dest);

    int speed_penalty = get_terrain_speed_penalty (t);
    if (speed_penalty == INT_MAX)
        return false;

    for (Thing * hit = NULL;
         is_thing_alive (mover) && next_thing_at (dest, &hit);)
    {
        if (hit != mover && !get_bump_action (hit) (mover, hit))
            return true;
    }

    mover->loc = dest;
    mover->remaining_wait += speed_penalty;
    return true;
}

/* A turn action function that does nothing at all. */
void
null_turn_action (Thing * actor)
{
}

/* A bump-action that does nothing and allows movement */
bool
null_bump_action (Thing * actor, Thing * target)
{
    return true;
}

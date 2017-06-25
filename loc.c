#include "loc.h"

/* Builds a Loc with the x and y given */
Loc
make_loc (int x, int y)
{
    Loc l;
    l.x = x;
    l.y = y;
    return l;
}

/* Adds two Locs and returns the result */
Loc
add_locs (Loc left, Loc right)
{
    left.x += right.x;
    left.y += right.y;
    return left;
}

/* Subtracts one loc from another */
Loc
subtract_locs (Loc left, Loc right)
{
    left.x -= right.x;
    left.y -= right.y;
    return left;
}

/* Creates a new Loc by applying a delta to an existing one */
Loc
offset_loc (Loc origin, int dx, int dy)
{
    return add_locs (origin, make_loc (dx, dy));
}

/* Decides if two locs are the same by comparing the fields. */
bool
equal_locs (Loc left, Loc right)
{
    return left.x == right.x && left.y == right.y;
}

#include "thing.h"
#include "util.h"

/*
Create a thing in a single call.
*/
Thing
make_thing (Appearance appearance, char *name, int speed,
            BumpAction bump_action, TurnAction turn_action)
{
    Thing th = { 0 };
    th.appearance = appearance;
    strtcpy (th.name, name, NAME_SIZE);
    th.speed = speed;
    th.remaining_wait = 0;
    th.bump_action = bump_action;
    th.turn_action = turn_action;
    return th;
}

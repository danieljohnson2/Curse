#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "thing.h"
#include <stdbool.h>

#define WEAPON_COUNT 6

Thing make_random_weapon (void);

bool weapon_pickup_bump_action (Thing * actor, Thing * target);

#endif

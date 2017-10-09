#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "thing.h"
#include <stdbool.h>

#define ITEM_COUNT 10

Thing make_random_item (void);

Thing *get_equipped_item (Thing * owner, ThingBehavior thing_type);
void equip_item (Thing * owner, Thing * item);

bool item_pickup_bump_action (Thing * actor, Thing * target);

#endif

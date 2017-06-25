#ifndef TREASURE_H_INCLUDED
#define TREASURE_H_INCLUDED

#include "thing.h"

#define TREASURE_COUNT 16

Thing make_random_treasure ();
Thing make_treasure (int gold);

#endif

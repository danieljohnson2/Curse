#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "thing.h"

Thing make_player (Map * map, TurnAction turn_action,
                   TurnAction skipped_turn_action);

#endif

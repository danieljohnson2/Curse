#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "thing.h"
#include "playeraction.h"

typedef struct _Map Map;

Thing make_player (Map * map, TurnAction turn_action);

void perform_player_action (PlayerAction action);
void move_player_by (int dx, int dy);

#endif

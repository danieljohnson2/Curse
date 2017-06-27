#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "defs.h"

Thing make_player (Map * map, TurnAction turn_action);

void perform_player_action (Game * game, PlayerAction action);
void move_player_by (Game * game, int dx, int dy);

#endif

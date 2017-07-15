#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "thing.h"
#include "playeraction.h"

typedef struct Map Map;

Thing make_player (Map * map);

void perform_player_action (PlayerAction action);
void move_player_by (int dx, int dy);

void player_turn_action (Thing * player);

#endif

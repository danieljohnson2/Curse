#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "thing.h"

typedef enum _PlayerAction
{
    None,
    Pass,
    Left,
    Up,
    Right,
    Down,
    Quit
} PlayerAction;

Thing make_player (Map * map, TurnAction turn_action);

void perform_player_action (Game * game, PlayerAction action);

#endif

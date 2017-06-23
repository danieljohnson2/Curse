#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "thing.h"
#include "map.h"

#define MESSAGE_MAX 512

typedef struct _Game
{
    Map map;
    int viewx, viewy;

    char message[MESSAGE_MAX + 1];
    Thing things[THING_COUNT];
} Game;

Game make_game (Map map);
void clear_game_message (Game * game);
void write_game_message (Game * game, char *message);

void move_player (Game * game, int dx, int dy);

#endif

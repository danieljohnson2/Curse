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

Game make_game (Map map, Thing player);
void clear_game_message (Game * game);
void write_game_message (Game * game, char *message);

bool find_thing_at (Game * game, int x, int y, Thing ** found);

int new_thing (Game * game, Thing thing);
int place_thing (Game * game, int originx, int originy, Thing thing);
void find_empty_place (Game * game, int originx, int originy, int *x, int *y);

bool is_thing_player (Game * game, Thing * thing);

void perform_turns (Game * game);

#endif

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

Game make_game (Map map, TurnAction player_turn_action);
void clear_game_message (Game * game);
void write_game_message (Game * game, char *message);

bool find_thing_at (Game * game, int x, int y, Thing ** found);
void place_thing (Game * game, Thing * thing);

bool is_thing_player (Game * game, Thing * thing);

void perform_turns (Game * game);

#endif

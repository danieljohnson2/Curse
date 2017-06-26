#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "defs.h"

void clear_game_message (Game * game);
void write_game_message (Game * game, char *message);
void consolidate_game_messages (Game * game);

#endif

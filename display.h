#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include "loc.h"
#include "playeraction.h"

#include <stdbool.h>

void init_windows (void);
void end_windows (void);
void game_over (void);

void paint (bool messages);

void center_view (Loc target);
PlayerAction read_player_action (void);

#endif

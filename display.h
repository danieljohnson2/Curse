#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

typedef struct _Game Game;
typedef struct _Thing Thing;
typedef enum _PlayerAction PlayerAction;

void init_windows (void);
void end_windows (void);
void wait_keystroke (void);

void paint (Game * game);

PlayerAction read_player_action (void);
void game_over (Game * game);

#endif

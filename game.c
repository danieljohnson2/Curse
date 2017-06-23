#include "game.h"

#include <string.h>

Game
make_game (Map map)
{
    Game game = { 0 };
    game.map = map;
    game.things[PLAYER_INDEX] = make_thing ('@', 0, -8);
    return game;
}

/* Clears the current message */
void
clear_game_message (Game * game)
{
    memset (game->message, '\0', MESSAGE_MAX + 1);
}

/* Appends a message to the current message. */
void
write_game_message (Game * game, char *message)
{
    if (game->message[0] == '\0')
        strncpy (game->message, message, MESSAGE_MAX);
    else
    {
        strncat (game->message, " ", MESSAGE_MAX);
        strncat (game->message, message, MESSAGE_MAX);
        game->message[MESSAGE_MAX] = '\0';      // ensure null termination
    }
}

void
move_player (Game * game, int dx, int dy)
{
    Thing *player = &game->things[PLAYER_INDEX];
    move_thing_to (game, player, player->x + dx, player->y + dy);
}

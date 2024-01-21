#include "game.h"
void InitLobby(void);
void UpdateLobby(GameState *state);
void DrawLobby(void);

typedef enum ButtonState
{
    DEFAULT,
    HOVER,
    DEPRESSED,
    CLICKED,

} ButtonState;

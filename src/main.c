#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emscripten/emscripten.h>
#include "raylib.h"
#include "main.h"
#include "lobby.h"
#include "board.h"
#include "shop.h"

void UpdateScreen(void);

int canvasWidth = 1000;
int canvasHeight = 1000;
int isFullscreen;

GameState state = BOARD;

int main(void)
{
    srand(time(NULL));
    InitWindow(canvasWidth, canvasHeight, "Beans");
    AddPlayer();
    InitBags();

    emscripten_set_main_loop(UpdateScreen, 0, 1);
    CloseWindow();
    return 0;
}

void UpdateScreen(void)
{

    BeginDrawing();
    switch (state)
    {
    case LOBBY:
        UpdateLobby(&state);
        DrawLobby();
        break;
    case BOARD:
        UpdateBoard(&state);
        DrawBoard();
        break;
    case SHOP:
        DrawAndUpdateShop(&state);
        break;
    default:
        break;
    }
    EndDrawing();
}

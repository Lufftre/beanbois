#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>
#include "main.h"
#include "raylib.h"
#include <time.h>
#include "board.h"
#include "lobby.h"

void UpdateScreen(void);

int canvasWidth = 1000;
int canvasHeight = 1000;
int isFullscreen;

GameState state = LOBBY;

int main(void)
{
    srand(time(NULL));
    InitWindow(canvasWidth, canvasHeight, "Beans");

    InitLobby();
    AddPlayer();
    AddPlayer();
    InitBags();

    emscripten_set_main_loop(UpdateScreen, 60, 1);
    CloseWindow();
    return 0;
}

void UpdateScreen(void)
{

    BeginDrawing();
    switch (state)
    {
    case LOBBY:
        UpdateLobby();
        DrawLobby();
        break;
    // case BOARD:
    //     UdpateBoard();
    //     DrawBoard();
    // case SHOP:
    //     UpdateShop();
    //     DrawShop();
    default:
        break;
    }
    EndDrawing();
}

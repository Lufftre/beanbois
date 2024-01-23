#include <stdlib.h>
#include "shop.h"
#include "board.h"
#include "raylib.h"
int currentShopper = 0;

void DrawAndUpdateShop(GameState *state)
{
    ClearBackground(BLACK);
    Vector2 mousePos = GetMousePosition();

    bool clicked = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    bool rclicked = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
    if (rclicked)
    {
        currentShopper++;
        if (boards[currentShopper].state == INACTIVE)
        {
            ResetBoards();
            *state = BOARD;
            currentShopper = 0;
            return;
        }
    }
    int i = currentShopper;
    Board *b = &boards[i];
    Bag *bag = &bags[i];

    int x = gutter + (gutter + boardSize) * (i % 3);
    int y = gutter + (gutter + boardSize) * (i / 3);

    Color c = b->state == INACTIVE ? GRAY : YELLOW;

    DrawRectangleLinesEx((Rectangle){x, y, boardRectSize, boardRectSize}, lineWidth, c);
    int chipCounter = 0;
    for (size_t chip_i = 0; chip_i < bag->nChips; chip_i++)
    {
        Chip *chip = &bag->chips[chip_i];
        DrawChip(chip, x, y, &chipCounter);
    }

    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    // Shop
    for (size_t chipType = 1; chipType < NCHIP_COLORS; chipType++)
    {
        Chip chip1 = {chipType, 1};
        Chip chip2 = {chipType, 2};
        Chip chip4 = {chipType, 4};
        int y = 200 + chipType * 100;
        int counter = 0;
        DrawChip(&chip1, 500, y, &counter);
        if (CheckCollisionPointRec(mousePos, (Rectangle){500, y, 30, 30}))
        {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            DrawRectangle(490, y + 10, 8, 8, WHITE);
            if (clicked && b->money >= chip1.value)
            {
                b->money -= chip1.value;
                bag->chips[bag->nChips++] = chip1;
            }
        }
        counter = 0;
        DrawChip(&chip2, 500, y + 30, &counter);
        if (CheckCollisionPointRec(mousePos, (Rectangle){500, y + 30, 30 * 2, 30}))
        {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            DrawRectangle(490, y + 40, 8, 8, WHITE);
            if (clicked && b->money >= chip2.value)
            {
                b->money -= chip2.value;
                bag->chips[bag->nChips++] = chip2;
            }
        }
        counter = 0;
        DrawChip(&chip4, 500, y + 60, &counter);
        if (CheckCollisionPointRec(mousePos, (Rectangle){500, y + 60, 30 * 4, 30}))
        {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            DrawRectangle(490, y + 70, 8, 8, WHITE);
            if (clicked && b->money >= chip4.value)
            {
                b->money -= chip4.value;
                bag->chips[bag->nChips++] = chip4;
            }
        }
    }
}

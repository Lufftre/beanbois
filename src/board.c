#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum BoardState
{
    ACTIVE,
    LOCKED,
    EXPLODED,

} BoardState;

typedef struct Chip
{
    int type;
    int value;
} Chip;

typedef struct Board
{
    int index;
    bool isHovered;
    BoardState state;
    Chip chips[256];
    int nChips;
    int whites;

} Board;

#define NBOARDS 9
Board boards[NBOARDS] = {
    {0},
    {1},
    {2},
    {3},
    {4},
    {5},
    {6},
    {7},
    {8},
};

const int chipSize = 20;
const int chipBoarder = 2;
const int chipsPerRow = 10;
const int boardSize = chipSize * chipsPerRow + (chipBoarder * (chipsPerRow + 1));
const int gutter = (1000 - boardSize * 3) / 4;
const int lineWidth = 4;
const int boardRectSize = boardSize + lineWidth * 2;

#define NCHIP_COLORS 4
const Color chipColors[NCHIP_COLORS] = {
    WHITE,
    // ORANGE,
    // GREEN,
    RED,
    BLUE,
    // BLACK,
    YELLOW,
    // PINK,
};

typedef struct Bag
{
    Chip chips[100];
    bool draws[100];
    int drawCounter;
    int nChips;
} Bag;

Bag bags[NBOARDS] = {
    {.chips = {
         {0, 1},
         {0, 1},
         {0, 1},
         {0, 1},
         {0, 2},
         {0, 2},
         {0, 3},
         {1, 1},
         {2, 1},
     },
     .nChips = 9}};

void shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void AddChip(Board *b, Bag *bag)
{



    int i = bag->draws[bag->drawCounter++];
    Chip chip = bag->chips[i];
    
    b->chips[b->nChips++] = chip;
    if (chip.type == 0)
    {
        b->whites += chip.value;
    }
    printf("Chip: %d %d %d\n", chip.type, chip.value, b->whites);
}

void DrawBoard(void)
{
    ClearBackground(DARKGRAY);
    Vector2 mousePos = GetMousePosition();

    bool clicked = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    for (size_t i = 0; i < NBOARDS; i++)
    {
        Board *b = &boards[i];
        int x = gutter + (gutter + boardSize) * (i % 3);
        int y = gutter + (gutter + boardSize) * (i / 3);
        b->isHovered = !(mousePos.x < x || mousePos.x > x + boardSize || mousePos.y < y || mousePos.y > y + boardSize);
        if (b->isHovered && clicked)
        {
            AddChip(b, &bags[0]);
        }
        Color c = b->isHovered ? RAYWHITE : YELLOW;
        DrawRectangleLinesEx((Rectangle){x, y, boardRectSize, boardRectSize}, lineWidth, c);
        int chipCounter = 0;
        for (size_t chip_i = 0; chip_i < b->nChips; chip_i++)
        {
            Chip *chip = &b->chips[chip_i];
            for (size_t _chip = 0; _chip < chip->value; _chip++)
            {
                int chipX = lineWidth + chipBoarder + x + (chipSize + chipBoarder) * (chipCounter % 10);
                int chipY = lineWidth + chipBoarder + y + (chipSize + chipBoarder) * (chipCounter / 10);
                DrawRectangle(chipX, chipY, chipSize, chipSize, chipColors[chip->type]);
                chipCounter++;
            }
        }
    }
}
#pragma once
#include "raylib.h"
#include "game.h"
#include <stdbool.h>
typedef enum BoardState
{
    INACTIVE,
    ACTIVE,
    LOCKED,
    EXPLODED,
    CHOOSING,

} BoardState;

typedef struct Chip
{
    int type;
    int value;
    int idx;
} Chip;

typedef struct Board
{
    int index;
    bool isHovered;
    BoardState state;
    Chip chips[256];
    int nChips;
    int whites;
    int money;

} Board;

typedef struct Bag
{
    Chip chips[256];
    Chip *drawChips[256];
    int draws[256];
    int chipsLeft;
    int nChips;
} Bag;

#define NCHIP_COLORS 8
extern const Color chipColors[NCHIP_COLORS];

extern const int chipSize;
extern const int chipBoarder;
extern const int chipsPerRow;
extern const int boardSize;
extern const int gutter;
extern const int lineWidth;
extern const int boardRectSize;

#define NBOARDS 9
extern Bag bags[NBOARDS];
extern Board boards[NBOARDS];

void DrawBoard(void);
void UpdateBoard(GameState *state);
void ResetBoards(void);

void DrawChip(Chip *chip, int x, int y, int *chipCounter);
void InitBag(Bag *bag);
void InitBags(void);
void ResetDrawBag(Bag *bag);

int AddPlayer(void);

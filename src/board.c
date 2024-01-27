#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"

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
Bag bags[NBOARDS];
Chip selectionChips[16];
int nSelections = 0;
const Color chipColors[NCHIP_COLORS] = {
    WHITE,
    ORANGE,
    GREEN,
    RED,
    BLUE,
    BLACK,
    YELLOW,
    PINK,
};

typedef enum EChip
{
    EWHITE,
    EORANGE,
    EGREEN,
    ERED,
    EBLUE,
    EBLACK,
    EYELLOW,
    EPINK,
} EChip;

const int chipSize = 20;
const int chipBoarder = 2;
const int chipsPerRow = 10;
const int boardSize = chipSize * chipsPerRow + (chipBoarder * (chipsPerRow + 1));
const int gutter = (1000 - boardSize * 3) / 4;
const int lineWidth = 4;
const int boardRectSize = boardSize + lineWidth * 2;

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

void ResetDrawBag(Bag *bag)
{
    for (size_t i = 0; i < bag->nChips; i++)
    {
        // bag->draws[i] = i;
        bag->drawChips[i] = &bag->chips[i];
    }
    // shuffle(bag->draws, bag->nChips);
}

void InitBag(Bag *bag)
{
    bag->chips[0] = (Chip){EWHITE, 1, 0};
    bag->chips[1] = (Chip){EWHITE, 1, 1};
    bag->chips[2] = (Chip){EWHITE, 1, 2};
    bag->chips[3] = (Chip){EWHITE, 1, 3};
    bag->chips[4] = (Chip){EWHITE, 2, 4};
    bag->chips[5] = (Chip){EWHITE, 2, 5};
    bag->chips[6] = (Chip){EWHITE, 3, 6};
    bag->chips[7] = (Chip){ERED, 1, 7};
    bag->chips[8] = (Chip){EBLUE, 1, 8};
    bag->nChips = 9;
    bag->chipsLeft = 9;

    ResetDrawBag(bag);
}

void InitBags(void)
{
    for (size_t i = 0; i < NBOARDS; i++)
    {
        InitBag(&bags[i]);
    }
    boards[0].money = 10000;
}

void ResetBoard(Board *b, Bag *bag)
{
    b->nChips = 0;
    b->whites = 0;
    b->state = ACTIVE;
    bag->chipsLeft = bag->nChips;
    ResetDrawBag(bag);
}

void ResetBoards()
{
    for (size_t i = 0; i < NBOARDS; i++)
    {
        Board *b = &boards[i];
        Bag *bag = &bags[i];
        if (b->state != INACTIVE)
        {
            ResetBoard(b, bag);
        }
    }
}

int AddPlayer(void)
{
    for (size_t i = 0; i < NBOARDS; i++)
    {
        if (boards[i].state == INACTIVE)
        {
            boards[i].state = ACTIVE;
            return i;
        }
    }
    return -1;
}
Chip TakeChip(Bag *bag)
{
    int i = rand() % bag->chipsLeft;
    Chip chip = *bag->drawChips[i];
    bag->drawChips[i] = bag->drawChips[bag->chipsLeft - 1];
    bag->chipsLeft--;
    return chip;
}
void ReturnChip(Bag *bag, Chip chip)
{
    Chip *c = &bag->chips[chip.idx];
    bag->drawChips[bag->chipsLeft++] = c;
}
void ChooseFrom(Bag *bag, int n)
{
    nSelections = n;
    for (size_t i = 0; i < n; i++)
    {
        selectionChips[i] = TakeChip(bag);
    }
}

int CountPot(Board *b, EChip type)
{
    int count = 0;
    for (size_t i = 0; i < b->nChips; i++)
    {
        if (b->chips[i].type == type)
            count++;
    }
    printf("count %d: %d", type, count);
    return count;
}

void AddChip(Board *b, Bag *bag)
{
    if (b->state == EXPLODED)
        return;

    if (bag->chipsLeft == 0)
    {
        printf("out of chips\n");
        return;
    }
    Chip chip = TakeChip(bag);
    // int i = bag->draws[bag->chipsLeft++];
    // Chip chip = bag->chips[i];

    Chip *prev = &b->chips[b->nChips - 2];
    Chip *cur = &b->chips[b->nChips - 1];
    int nCount;
    switch (chip.type)
    {
    case EWHITE:
        b->whites += chip.value;
        if (b->whites > 7)
        {
            b->state = EXPLODED;
        }
        break;

    case ERED:
        nCount = CountPot(b, EORANGE);
        if (nCount >= 3)
        {
            chip.value += 2;
        }
        else if (nCount >= 1)
        {
            chip.value += 1;
        }
        break;

    case EBLUE:
        b->state = CHOOSING;
        ChooseFrom(bag, chip.value);
        break;

    case EYELLOW:

        if (prev->type == EWHITE)
        {
            // Put back and reshuffle
            b->whites -= prev->value;
            cur->value += prev->value;
            *prev = *cur;
            b->nChips--;
        }
        break;

    default:
        break;
    }

    b->chips[b->nChips++] = chip;
}

void PutInBag(Bag *bag, Chip *chip)
{
    bag->drawChips[bag->chipsLeft] = chip;
    bag->chipsLeft++;
}

void LockBoard(Board *b)
{
    b->state = LOCKED;
    for (size_t i = 0; i < b->nChips; i++)
    {
        b->money += b->chips[i].value;
    }
}

bool RoundIsDone(void)
{
    for (size_t i = 0; i < NBOARDS; i++)
    {
        if (boards[i].state == ACTIVE)
        {
            return false;
        }
    }
    return true;
}

void UpdateBoard(GameState *state)
{
    Vector2 mousePos = GetMousePosition();

    for (size_t i = 0; i < NBOARDS; i++)
    {
        Board *b = &boards[i];
        int x = gutter + (gutter + boardSize) * (i % 3);
        int y = gutter + (gutter + boardSize) * (i / 3);
        if (b->state == ACTIVE)
        {
            b->isHovered = !(mousePos.x < x || mousePos.x > x + boardSize || mousePos.y < y || mousePos.y > y + boardSize);

            if (!b->isHovered)
                continue;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                AddChip(b, &bags[i]);
            }
            else if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
            {
                LockBoard(b);
                if (RoundIsDone())
                {
                    *state = SHOP;
                }
            }
        }
        // else if (b->state == CHOOSING)
        // {
        //     for (size_t selIndex = 0; selIndex < nSelections; selIndex++)
        //     {

        //     }
        // }
    }
}

void DrawBoard(void)
{
    ClearBackground(BLACK);
    bool clicked = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    Vector2 mousePos = GetMousePosition();

    for (size_t i = 0; i < NBOARDS; i++)
    {
        Board *b = &boards[i];

        Color c = YELLOW;
        switch (b->state)
        {
        case INACTIVE:
            c = GRAY;
            break;
        case ACTIVE:
            c = b->isHovered ? RAYWHITE : YELLOW;
            break;
        case EXPLODED:
            c = RED;
            break;
        case LOCKED:
            c = GREEN;
            break;
        default:
            c = MAGENTA;
            break;
        }

        int x = gutter + (gutter + boardSize) * (i % 3);
        int y = gutter + (gutter + boardSize) * (i / 3);
        DrawRectangleLinesEx((Rectangle){x, y, boardRectSize, boardRectSize}, lineWidth, c);

        int chipCounter = 0;
        if (b->state == CHOOSING)
        {
            chipCounter += 12;
            for (size_t selIndex = 0; selIndex < nSelections; selIndex++)
            {
                if (CheckCollisionPointRec(mousePos, (Rectangle){x, 30 + y + 20 * selIndex, 100, 20}))
                {
                    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
                    DrawRectangle(x - 10, 30 + y + 20 * selIndex, 8, 8, WHITE);
                    if (clicked)
                    {
                        b->chips[b->nChips++] = selectionChips[selIndex];
                        for (size_t ci = 0; ci < nSelections; ci++)
                        {
                            if (ci == selIndex)
                                continue;
                            ReturnChip(&bags[i], selectionChips[ci]);
                        }

                        nSelections = 0;
                        b->state = ACTIVE;
                        break;
                    }
                }
                DrawChip(&selectionChips[i], x, y, &chipCounter);
                chipCounter += 10 - selectionChips[i].value;
            }
        }
        else
        {
            for (size_t chip_i = 0; chip_i < b->nChips; chip_i++)
            {
                Chip *chip = &b->chips[chip_i];
                DrawChip(chip, x, y, &chipCounter);
            }
        }
    }
}

void DrawChip(Chip *chip, int x, int y, int *chipCounter)
{
    for (size_t _chip = 0; _chip < chip->value; _chip++)
    {
        int chipX = lineWidth + chipBoarder + x + (chipSize + chipBoarder) * (*chipCounter % 10);
        int chipY = lineWidth + chipBoarder + y + (chipSize + chipBoarder) * (*chipCounter / 10);
        DrawRectangle(chipX, chipY, chipSize, chipSize, chipColors[chip->type]);

        int connectorSize = 6;
        Rectangle connectorRec = {
            chipX - chipBoarder / 2,
            chipY + chipSize / 2,
            (connectorSize - chipBoarder) / 2,
            connectorSize,
        };
        if (_chip != chip->value - 1)
        {
            DrawRectanglePro(connectorRec, (Vector2){-(chipSize - 1), connectorSize / 2}, 0, BLACK);
        }
        if (_chip != 0)
        {
            DrawRectanglePro(connectorRec, (Vector2){-1, connectorSize / 2}, 0, BLACK);
        }
        (*chipCounter)++;
    }
}

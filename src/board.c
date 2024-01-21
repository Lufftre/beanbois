#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"
#include "main.h"
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

int currentShopper = 0;
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

void ShuffleBag(Bag *bag)
{
    for (size_t i = 0; i < bag->nChips; i++)
    {
        bag->draws[i] = i;
    }
    shuffle(bag->draws, bag->nChips);
    printf("Shuffled bag ");
    for (size_t i = 0; i < bag->nChips; i++)
    {
        printf("%d ", bag->draws[i]);
    }
    printf("\n");
}

void InitBag(Bag *bag)
{
    bag->chips[0] = (Chip){0, 1};
    bag->chips[1] = (Chip){0, 1};
    bag->chips[2] = (Chip){0, 1};
    bag->chips[3] = (Chip){0, 1};
    bag->chips[4] = (Chip){0, 2};
    bag->chips[5] = (Chip){0, 2};
    bag->chips[6] = (Chip){0, 3};
    bag->chips[7] = (Chip){1, 1};
    bag->chips[8] = (Chip){2, 1};
    bag->nChips = 9;
    
    // bag->chips[0] = (Chip){0, 3};
    // bag->chips[1] = (Chip){1, 3};
    // bag->chips[2] = (Chip){2, 3};
    // bag->chips[3] = (Chip){3, 3};
    // bag->nChips = 4;

    ShuffleBag(bag);
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
    bag->drawCounter = 0;
    ShuffleBag(bag);
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

void AddChip(Board *b, Bag *bag)
{
    if (b->state == EXPLODED)
        return;

    if (bag->drawCounter == bag->nChips)
    {
        printf("out of chips\n");
        return;
    }
    int i = bag->draws[bag->drawCounter++];
    Chip chip = bag->chips[i];

    b->chips[b->nChips++] = chip;
    if (chip.type == 0)
    {
        b->whites += chip.value;
    }
    printf("Chip: %d %d %d\n", chip.type, chip.value, b->whites);

    if (b->whites > 7)
    {
        b->state = EXPLODED;
    }
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

void DrawBoard(void)
{
    ClearBackground(DARKGRAY);
    Vector2 mousePos = GetMousePosition();

    bool clicked = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    bool rclicked = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);

    for (size_t i = 0; i < NBOARDS; i++)
    {
        Board *b = &boards[i];

        int x = gutter + (gutter + boardSize) * (i % 3);
        int y = gutter + (gutter + boardSize) * (i / 3);
        b->isHovered = !(mousePos.x < x || mousePos.x > x + boardSize || mousePos.y < y || mousePos.y > y + boardSize);

        if (b->state == ACTIVE && b->isHovered)
        {
            if (clicked)
            {
                AddChip(b, &bags[i]);
            }
            else if (rclicked)
            {
                LockBoard(b);
                if (RoundIsDone())
                {
                    currentShopper = 0;
                    state = SHOP;
                }
            }
        }

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
                chipCounter++;
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

void DrawShop(void)
{
    ClearBackground(DARKGRAY);
    Vector2 mousePos = GetMousePosition();

    bool clicked = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    bool rclicked = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
    if (rclicked)
    {
        currentShopper++;
        if (boards[currentShopper].state == INACTIVE)
        {
            ResetBoards();
            state = BOARD;
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
                printf("nChips: %d\n", bag->nChips);
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

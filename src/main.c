#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>
#include "main.h"
#include "raylib.h"
#include "scenes.h"
#include <time.h>

void UpdateScreen(void);

int canvasWidth = 1000;
int canvasHeight = 1000;
int isFullscreen;

GameState state = LOBBY;

// ENTRY
const char ENTRY_TEXT[] = "BeanBois";
const int ENTRY_FONTSIZE = 60;
int ENTRY_TEXTWIDTH;

Rectangle buttonJoinRec;
const char buttonJoinText[] = "Join";
const int buttonJoinFontSize = 30;
ButtonState buttonJoinState;

#define MAX_INPUT_CHARS 15
Rectangle textBox;
char name[MAX_INPUT_CHARS + 1] = "\0";
int letterCount = 0;
const int textBoxWidth = 500;
int main(void)
{
    srand(time(NULL));
    InitWindow(canvasWidth, canvasHeight, "Beans");

    // ENTRY
    ENTRY_TEXTWIDTH = MeasureText(ENTRY_TEXT, ENTRY_FONTSIZE);
    int buttonJoinTextWidth = MeasureText(buttonJoinText, buttonJoinFontSize);
    buttonJoinRec.x = (canvasWidth - buttonJoinTextWidth + textBoxWidth) / 2;
    buttonJoinRec.y = canvasHeight / 3 + ENTRY_FONTSIZE + 20;
    buttonJoinRec.width = buttonJoinTextWidth + 20;
    buttonJoinRec.height = buttonJoinFontSize + 10;
    textBox.x = buttonJoinRec.x - textBoxWidth - 25;
    textBox.y = buttonJoinRec.y;
    textBox.width = textBoxWidth;
    textBox.height = 50;

    emscripten_set_main_loop(UpdateScreen, 60, 1);
    CloseWindow();
    return 0;
}

void UpdateScreen(void)
{

    Vector2 mousePos = GetMousePosition();
    if (state == ENTRY)
    {

        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
            {
                name[letterCount] = (char)key;
                name[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
            }

            key = GetCharPressed(); // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            letterCount--;
            if (letterCount < 0)
                letterCount = 0;
            name[letterCount] = '\0';
        }

        if (CheckCollisionPointRec(mousePos, buttonJoinRec))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                buttonJoinState = DEPRESSED;
            }
            else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                buttonJoinState = CLICKED;
                state = LOBBY;
                printf("state -> LOBBY\n");
            }
            else
            {
                buttonJoinState = HOVER;
            }
        }
        else
            buttonJoinState = DEFAULT;
    }
    else if (state == LOBBY)
    {
    }

    BeginDrawing();
    if (state == ENTRY)
    {
        ClearBackground(DARKGRAY);
        DrawRectangleRec(textBox, LIGHTGRAY);
        DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
        Color c;
        if (buttonJoinState == HOVER)
            c = LIGHTGRAY;
        else if (buttonJoinState >= DEPRESSED)
            c = DARKGRAY;
        else
            c = GRAY;
        DrawRectangleRec(buttonJoinRec, c);
        DrawText(buttonJoinText, buttonJoinRec.x + 10, buttonJoinRec.y + 5, buttonJoinFontSize, WHITE);
    }
    else if (state == LOBBY)
    {
        DrawBoard();
    }
    EndDrawing();
}

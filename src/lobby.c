#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "lobby.h"

Rectangle buttonJoinRec;
const char buttonJoinText[] = "Join";
const int buttonJoinFontSize = 30;
ButtonState buttonJoinState;

#define MAX_INPUT_CHARS 15
#define ENTRY_FONTSIZE 12
Rectangle textBox;
char name[MAX_INPUT_CHARS + 1] = "\0";
int letterCount = 0;
const int textBoxWidth = 500;

void InitLobby(void)
{
    int buttonJoinTextWidth = MeasureText(buttonJoinText, buttonJoinFontSize);
    buttonJoinRec.x = (1000 - buttonJoinTextWidth + textBoxWidth) / 2;
    buttonJoinRec.y = 1000 / 3 + ENTRY_FONTSIZE + 20;
    buttonJoinRec.width = buttonJoinTextWidth + 20;
    buttonJoinRec.height = buttonJoinFontSize + 10;
    textBox.x = buttonJoinRec.x - textBoxWidth - 25;
    textBox.y = buttonJoinRec.y;
    textBox.width = textBoxWidth;
    textBox.height = 50;
}
void DrawLobby(void)
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

void UpdateLobby(void)
{
    Vector2 mousePos = GetMousePosition();
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
            // state = BOARD;
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
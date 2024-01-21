#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "lobby.h"
#define MAX_INPUT_CHARS 15
#define ENTRY_FONTSIZE 12

typedef struct Button{
    Rectangle rec;
    char text[16];
    int fontSize;
    ButtonState state;
} Button;

typedef struct InputBox {
    Rectangle rec;
    char input[MAX_INPUT_CHARS + 1];
    int count;
} InputBox;

InputBox inputName = {
    (Rectangle){320, 440, 360, 60}
};

void InitLobby(void)
{
    inputName.input[0] = '\0';
    inputName.count = 0;
}
void DrawLobby(void)
{
    ClearBackground(DARKGRAY);
    DrawRectangleRec(inputName.rec, LIGHTGRAY);
    if(inputName.count == 0)
        DrawText("Enter Name", (int)inputName.rec.x + 5, (int)inputName.rec.y + 8, 40, GRAY);
    else 
        DrawText(inputName.input, (int)inputName.rec.x + 5, (int)inputName.rec.y + 8, 40, MAROON);
}

void UpdateLobby(GameState *state)
{
    SetMouseCursor(MOUSE_CURSOR_IBEAM);
    int key = GetKeyPressed();
    while (key > 0)
    {
        if ((key >= 32) && (key <= 125) && (inputName.count < MAX_INPUT_CHARS))
        {
            inputName.input[inputName.count] = (char)key;
            inputName.input[inputName.count + 1] = '\0'; // Add null terminator at the end of the string.
            inputName.count++;
        }
        else if(key == KEY_BACKSPACE)
        {
            inputName.count--;
            if (inputName.count < 0)
                inputName.count = 0;
            inputName.input[inputName.count] = '\0';
        }
        else if (key == KEY_ENTER)
        {
            *state = BOARD;
        }

        key = GetKeyPressed(); // Check next character in the queue
    }
}
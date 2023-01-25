#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include "main.h"
#include "raylib.h"

void UpdateScreen(void);

EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData);
EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData);
EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData);
EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData);

// WS
#define SERVER_URL "ws://localhost:8100"
EMSCRIPTEN_WEBSOCKET_T ws;
unsigned short readyState;
char* peers[10];

int screenWidth;
int screenHeight;
int isFullscreen;


GameState state = ENTRY;

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

// LOBBY
Rectangle buttonStartRec;
const char buttonStartText[] = "Start";
const int buttonStartFontSize = 30;
ButtonState buttonStartState;

int main(void)
{
    emscripten_get_screen_size(&screenWidth, &screenHeight);
    screenHeight -= 130;
    screenWidth /= 2.0f;
    InitWindow(screenWidth, screenHeight, "Beans");

    // ENTRY
    ENTRY_TEXTWIDTH = MeasureText(ENTRY_TEXT, ENTRY_FONTSIZE);
    int buttonJoinTextWidth = MeasureText(buttonJoinText, buttonJoinFontSize);
    buttonJoinRec.x = (screenWidth - buttonJoinTextWidth + textBoxWidth) / 2;
    buttonJoinRec.y = screenHeight / 3 + ENTRY_FONTSIZE + 20;
    buttonJoinRec.width = buttonJoinTextWidth + 20;
    buttonJoinRec.height = buttonJoinFontSize + 10;
    textBox.x = buttonJoinRec.x - textBoxWidth - 25;
    textBox.y = buttonJoinRec.y;
    textBox.width = textBoxWidth;
    textBox.height = 50;

    // LOBBY
    int buttonStartTextWidth = MeasureText(buttonStartText, buttonStartFontSize);
    buttonStartRec.x = 500;
    buttonStartRec.y = 500;
    buttonStartRec.width = buttonStartTextWidth + 20;
    buttonStartRec.height = buttonStartFontSize + 10;

    emscripten_set_main_loop(UpdateScreen, 60, 1);
    CloseWindow();
    return 0;
}

Message message;
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
                buttonJoinState = DEPRESSED;
            else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                EmscriptenWebSocketCreateAttributes ws_attrs = {SERVER_URL, NULL, EM_TRUE};
                ws = emscripten_websocket_new(&ws_attrs);

                emscripten_websocket_set_onopen_callback(ws, NULL, onopen);
                emscripten_websocket_set_onerror_callback(ws, NULL, onerror);
                emscripten_websocket_set_onclose_callback(ws, NULL, onclose);
                emscripten_websocket_set_onmessage_callback(ws, NULL, onmessage);
                buttonJoinState = CLICKED;
                state = LOBBY;
            }
            else
                buttonJoinState = HOVER;
        }
        else
            buttonJoinState = DEFAULT;
    }
    else if (state == LOBBY)
    {
        emscripten_websocket_get_ready_state(ws, &readyState);
        if (CheckCollisionPointRec(mousePos, buttonStartRec))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                buttonStartState = DEPRESSED;
            else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                buttonStartState = CLICKED;
                message.op = START_GAME;
                emscripten_websocket_send_binary(ws, &message, sizeof(Message));

                // state = STARTED;
            }
            else
                buttonStartState = HOVER;
        }
        else
            buttonStartState = DEFAULT;
    }

    BeginDrawing();
    if (state == ENTRY)
    {
        ClearBackground(RAYWHITE);
        DrawText(ENTRY_TEXT, (screenWidth - ENTRY_TEXTWIDTH) / 2, screenHeight / 3, ENTRY_FONTSIZE, BLACK);

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
        ClearBackground(BLACK);
        if (readyState == 1)
        {
            int ipeer = 0;
            for (size_t i = 0; i < 10; i++)
            {
                if(peers[i] != NULL)
                {
                    DrawText(peers[i], 150, 150 + ipeer*40, 30, RAYWHITE);
                    ipeer++;
                }
            }
            
            char text[] = "Connected";
            int ft = 30;
            int w = MeasureText(text, ft);
            DrawText(text, screenWidth - w - 150, screenHeight - ft - 150, ft, RAYWHITE);

            Color c;
            if (buttonStartState == HOVER)
                c = LIGHTGRAY;
            else if (buttonStartState >= DEPRESSED)
                c = DARKGRAY;
            else
                c = GRAY;
            DrawRectangleRec(buttonStartRec, c);
            DrawText(buttonStartText, buttonStartRec.x + 10, buttonStartRec.y + 5, buttonStartFontSize, WHITE);
        }
        else
        {
            char text[] = "Waiting to connect...";
            int ft = 30;
            int w = MeasureText(text, ft);
            DrawText(text, screenWidth - w - 150, screenHeight - ft - 150, ft, RAYWHITE);
        }
    }
    else if (state == STARTED)
    {
        ClearBackground(RAYWHITE);
    }
    EndDrawing();
}

JoinMessage joinMessage;
EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData)
{
    puts("onopen");
    joinMessage.op = JOIN;
    strcpy(joinMessage.name, name);
    emscripten_websocket_send_binary(ws, &joinMessage, sizeof(JoinMessage));
    return EM_TRUE;
}

EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData)
{
    puts("onerror");
    return EM_TRUE;
}

EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData)
{
    puts("onclose");
    return EM_TRUE;
}

EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData)
{
    puts("onmessage");
    printf("%s", websocketEvent->data);

    Message *msg = (Message *)websocketEvent->data;
    printf("message op: %d\n", msg->op);
    if (msg->op == PLAYER_JOIN)
    {
        JoinMessage *msg =  (JoinMessage *)websocketEvent->data;
        // JoinMessage *joinmsg = malloc(sizeof(JoinMessage));
        char* joiningPlayer = malloc(sizeof(msg->name));
        strcpy(joiningPlayer, msg->name);
        printf("Player %s has joined\n", joiningPlayer);
        for (int i = 0; i < 10; i++)
        {
            printf("peers %d %s\n", i, peers[i]);
            if(peers[i] == NULL)
            {
                printf("adding %s\n", joiningPlayer);
                peers[i] = joiningPlayer;
                break;
            }
        }
        
    }
    else if (msg->op == PLAYER_LEFT)
    {
        JoinMessage *msg =  (JoinMessage *)websocketEvent->data;
        printf("Player %s has left\n", msg->name);
        for (size_t i = 0; i < 10; i++)
        {
            if(strcmp(peers[i], msg->name) == 0)
            {
                free(peers[i]);
                peers[i] = NULL;
                break;
            }
        }
    }
    else if (msg->op == START_GAME)
    {
        state = STARTED;
    }

    return EM_TRUE;
}
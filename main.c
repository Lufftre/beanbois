#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>
#include "main.h" 
#include "raylib.h"

#define SERVER_URL "ws://localhost:8100"
EMSCRIPTEN_WEBSOCKET_T ws;

int screenWidth = 800;
int screenHeight = 600;

int main(void)
{
    
    InitWindow(screenWidth, screenHeight, "Beans");

    EmscriptenWebSocketCreateAttributes ws_attrs = {SERVER_URL, NULL, EM_TRUE};
    ws = emscripten_websocket_new(&ws_attrs);

    emscripten_websocket_set_onopen_callback(ws, NULL, onopen);
    emscripten_websocket_set_onerror_callback(ws, NULL, onerror);
    emscripten_websocket_set_onclose_callback(ws, NULL, onclose);
    emscripten_websocket_set_onmessage_callback(ws, NULL, onmessage);

    emscripten_set_main_loop(UpdateScreen, 60, 1);
    CloseWindow();
    return 0;
}

void UpdateScreen(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}


EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData)
{
    puts("onopen");
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
    return EM_TRUE;
}
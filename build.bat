@echo off
emcc ^
    -o build/main.html^
    ./src/main.c^
    ./src/lobby.c^
    ./src/board.c^
    -O0 -Wall -std=c99^
    C:/raylib/src/libraylib.a^
    -I C:/raylib/src^
    -I C:/raylib/src/external^
    -I C:/raylib/src/extras^
    -s USE_GLFW=3^
    -s GL_ENABLE_GET_PROC_ADDRESS^
    -D PLATFORM_WEB^
    --shell-file ./resc/shell.html
@REM -l websocket.js^
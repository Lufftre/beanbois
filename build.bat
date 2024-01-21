@echo off
emcc ^
    -o build/main.html^
    ./src/main.c^
    ./src/board.c^
    ./src/lobby.c^
    -O0 -Wall -std=c99^
    ..\raylib\src\libraylib.a^
    -I ../raylib/src^
    -I ../raylib/src/external^
    -I ../raylib/src/extras^
    -s USE_GLFW=3^
    -s GL_ENABLE_GET_PROC_ADDRESS^
    -D PLATFORM_WEB^
    --shell-file ./resc/shell.html
@REM -l websocket.js^
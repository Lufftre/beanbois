gcc ^
    .\src\server.c^
    -std=c99^
    -I ..\wsServer\include\^
    ..\wsServer\build\libws.a^
    -l wsock32^
    -l Ws2_32^
    -o build\server.exe
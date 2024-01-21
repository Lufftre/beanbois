typedef enum GameState
{
    LOBBY,
    BOARD,
    SHOP,

} GameState;


typedef struct Message
{
    int op;
    char name[16];
} Message;

extern GameState state;
 
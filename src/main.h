typedef enum GameState
{
    ENTRY,
    LOBBY,

} GameState;

typedef enum ButtonState
{
    DEFAULT,
    HOVER,
    DEPRESSED,
    CLICKED,

} ButtonState;

typedef struct Message
{
    int op;
    char name[16];
} Message;
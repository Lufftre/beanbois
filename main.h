typedef enum GameState
{
    ENTRY,
    LOBBY,
    STARTED,

} GameState;

typedef enum ButtonState
{
    DEFAULT,
    HOVER,
    DEPRESSED,
    CLICKED,

} ButtonState;

typedef enum MessageType
{
    JOIN,
    LEAVE,
    PLAYER_JOIN,
    PLAYER_LEFT,
    START_GAME,

} MessageType;

typedef struct Message
{
    MessageType op;
} Message;

typedef struct JoinMessage
{
    MessageType op;
    char name[16];
} JoinMessage;



typedef enum BoardState
{
    INACTIVE,
    ACTIVE,
    LOCKED,
    EXPLODED,

} BoardState;

typedef struct Chip
{
    int type;
    int value;
} Chip;

typedef struct Board
{
    int index;
    bool isHovered;
    BoardState state;
    Chip chips[256];
    int nChips;
    int whites;
    int money;

} Board;

typedef struct Bag
{
    Chip chips[256];
    int draws[256];
    int drawCounter;
    int nChips;
} Bag;

#define NBOARDS 9
extern Bag bags[NBOARDS];

void DrawBoard(void);
void DrawShop(void);
void InitBag(Bag *bag);
void InitBags(void);
int AddPlayer(void);
void ShuffleBag(Bag *bag);

typedef struct
{
    int w;
    int h;
    int **board;
} Board;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct
{
    int x;
    int y;
} Point;

typedef struct Node
{
    Point *p;
    Direction dir;
    struct Node *next;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
    int length;
} Snake;

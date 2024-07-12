#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "raylib.h"
#include "snake.h"

Node *createNode(int x, int y, Direction dir, Node *next)
{
    Node *node = malloc(sizeof(Node));
    node->p = malloc(sizeof(Point));
    node->dir = dir;
    node->next = next;
    node->p->x = x;
    node->p->y = y;
    return node;
}

void freeNode(Node *node)
{
    free(node->p);
    free(node);
}

void growSnake(Snake *snake, int length)
{
    snake->length += length;
    Node *current = snake->tail;
    int x = snake->tail->p->x;
    int y = snake->tail->p->y;
    for (int i = 0; i < length; i++)
    {
        if (snake->tail->dir == UP)
        {
            y = snake->tail->p->y + i + 1;
        }
        else if (snake->tail->dir == DOWN)
        {
            y = snake->tail->p->y - i - 1;
        }
        else if (snake->tail->dir == LEFT)
        {
            x = snake->tail->p->x + i + 1;
        }
        else if (snake->tail->dir == RIGHT)
        {
            x = snake->tail->p->x - i - 1;
        }
        current->next = createNode(x, y, snake->tail->dir, NULL);
        current = current->next;
    }
    snake->tail = current;
}

void grow(Snake *snake)
{
    growSnake(snake, 1);
}

Snake *createSnake(int x, int y, int length, Direction startDir)
{
    Snake *snake = malloc(sizeof(Snake));
    Node *head = createNode(x, y, startDir, NULL);
    snake->length = length;
    snake->head = head;
    snake->tail = head;
    if (length > 1)
    {
        growSnake(snake, length - 1);
    }
    return snake;
}

void freeSnake(Snake *snake)
{
    Node *current = snake->head;
    while (current != NULL)
    {
        Node *next = current->next;
        freeNode(current);
        current = next;
    }
}

char *get_direction_s(Direction dir)
{
    switch (dir)
    {
    case LEFT:
        return "Left";
    case RIGHT:
        return "Right";
    case UP:
        return "Up";
    case DOWN:
        return "Down";
    }
    return "None";
}

void printSnake(Snake *snake) // create rect
                              // draw rect
{
    Node *current = snake->head;
    printf("length=%d\n", snake->length);
    while (current != NULL)
    {
        Node *next = current->next;
        printf("[%d, %d, %s]", current->p->x, current->p->y, get_direction_s(current->dir));
        current = next;
    }
}

void rand_point(Point *point, int max_x, int max_y)
{
    point->x = rand() % (max_x + 1) + 1;
    point->y = rand() % (max_y + 1) + 1;
}

const int screen_width = 1000;
const int screen_height = 1000;

const int board_width = 50;
const int board_height = 50;

const int square_size = screen_width / board_width;

const double tickrate = 50;

int main(void)
{
    time_t t;
    srand((unsigned)time(&t));

    Snake *snake = createSnake(board_width / 2, board_height / 2, 1, LEFT);

    Point *apple = malloc(sizeof(Point));
    rand_point(apple, board_width, board_height);

    printSnake(snake);

    InitWindow(screen_width, screen_height, "snake");
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetTargetFPS(60);

    double tickrate_sum = 1;
    Direction prev_dir = snake->head->dir;
    bool game_over = false;
    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (game_over)
        {
            ClearBackground(BLACK);
            DrawText("GAME OVER", screen_width / 2-200, screen_height / 2-200, 60, RED);
            DrawText(TextFormat("Your score: %d", snake->length), screen_width / 2-200, screen_height / 2-50, 60, BLUE);
            DrawText("Press enter to play again", screen_width / 2-200, screen_height / 2-100, 30, GREEN);
            
            if (IsKeyPressed(KEY_ENTER))
            {
                game_over = false;
                freeSnake(snake);
                snake = createSnake(board_width / 2, board_height / 2, 1, LEFT);
            }
        }
        else
        {
            BeginDrawing();
            tickrate_sum += GetFrameTime();
            if (snake->head->dir != LEFT && snake->head->dir != RIGHT)
            {
                if (IsKeyPressed(KEY_LEFT))
                    snake->head->dir = LEFT;
                if (IsKeyPressed(KEY_RIGHT))
                    snake->head->dir = RIGHT;
            }

            if (snake->head->dir != UP && snake->head->dir != DOWN)
            {
                if (IsKeyPressed(KEY_UP))
                    snake->head->dir = UP;
                if (IsKeyPressed(KEY_DOWN))
                    snake->head->dir = DOWN;
            }

            prev_dir = snake->head->dir;

            DrawText(TextFormat("Length: %d", snake->length), 100, 50, 30, WHITE);

            DrawRectangle(apple->x * square_size, apple->y * square_size, square_size, square_size, YELLOW);
            if (snake->head->p->x == apple->x && snake->head->p->y == apple->y)
            {
                rand_point(apple, board_width, board_height);
                grow(snake);
            }

            // mode: hard wall
            if (snake->head->p->x < 0 || snake->head->p->x > board_width ||
                snake->head->p->y < 0 || snake->head->p->y > board_height)
                game_over = true;
            // mode: teleport wall

            if (tickrate_sum * 1000 > tickrate)
            {
                // printSnake(snake);

                Node *current = snake->head;

                while (current != NULL)
                {
                    Node *next = current->next;
                    // update positions based off direction
                    DrawRectangle(snake->head->p->x * square_size, snake->head->p->y * square_size, square_size, square_size, RED);

                    if (current->dir == UP)
                        current->p->y -= 1;
                    else if (current->dir == DOWN)
                        current->p->y += 1;
                    else if (current->dir == LEFT)
                        current->p->x -= 1;
                    else if (current->dir == RIGHT)
                        current->p->x += 1;

                    if (current != snake->head)
                    {
                        Direction tmp = current->dir;
                        current->dir = prev_dir;
                        prev_dir = tmp;
                        DrawRectangle(current->p->x * square_size, current->p->y * square_size, square_size, square_size, GREEN);
                    }

                    current = next;
                }
                tickrate_sum = 0;
                ClearBackground(BLACK);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    freeSnake(snake);
    free(apple);

    return 0;
}

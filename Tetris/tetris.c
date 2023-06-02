#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

#define LEFT_KEY 0x25     // The key to move left, default = 0x25 (left arrow)
#define RIGHT_KEY 0x27    // The key to move right, default = 0x27 (right arrow)
#define ROTATE_KEY 0x26   // The key to rotate, default = 0x26 (up arrow)
#define DOWN_KEY 0x28     // The key to move down, default = 0x28 (down arrow)
#define FALL_KEY 0x20     // The key to fall, default = 0x20 (spacebar)
#define HOLD_KEY 0x43
#define QUIT_KEY 0x51

#define FALL_DELAY 500    // The delay between each fall, default = 500
#define RENDER_DELAY 100  // The delay between each frame, default = 100

#define LEFT_FUNC() GetAsyncKeyState(LEFT_KEY) & 0x8000
#define RIGHT_FUNC() GetAsyncKeyState(RIGHT_KEY) & 0x8000
#define ROTATE_FUNC() GetAsyncKeyState(ROTATE_KEY) & 0x8000
#define DOWN_FUNC() GetAsyncKeyState(DOWN_KEY) & 0x8000
#define FALL_FUNC() GetAsyncKeyState(FALL_KEY) & 0x8000
#define HOLD_FUNC() GetAsyncKeyState(HOLD_KEY) & 0x8000
#define QUIT_FUNC() GetAsyncKeyState(QUIT_KEY) & 0x8000

#define CANVAS_WIDTH 10
#define CANVAS_HEIGHT 20

typedef enum {
    RED = 41,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE,
    BLACK = 0,
}Color;

typedef enum {
    EMPTY = -1,
    I,
    J,
    L,
    O,
    S,
    T,
    Z
}ShapeId;

typedef struct {
    ShapeId shape;
    Color color;
    int size;
    char rotates[4][4][4];
}Shape;

typedef struct
{
    int x;
    int y;
    int score;
    int rotate;
    int fallTime;
    ShapeId queue[4];
}State;

typedef struct {
    Color color;
    ShapeId shape;
    bool current;
}Block;

Shape shapes[8] = {
    {
        .shape = I,
        .color = CYAN,
        .size = 4,
        .rotates =
        {
            {
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0}
            },
            {
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0}
            }
        }
    },
    {
        .shape = J,
        .color = BLUE,
        .size = 3,
        .rotates =
        {
            {
                {1, 0, 0},
                {1, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 1, 1},
                {0, 1, 0},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {0, 0, 1}
            },
            {
                {0, 1, 0},
                {0, 1, 0},
                {1, 1, 0}
            }
        }
    },
    {
        .shape = L,
        .color = YELLOW,
        .size = 3,
        .rotates =
        {
            {
                {0, 0, 1},
                {1, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 1, 0},
                {0, 1, 0},
                {0, 1, 1}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {1, 0, 0}
            },
            {
                {1, 1, 0},
                {0, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = O,
        .color = WHITE,
        .size = 2,
        .rotates =
        {
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            }
        }
    },
    {
        .shape = S,
        .color = GREEN,
        .size = 3,
        .rotates =
        {
            {
                {0, 1, 1},
                {1, 1, 0},
                {0, 0, 0}
            },
            {
                {0, 1, 0},
                {0, 1, 1},
                {0, 0, 1}
            },
            {
                {0, 0, 0},
                {0, 1, 1},
                {1, 1, 0}
            },
            {
                {1, 0, 0},
                {1, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = T,
        .color = PURPLE,
        .size = 3,
        .rotates =
        {
            {
                {0, 1, 0},
                {1, 1, 1},
                {0, 0, 0}
            },

                {{0, 1, 0},
                {0, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 1, 0},
                {1, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = Z,
        .color = RED,
        .size = 3,
        .rotates =
        {
            {
                {1, 1, 0},
                {0, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 0, 1},
                {0, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 0},
                {0, 1, 1}
            },
            {
                {0, 1, 0},
                {1, 1, 0},
                {1, 0, 0}
            }
        }
    },
    {
        .shape = EMPTY,
        .color = BLACK,
        .size = 4,
        .rotates =
        {
            {
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1}
            },
            {
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1}
            },
            {
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1}
            },
            {
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1}
            }
        }
    },
};

Shape Hold = {.size = 0};
bool isHold = false;
int scoremode = 0;
bool programContinue = true;

void setBlock(Block* block, Color color, ShapeId shape, bool current)
{
    block->color = color;
    block->shape = shape;
    block->current = current;
}

void resetBlock(Block* block)
{
    block->color = BLACK;
    block->shape = EMPTY;
    block->current = false;
}

void cleanShape(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int originalX, int originalY, int originalRotate, ShapeId shapeId) {
    Shape shapeData = shapes[shapeId];
    int size = shapeData.size;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[originalRotate][i][j]) {
                resetBlock(&canvas[originalY + i][originalX + j]);
            }
        }
    }
}

bool move(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int originalX, int originalY, int originalRotate, int newX, int newY, int newRotate, ShapeId shapeId) {
    Shape shapeData = shapes[shapeId];
    int size = shapeData.size;

    // check if the new position is valid to place the block
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[newRotate][i][j]) {
                if (newX + j < 0 || newX + j >= CANVAS_WIDTH || newY + i < 0 || newY + i >= CANVAS_HEIGHT) {
                    return false;
                }
                if (!canvas[newY + i][newX + j].current && canvas[newY + i][newX + j].shape != EMPTY) {
                    return false;
                }
            }
        }
    }

    // remove the old position
    cleanShape(canvas, originalX, originalY, originalRotate, shapeId);

    // move the block
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[newRotate][i][j]) {
                setBlock(&canvas[newY + i][newX + j], shapeData.color, shapeId, true);
            }
        }
    }

    return true;
}

void printsplash(void) {

    printf("     /\\    \\                  /\\    \\              /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\\n");
    printf("    /::\\    \\                /::\\    \\            /::\\    \\                /::\\    \\                /::\\    \\                /::\\    \\\n");
    printf("    \\:::\\    \\              /::::\\    \\           \\:::\\    \\              /::::\\    \\               \\:::\\    \\              /::::\\    \\\n");
    printf("     \\:::\\    \\            /::::::\\    \\           \\:::\\    \\            /::::::\\    \\               \\:::\\    \\            /::::::\\    \\\n");
    printf("      \\:::\\    \\          /:::/\\:::\\    \\           \\:::\\    \\          /:::/\\:::\\    \\               \\:::\\    \\          /:::/\\:::\\    \\\n");
    printf("       \\:::\\    \\        /:::/__\\:::\\    \\           \\:::\\    \\        /:::/__\\:::\\    \\               \\:::\\    \\        /:::/__\\:::\\    \\\n");
    printf("       /::::\\    \\      /::::\\   \\:::\\    \\          /::::\\    \\      /::::\\   \\:::\\    \\              /::::\\    \\       \\:::\\   \\:::\\    \\\n");
    printf("      /::::::\\    \\    /::::::\\   \\:::\\    \\        /::::::\\    \\    /::::::\\   \\:::\\    \\    ____    /::::::\\    \\    ___\\:::\\   \\:::\\    \\\n");
    printf("     /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\      /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\____\\  /\\   \\  /:::/\\:::\\    \\  /\\   \\:::\\   \\:::\\    \\\n");
    printf("    /:::/  \\:::\\____\\/:::/__\\:::\\   \\:::\\____\\    /:::/  \\:::\\____\\/:::/  \\:::\\   \\:::|    |/::\\   \\/:::/  \\:::\\____\\/::\\   \\:::\\   \\:::\\____\\\n");
    printf("   /:::/    \\::/    /\\:::\\   \\:::\\   \\::/    /   /:::/    \\::/    /\\::/   |::::\\  /:::|____|\\:::\\  /:::/    \\::/    /\\:::\\   \\:::\\   \\::/    /\n");
    printf("  /:::/    / \\/____/  \\:::\\   \\:::\\   \\/____/   /:::/    / \\/____/  \\/____|:::::\\/:::/    /  \\:::\\/:::/    / \\/____/  \\:::\\   \\:::\\   \\/____/\n");
    printf(" /:::/    /            \\:::\\   \\:::\\    \\      /:::/    /                 |:::::::::/    /    \\::::::/    /            \\:::\\   \\:::\\    \\\n");
    printf("/:::/    /              \\:::\\   \\:::\\____\\    /:::/    /                  |::|\\::::/    /      \\::::/____/              \\:::\\   \\:::\\____\\\n");
    printf("\\::/    /                \\:::\\   \\::/    /    \\::/    /                   |::| \\::/____/        \\:::\\    \\               \\:::\\  /:::/    /\n");
    printf(" \\/____/                  \\:::\\   \\/____/      \\/____/                    |::|  ~|               \\:::\\    \\               \\:::\\/:::/    /\n");
    printf("                           \\:::\\    \\                                     |::|   |                \\:::\\    \\               \\::::::/    /\n");
    printf("                            \\:::\\____\\                                    \\::|   |                 \\:::\\____\\               \\::::/    /\n");
    printf("                             \\::/    /                                     \\:|   |                  \\::/    /                \\::/    /\n");
    printf("                              \\/____/                                       \\|___|                   \\/____/                  \\/____/\n");
    printf("\n\n\n\nWelcome to tetris!!!");
}

void printanimation(int mode) {
    if (mode == 0) {
        printf("\033[%d;%dH%55c", 15, CANVAS_WIDTH * 2 + 25, ' ');
        printf("\033[%d;%dH%55c", 16, CANVAS_WIDTH * 2 + 25, ' ');
        printf("\033[%d;%dH%55c", 17, CANVAS_WIDTH * 2 + 25, ' ');
        printf("\033[%d;%dH%55c", 18, CANVAS_WIDTH * 2 + 25, ' ');
        printf("\033[%d;%dH%55c", 19, CANVAS_WIDTH * 2 + 25, ' ');
        printf("\033[%d;%dH%55c", 20, CANVAS_WIDTH * 2 + 25, ' ');
        printf("\033[%d;%dH%55c", 21, CANVAS_WIDTH * 2 + 25, ' ');
    }
    else if (mode == 1) {
        printf("\033[%d;%dH", 15, CANVAS_WIDTH * 2 + 25);
        printf("  ####     ####    ##   #   #####    #         #####\n");
        printf("\033[%d;%dH", 16, CANVAS_WIDTH * 2 + 25);
        printf(" ### ##    ####   #### ##  ## ####   ##       ####\n");
        printf("\033[%d;%dH", 17, CANVAS_WIDTH * 2 + 25);
        printf(" ##        ####   #######  ##        ##       #\n");
        printf("\033[%d;%dH", 18, CANVAS_WIDTH * 2 + 25);
        printf("  ####      ##    ## ####  ##  ###   ##       ####\n");
        printf("\033[%d;%dH", 19, CANVAS_WIDTH * 2 + 25);
        printf("     ##     ##    ##  ###  ##   ##   ##       ##\n");
        printf("\033[%d;%dH", 20, CANVAS_WIDTH * 2 + 25);
        printf("### ###     ##    ##   ##  ### ###   ## ###   ###\n");
        printf("\033[%d;%dH", 21, CANVAS_WIDTH * 2 + 25);
        printf(" #####       #     #   #    #####    # ####    #####\n");
    }
    else if (mode == 2) {
        printf("\033[%d;%dH", 15, CANVAS_WIDTH * 2 + 25);
        printf("####      #####   ##   ##    #####   ##        ######\n");
        printf("\033[%d;%dH", 16, CANVAS_WIDTH * 2 + 25);
        printf("#  ###   ###  ##  ##   ##   ###  ##  ###      ####\n");
        printf("\033[%d;%dH", 17, CANVAS_WIDTH * 2 + 25);
        printf("##  ###  ##   ##  ##   ##  ##   ##   ###      #\n");
        printf("\033[%d;%dH", 18, CANVAS_WIDTH * 2 + 25);
        printf("##  ###  ##   ##  ##   ##  ## ###    ##       ####\n");
        printf("\033[%d;%dH", 19, CANVAS_WIDTH * 2 + 25);
        printf("##  ###  ##   ##  ###  ##  ##   ##   ##       ##\n");
        printf("\033[%d;%dH", 20, CANVAS_WIDTH * 2 + 25);
        printf("#  ###   ### ###  ###  ##   # ####   ## ###   ###\n");
        printf("\033[%d;%dH", 21, CANVAS_WIDTH * 2 + 25);
        printf("#####     #####    #####     ######  # ####    #####\n");
    }
    else if (mode == 3) {
        printf("\033[%d;%dH", 15, CANVAS_WIDTH * 2 + 25);
        printf(" ######  ######     ####    #####    ##        ######\n");
        printf("\033[%d;%dH", 16, CANVAS_WIDTH * 2 + 25);
        printf("######   ###  ##    ####   ###  ##   ###      ####\n");
        printf("\033[%d;%dH", 17, CANVAS_WIDTH * 2 + 25);
        printf("   ##    ##   ##    ####   ##   ##   ###      #\n");
        printf("\033[%d;%dH", 18, CANVAS_WIDTH * 2 + 25);
        printf("   ##    ##  ##      ##    ## ###    ##       ####\n");
        printf("\033[%d;%dH", 19, CANVAS_WIDTH * 2 + 25);
        printf("   ##    #####       ##    ####      ##       ##\n");
        printf("\033[%d;%dH", 20, CANVAS_WIDTH * 2 + 25);
        printf("   ##     ## ##      ##     ##       ## ###   ###\n");
        printf("\033[%d;%dH", 21, CANVAS_WIDTH * 2 + 25);
        printf("    #     ##  ##      #     ##       # ####    #####\n");
    }
    else if (mode == 4) {
        printf("\033[%d;%dH", 15, CANVAS_WIDTH * 2 + 25);
        printf(" #####   ##  ##      ##    ####\n");
        printf("\033[%d;%dH", 16, CANVAS_WIDTH * 2 + 25);
        printf("###  ##  ##  ##    #####   #  ###\n");
        printf("\033[%d;%dH", 17, CANVAS_WIDTH * 2 + 25);
        printf("##   ##  ##   ##   ## ###  ##  ###\n");
        printf("\033[%d;%dH", 18, CANVAS_WIDTH * 2 + 25);
        printf("##   ##  ##   ##  ##   ##  ##  ###\n");
        printf("\033[%d;%dH", 19, CANVAS_WIDTH * 2 + 25);
        printf("## ####  ###  ##  #######  ##  ###\n");
        printf("\033[%d;%dH", 20, CANVAS_WIDTH * 2 + 25);
        printf("### ###  ###  ##  ##  ##   # ###\n");
        printf("\033[%d;%dH", 21, CANVAS_WIDTH * 2 + 25);
        printf(" ######   #####   #   #    #####\n");
    }
    mode = 0;
}

void printwin(void) {
    printf("░░░░░░░░░░█▀▀▀█░░░░░░░░▄█▀▀▀█░░░\n");
    printf("░░░░░░░░░░█░░░░█░░░░░░▄█░░░░█░░░\n");
    printf("░░░░░░░░░░█▄░░░▀█░░░░░█▀░░░██░░░\n");
    printf("░░░░░░░░░░░█░░░░█▄░░░█▀░░░▄█░░░░\n");
    printf("░░░░░░░░░░░▀█░░░▀█░░█▀░░░░█░░░░░\n");
    printf("░░░░░░░░░░░░█▄░░░██▄█░░░░██░░░░\n");
    printf("░░░░░░░░░▄▄░░█▄▄▄░░▀░░░░▄█░░░░░\n");
    printf("░░░░░░░▄█▀▀████▀▀▀▀▀▀█▄░██░░░░░\n");
    printf("░░░░░░░█▄░░░▀██▄▄░░░░░▀███░░░░░\n");
    printf("░░░░█▀▀▀█▄░░░▀█▀▀▀▀█░░░▀██░░░░░\n");
    printf("░░░░█▄░░░█▄░░░▀█░░░█░░░░▀█▄░░░░\n");
    printf("░░░░██▄░░░██▄░▄█░░░▀░░░░░██░░░░\n");
    printf("░░░░██▀█▄▄█▀▀▀▀░░░░░░░░░░█▀░░░░\n");
    printf("░░░░░█░░▀▀░░░░░░░░░░░░░▄█▀░░░░░\n");
    printf("░░░░░▀█▄░░░░░░░░░░░░░░▄█░░░░░░░\n");
    printf("░░░░░░░▀█▄░░░░░░░░░░░█▀░░░░░░░░\n");
}

void printlose(void) {
    printf("░░░░░░░░░░░░▄█▀▀██▀▀▀▀▀▀▀█▄░░░░░\n");
    printf("░░░░░░░░░░▄▀░░░░█▄░▄▄▄███▀▀█▄░░\n");
    printf("░░░▄▀▀▀▀▀▀░░░░░░▄█▀▀░░░░░░░░▄█░\n");
    printf("░░░█░░░░░░░░░░░░▀█▄▄▄█▀▀▀▀▀▀▀▀█\n");
    printf("░░░█░░░░░░░░░░░░░░█▀░░░░▄▄▄▄▄░▄█\n");
    printf("░░░██░░░░░░░░░░░░░▀█▄█▀▀▀░░░░▀█▄\n");
    printf("░░░░█░░░░░░░░░░░░░░▀█░░░░░░░░░░▀\n");
    printf("░░░░█▄░░░░░░░░░░░░░░▀▀▀▀▀▀▀▀▀░░▄\n");
    printf("░░░░░▀▀▀▀▀█▄░░░░░░░░░░░░░░░▄▄▄█▀\n");
    printf("░░░░░░░░░░░▀█▄░░░░░░░██▀▀▀▀▀░░░░\n");
    printf("░░░░░░░░░░░░░▀█▄░░░░░░█░░░░░░░░░\n");
    printf("░░░░░░░░░░░░░░░▀█▄░░░░▀█░░░░░░░░\n");
    printf("░░░░░░░░░░░░░░░░░▀▄░░░░▀█░░░░░░░\n");
    printf("░░░░░░░░░░░░░░░░░░█▄░░░░▀█░░░░░░\n");
    printf("░░░░░░░░░░░░░░░░░░░█░░░░░█░░░░░░\n");
    printf("░░░░░░░░░░░░░░░░░░░░▀▄▄▄▄▀░░░░░░\n");
}

void printCanvas(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], State* state, int floor)
{
    printf("\033[0;0H\n");
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            printf("\033[%dm\u3000", canvas[i][j].color);
        }
        printf("\033[0m|\n");
    }

    Shape shapeData = shapes[state->queue[1]];
    printf("\033[%d;%dHNext:", 3, CANVAS_WIDTH * 2 + 5);
    for (int i = 1; i <= 3; i++)
    {
        shapeData = shapes[state->queue[i]];
        for (int j = 0; j < 4; j++) {
            printf("\033[%d;%dH", i * 4 + j, CANVAS_WIDTH * 2 + 15);//離欄多遠
            for (int k = 0; k < 4; k++) {
                if (j < shapeData.size && k < shapeData.size && shapeData.rotates[0][j][k]) {
                    printf("\x1b[%dm  ", shapeData.color);
                }
                else {
                    printf("\x1b[0m  ");
                }
            }
        }
    }

    printf("\033[%d;%dHHold:", 3, CANVAS_WIDTH * 2 + 25);
    if (isHold) {
        for (int j = 0; j < 4; j++) {
            printf("\033[%d;%dH", 1 * 4 + j, CANVAS_WIDTH * 2 + 35);
            for (int k = 0; k < 4; k++) {
                if (j < Hold.size && k < Hold.size && Hold.rotates[0][j][k]) {
                    printf("\x1b[%dm  ", Hold.color);
                }
                else {
                    printf("\x1b[0m  ");
                }
            }
        }
    }

    printanimation(scoremode);

    printf("\033[%d;%dHFloorleft: %d", 18, CANVAS_WIDTH * 2 + 5, floor);
    printf("\033[%d;%dHScore: %d", 20, CANVAS_WIDTH * 2 + 5, (state->score));

    return;
}

int clearLine(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH]) {
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            if (canvas[i][j].current) {
                canvas[i][j].current = false;
            }
        }
    }

    int linesCleared = 0;

    for (int i = CANVAS_HEIGHT - 1; i >= 0; i--)
    {
        bool isFull = true;
        for (int j = 0; j < CANVAS_WIDTH; j++)
        {
            if (canvas[i][j].shape == EMPTY) {
                isFull = false;
                break;
            }
        }

        if (isFull) {
            linesCleared += 1;

            for (int j = i; j > 0; j--)
            {
                for (int k = 0; k < CANVAS_WIDTH; k++)
                {
                    setBlock(&canvas[j][k], canvas[j - 1][k].color, canvas[j - 1][k].shape, false);
                    resetBlock(&canvas[j - 1][k]);
                }
            }
            i++;//消掉後還原
        }
    }


    return linesCleared;
}

void logic(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], State* state, int* floor)
{
    if (ROTATE_FUNC()) {
        int newRotate = (state->rotate + 1) % 4;
        if (move(canvas, state->x, state->y, state->rotate, state->x, state->y, newRotate, state->queue[0]))
        {
            state->rotate = newRotate;
        }
    }
    else if (LEFT_FUNC()) {
        if (move(canvas, state->x, state->y, state->rotate, state->x - 1, state->y, state->rotate, state->queue[0]))
        {
            state->x -= 1;
        }
    }
    else if (RIGHT_FUNC()) {
        if (move(canvas, state->x, state->y, state->rotate, state->x + 1, state->y, state->rotate, state->queue[0]))
        {
            state->x += 1;
        }
    }
    else if (DOWN_FUNC()) {
        state->fallTime = FALL_DELAY;
    }
    else if (FALL_FUNC()) {
        state->fallTime += FALL_DELAY * CANVAS_HEIGHT;
    }
    else if (HOLD_FUNC() && !isHold) {
        isHold = true;

        cleanShape(canvas, state->x, state->y, state->rotate, state->queue[0]);//clean original shape

        if (Hold.size == 0) {
            Hold = shapes[state->queue[0]];
            state->queue[0] = state->queue[1];
            state->queue[1] = state->queue[2];
            state->queue[2] = state->queue[3];
            state->queue[3] = rand() % 7;
        }//hold have nothing
        else {
            ShapeId temp = state->queue[0];
            state->queue[0] = Hold.shape;
            Hold = shapes[temp];
        }//when hold exist
        state->rotate = 0;
    }

    state->fallTime += RENDER_DELAY;

    while (state->fallTime >= FALL_DELAY) {
        state->fallTime -= FALL_DELAY;

        if (move(canvas, state->x, state->y, state->rotate, state->x, state->y + 1, state->rotate, state->queue[0])) {
            state->y++;
        }
        else {
            state->score += 20;

            int score = clearLine(canvas);
            if (score == 0) {
                scoremode = 0;
            }
            else if (score == 1) {
                scoremode = 1;
                state->score += 100;
            }
            else if (score == 2) {
                scoremode = 2;
                state->score += 200;
            }
            else if (score == 3) {
                scoremode = 3;
                state->score += 300;
            }
            else if (score == 4) {
                scoremode = 4;
                state->score += 500;
            }

            *floor -= score;

            state->x = CANVAS_WIDTH / 2;
            state->y = 0;
            state->rotate = 0;
            state->fallTime = 0;
            state->queue[0] = state->queue[1];
            state->queue[1] = state->queue[2];
            state->queue[2] = state->queue[3];
            state->queue[3] = rand() % 7;

            if (!move(canvas, state->x, state->y, state->rotate, state->x, state->y, state->rotate, state->queue[0]))
            {
                programContinue = false;
                system("cls");
                printlose();
                Sleep(1000);
                return;
            }

            isHold = false;
        }
    }
    return;
}

int main()
{
    while (1) {
        int floor = 0;
        srand(time(NULL));
        State state = {
            .x = CANVAS_WIDTH / 2,
            .y = 0,
            .score = 0,
            .rotate = 0,
            .fallTime = 0
        };

        for (int i = 0; i < 4; i++)
        {
            state.queue[i] = rand() % 7;
        }

        Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
        for (int i = 0; i < CANVAS_HEIGHT; i++)
        {
            for (int j = 0; j < CANVAS_WIDTH; j++)
            {
                resetBlock(&canvas[i][j]);
            }
        }

        system("cls");
        printsplash();
        printf("\n請輸入您想要挑戰的層數 : ");
        printf("\n或輸0退出");        
        scanf_s("%d", &floor);
        if (floor == 0) return 0;


        system("cls");

        move(canvas, state.x, state.y, state.rotate, state.x, state.y, state.rotate, state.queue[0]);

        while (floor > 0 && programContinue)
        {
            logic(canvas, &state, &floor);
            printCanvas(canvas, &state, floor);
            Sleep(100);
        }
        
        if (programContinue) {
            system("cls");
            printwin();
            Sleep(1000);
        }

        Hold.size = 0;
        isHold = false;
        scoremode = 0;
        programContinue = true;
    }
}
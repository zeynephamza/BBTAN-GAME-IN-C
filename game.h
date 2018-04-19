#include "engine.h"

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#define BRICK_TYPE_EMPTY 10
#define BRICK_TYPE_SQUARE 11
#define BRICK_TYPE_TRIANGLE 12
#define BRICK_TYPE_ADD 13
#define BRICK_TYPE_ROW 14
#define BRICK_TYPE_COLUMN 15
#define BRICK_TYPE_RANDOMIZE 16

typedef struct brick_t{
    int length;
    int x;
    int y;
    int strength;
    int showing;
    double velocity;
    int type;
    int row;
    int column;

}brick_t;

typedef struct ball_t{
    volatile int x;
    volatile int y;
    int moving;
    double velocity;
    int velocity_x;
    int velocity_y;
    int wait;
    brick_t *brick;

}ball_t;

typedef struct pipe_t{
    int x1;
    int y1;
    int x2;
    int y2;
}pipe_t;

typedef struct cannon_t{
    int x;
    int y;
    int radius;
    int color;
    int activated;
}cannon_t;





#endif // GAME_H_INCLUDED

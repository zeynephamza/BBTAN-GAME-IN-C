#include <allegro.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include "game.h"

#define BRICK_ROW_COUNT 20
#define BRICK_COLUMN_COUNT 8
#define BALL_MAX_COUNT 10

const char *title="BBTAN";
const int title_x=SCREEN_WIDTH/2;
const int title_y=50;

const int brick_length=50;
int brick_color_strong;
int brick_color_average;
int brick_color_weak;
int brick_color_add;
int brick_color_row;
int brick_color_column;
int brick_color_randomize;

double brick_velocity=2.0;

brick_t bricks[BRICK_ROW_COUNT][BRICK_COLUMN_COUNT];

int bricks_falling=1;
int bricks_fall_count=0;

cannon_t cannon;

pipe_t pipe;

double ball_velocity=10;
int ball_count=1;
ball_t balls[BALL_MAX_COUNT];
int ball_count_moving=0;

int level_number=1;
int level_brick_capacity=5;

int game_over=0;

void game_start()
{
    engine_start();
    brick_color_strong= makecol(0xFF,0x00,0x00);
    brick_color_average= makecol(0xFF,0x00,0xFF);
    brick_color_weak= makecol(0xFF,0xFF,0x00);
    brick_color_add= makecol(0xFF,0xAA,0xFF);
    brick_color_row= makecol(0xAA,0xFF,0xFF);
    brick_color_column= makecol(0x44,0xDD,0x00);
    brick_color_randomize= makecol(0xFF,0xAA,0x00);
    show_mouse(screen);
}
void game_stop()
{
    engine_stop();
    show_mouse(NULL);
}
int find_brick_type(int random)
{
    if(random<18)
    {
        return BRICK_TYPE_SQUARE;
    }
    else if(random<35)
    {
        return BRICK_TYPE_TRIANGLE;
    }
    else if(random<43)
    {
        return BRICK_TYPE_ADD;
    }
    else if(random<45)
    {
        return BRICK_TYPE_ROW;
    }
    else if(random<48)
    {
        return BRICK_TYPE_COLUMN;
    }
    else if(random<50)
    {
        return BRICK_TYPE_RANDOMIZE;
    }
    else
    {
        return BRICK_TYPE_EMPTY;
    }
}
void bricks_init()
{
    int i,j;

    for(i=0; i<BRICK_ROW_COUNT; i++)
    {
        for(j=0; j<BRICK_COLUMN_COUNT; j++)
        {
            int random=(int)(rand()%100);
            int type=find_brick_type(random);
            int x=50+j*100;
            int y=100-brick_length*i;
            int strength=i+1;
            int showing=1;
            brick_init(&bricks[i][j],x,y,type,strength,showing,i,j);
        }
    }

}
void brick_init(brick_t *brick,int x,int y,int type,int strength,int showing,int row,int column)
{
    brick->x=x;
    brick->y=y;
    brick->type=type;
    brick->strength=strength;
    brick->showing=showing;
    brick->velocity=brick_velocity;
    brick->length=brick_length;
    brick->row=row;
    brick->column=column;

}
int find_brick_color(int strength)
{
    int color=foreground_color;
    if(strength>20)
    {
        color=brick_color_strong;
    }
    else if(strength>=10)
    {
        color=brick_color_average;
    }
    else
    {
        color=brick_color_weak;
    }
}
void cannon_init()
{
    cannon.x=SCREEN_WIDTH/2;
    cannon.y=SCREEN_HEIGHT-100;
    cannon.radius=30;
    cannon.color=makecol(0x00,0xFF,0xFF);
    cannon.activated=0;
}
void balls_init()
{
    int i;

    for(i=0; i<BALL_MAX_COUNT; i++)
    {
        balls[i].moving=0;
    }
}

void paint_title()
{
    paint_text(title,title_x,title_y,foreground_color);

    char level_text[3];
    itoa(level_number,level_text,10);
    paint_text(level_text,title_x+100,title_y,foreground_color);
    if(game_over)
    {
        paint_text("GAME OVER",title_x+200,title_y,foreground_color);
    }
}

void paint_brick_rectangle(brick_t *brick)
{
    int color=find_brick_color(brick->strength);
    paint_rectangle(brick->x,brick->y,brick->length,brick->length,color);
    char number_text[3];
    itoa(brick->strength,number_text,10);
    paint_text(number_text,brick->x,brick->y,color);
}
void paint_brick_triangle(brick_t *brick)
{
    int x1=brick->x-brick->length/2;
    int y1=brick->y-brick->length/2;
    int x2=brick->x+brick->length/2;
    int y2=brick->y-brick->length/2;
    int x3=brick->x-brick->length/2;
    int y3=brick->y+brick->length/2;
    int color=find_brick_color(brick->strength);
    paint_triangle(x1,y1,x2,y2,x3,y3,color);
    char number_text[3];
    itoa(brick->strength,number_text,10);
    int number_x=brick->x-brick->length/4;
    int number_y=brick->y-brick->length/4;
    paint_text(number_text,number_x,number_y,color);
}
void paint_brick_add(brick_t *brick)
{
    paint_circle(brick->x,brick->y,brick->length/2,brick_color_add);
    paint_text("+",brick->x,brick->y,brick_color_add);
}
void paint_brick_row(brick_t *brick)
{
    paint_circle(brick->x,brick->y,brick->length/2,brick_color_row);
    paint_text("--",brick->x,brick->y,brick_color_row);
}

void paint_brick_column(brick_t *brick)
{
    paint_circle(brick->x,brick->y,brick->length/2,brick_color_column);
    paint_text("|",brick->x,brick->y,brick_color_column);
}

void paint_brick_randomize(brick_t *brick)
{
    paint_circle(brick->x,brick->y,brick->length/2,brick_color_randomize);
    paint_text("X",brick->x,brick->y,brick_color_randomize);
}
void paint_brick(brick_t *brick)
{
    if(!brick->showing)
    {
        return;
    }

    switch(brick->type)
    {
    case BRICK_TYPE_SQUARE:
        paint_brick_rectangle(brick);
        break;
    case BRICK_TYPE_TRIANGLE:
        paint_brick_triangle(brick);
        break;
    case BRICK_TYPE_ADD:
        paint_brick_add(brick);
        break;
    case BRICK_TYPE_ROW:
        paint_brick_row(brick);
        break;
    case BRICK_TYPE_COLUMN:
        paint_brick_column(brick);
        break;
    case BRICK_TYPE_RANDOMIZE:
        paint_brick_randomize(brick);
        break;
    }
}


void paint_bricks()
{
    int i,j;

    for(i=0; i<level_number; i++)
    {
        for(j=0; j<BRICK_COLUMN_COUNT; j++)
        {
            paint_brick(&bricks[i][j]);
        }

    }

}

int once_hit(ball_t *ball,brick_t *brick)
{
    if(ball->brick==brick)
    {
        return 1;
    }
    else
    {
        ball->brick=brick;
        return 0;
    }
}
int hit_brick_square(ball_t *ball,brick_t *brick)
{

    int delta_x=abs(ball->x-brick->x);
    int delta_y=abs(ball->y-brick->y);

    if(delta_x>delta_y)
    {
        ball->velocity_x=-ball->velocity_x;
    }
    else
    {
        ball->velocity_y=-ball->velocity_y;
    }

    if(brick->strength>0)
    {
        brick->strength--;
    }

    if(brick->strength==0)
    {
        brick->showing=0;
    }
}
int hit_brick_triangle(ball_t *ball,brick_t *brick)
{

    int delta_x=ball->x-brick->x;
    int delta_y=ball->y-brick->y;
    int temp;

    temp=ball->velocity_x;
    ball->velocity_x=-ball->velocity_y;
    ball->velocity_y=-temp;

    if(brick->strength>0)
    {
        brick->strength--;
    }
    if(brick->strength==0)
    {
        brick->showing=0;
    }
}
int hit_brick_add(ball_t *ball,brick_t *brick)
{
    ball_count++;
    brick->showing=0;
}
int hit_brick_row(ball_t *ball,brick_t *brick)
{
    int j;
    for(j=0; j<BRICK_COLUMN_COUNT; j++)
    {
        brick_t *sibling=&bricks[brick->row][j];

        if(sibling->type==BRICK_TYPE_SQUARE)
        {
            hit_brick_square(ball,sibling);
        }
        if(sibling->type==BRICK_TYPE_TRIANGLE)
        {
            hit_brick_triangle(ball,sibling);
        }
    }
    brick->strength=0;
}
int hit_brick_column(ball_t *ball,brick_t *brick)
{
    int i;
    for(i=0; i<BRICK_ROW_COUNT; i++)
    {
        brick_t *sibling=&bricks[i][brick->column];
        if(sibling->type==BRICK_TYPE_SQUARE)
        {
            hit_brick_square(ball,sibling);
        }
        if(sibling->type==BRICK_TYPE_TRIANGLE)
        {
            hit_brick_triangle(ball,sibling);
        }
    }
    brick->strength=0;

}
int hit_brick_randomize(ball_t *ball,brick_t *brick)
{
    int i;

    for(i=0; i<ball_count; i++)
    {
        int delta_x=(int)((rand()*100)/RAND_MAX)-50;
        int delta_y=-(int)((rand()*100)/RAND_MAX);

        set_ball_velocity(&balls[i],delta_x,delta_y);
    }
}
int find_brick_region(int delta_x,int delta_y)
{
    if(delta_x>0)
        if(delta_y>0)
            return abs(delta_x)>abs(delta_y) ? 1 : 2;
        else
            return abs(delta_x)>abs(delta_y) ? 3 : 4;
    else
        if(delta_y>0)
            return abs(delta_x)>abs(delta_y) ? 5 : 6;
        else
            return abs(delta_x)>abs(delta_y) ? 7 : 8;

}
void hide_dead_bricks(){
    int i,j;
    int n=level_number;
    for(i=0; i<n; i++)
    {
        for(j=0; j<BRICK_COLUMN_COUNT; j++)
        {
            if((bricks[i][j].type==BRICK_TYPE_COLUMN
                    || bricks[i][j].type==BRICK_TYPE_ROW)
                    && bricks[i][j].strength==0)
            {
                    bricks[i][j].showing=0;
            }
        }
    }
}
void check_brick_hit(ball_t *ball,brick_t *brick)
{

    if(brick->showing==0)
    {
        return;
    }
    int approximity=0.5*brick->length;
    int delta_x=ball->x-brick->x;
    int delta_y=ball->y-brick->y;
    int region;

    if(abs(delta_x)<=approximity && abs(delta_y)<=approximity)
    {
        /*
        if(once_hit(ball,brick))
        {
            return ;
        }*/
        switch(brick->type)
        {
        case BRICK_TYPE_SQUARE:
            hit_brick_square(ball,brick);
            break;

        case BRICK_TYPE_TRIANGLE:
            region=find_brick_region(delta_x,delta_y);
            if(region==4 || region==5 || region==7 || region ==8){
                hit_brick_triangle(ball,brick);
            }

            break;
        case BRICK_TYPE_ADD:
            if(!once_hit(ball,brick))
            {
                   hit_brick_add(ball,brick);
            }
            break;
        case BRICK_TYPE_ROW:
            hit_brick_row(ball,brick);
            break;
        case BRICK_TYPE_COLUMN:
            hit_brick_column(ball,brick);
            break;
        case BRICK_TYPE_RANDOMIZE:
            hit_brick_randomize(ball,brick);
            break;
        }
    }
}

void check_brick_hits()
{
    int i,j,k;
    int n=level_number;
    for(k=0; k<ball_count; k++)
    {

        for(i=0; i<n; i++)
        {
            for(j=0; j<BRICK_COLUMN_COUNT; j++)
            {
                check_brick_hit( &balls[k],&bricks[i][j]);
            }
        }
    }
}
void increment_bricks()
{
    int i,j;

    if(bricks_falling==0)
    {
        return;
    }
    for(i=0; i<BRICK_ROW_COUNT; i++)
    {
        for(j=0; j<BRICK_COLUMN_COUNT; j++)
        {
            bricks[i][j].y+=bricks[i][j].velocity;
        }
    }
    bricks_fall_count++;

    int bricks_fall_max=brick_length/brick_velocity;
    if(bricks_fall_count>bricks_fall_max)
    {
        bricks_falling=0;
        bricks_fall_count=0;
    }
}

void paint_cannon()
{
    paint_circle_filled(cannon.x,cannon.y,cannon.radius,cannon.color);
}
void paint_pipe()
{
    line(buffer,pipe.x1,pipe.y1,pipe.x2,pipe.y2,foreground_color);
}

void increment_balls()
{
    int i;

    for(i=0; i<ball_count; i++)
    {
        if(!balls[i].moving)
        {
            continue;
        }
        if(balls[i].wait>0)
        {
            balls[i].wait--;
        }
        else
        {
            check_brick_hits();

            if(balls[i].x<=0)
            {
                balls[i].velocity_x=-balls[i].velocity_x;
            }
            if(balls[i].x>=SCREEN_WIDTH)
            {
                balls[i].velocity_x=-balls[i].velocity_x;
            }
            if(balls[i].y<=0)
            {
                balls[i].velocity_y=-balls[i].velocity_y;
            }
            if(balls[i].y>=SCREEN_HEIGHT)
            {
                balls[i].velocity_y=-balls[i].velocity_y;
            }

            balls[i].x+= balls[i].velocity_x;
            balls[i].y+= balls[i].velocity_y;
            if( balls[i].y>cannon.y)
            {
                balls[i].moving=0;
                ball_count_moving--;

                if(ball_count_moving==0)
                {
                    level_number++;
                    bricks_falling=1;
                    hide_dead_bricks();
                }
            }
        }
    }

}
void paint_balls()

{
    int i;

    for(i=0; i<ball_count; i++)
    {
        if(  balls[i].moving == 1)
        {
            paint_circle_filled(  balls[i].x,  balls[i].y,5,foreground_color);
        }
    }

}
END_OF_FUNCTION(paint_balls)

void timer_init()
{
    LOCK_VARIABLE(bricks);
    LOCK_FUNCTION(increment_bricks());
    install_int(increment_bricks, 100);


    //LOCK_VARIABLE(ball_count);
    LOCK_VARIABLE(balls);
    LOCK_FUNCTION(increment_balls());
    install_int(increment_balls, 100);

}

void locate_pipe()
{
    int delta_x=cannon.x-mouse_x;
    int delta_y=cannon.y-mouse_y;
    pipe.x1=mouse_x;
    pipe.y1=mouse_y;
    pipe.x2=mouse_x+2*delta_x;
    pipe.y2=mouse_y+2*delta_y;
}

void fill_ball(ball_t *ball,int wait)
{

    balls[0].moving=0;
    ball->wait=wait;

    int delta_x=cannon.x-mouse_x;
    int delta_y=cannon.y-mouse_y;

    set_ball_velocity(ball,delta_x,delta_y);
}
void set_ball_velocity(ball_t *ball,int delta_x,int delta_y)
{

    double angle=atan((double)delta_y/(double)delta_x);
    double sign_x=0;
    double sign_y=0;
    if(delta_x>0)
    {
        if(delta_y>0)
        {
            sign_x=1;
            sign_y=1;
        }
        else
        {
            sign_x=1;
            sign_y=1;
        }
    }
    else
    {
        if(delta_y>0)
        {
            sign_x=-1;
            sign_y=-1;
        }
        else
        {
            sign_x=-1;
            sign_y=-1;
        }
    }
    ball->velocity=ball_velocity;
    ball->velocity_x=sign_x* ball->velocity*cos(angle);
    ball->velocity_y=sign_y* ball->velocity*sin(angle);
}
void fill_balls()
{
    int i;

    for(i=0; i<ball_count; i++)
    {
        fill_ball(&balls[i],i*3);
    }
}
void fire_balls()
{
    int i;

    for(i=0; i<ball_count; i++)
    {
        balls[i].x=pipe.x2;
        balls[i].y=pipe.y2;
        balls[i].moving=1;

    }
    ball_count_moving=ball_count;
}
void check_game_over()
{
    if(level_number<level_brick_capacity)
    {
        return ;
    }
    int i,j;
    int n=level_number-level_brick_capacity+1;
    for(i=0; i<n; i++)
    {
        for(j=0; j<BRICK_COLUMN_COUNT; j++)
        {
            if((bricks[i][j].type==BRICK_TYPE_SQUARE
                    || bricks[i][j].type==BRICK_TYPE_TRIANGLE)
                    && bricks[i][j].showing)
            {
                game_over=1;
                return;
            }
        }
    }
}

int main()
{

    int seed = time(NULL);
    srand(seed);

    if(engine_init())
    {
        return 1;
    }

    timer_init();


    cannon_init();

    balls_init();

    bricks_init();

    do
    {

        game_start();

        rest(100);


        paint_background();

        if(mouse_x>0 && mouse_y>0 )
        {
            if(!cannon.activated && mouse_b & 1)
            {
                cannon.activated=1;

                locate_pipe();

                fill_balls();

                paint_pipe();
            }
            else
            {
                if(cannon.activated)
                {
                    fire_balls();
                    cannon.activated=0;
                }

            }

        }

        paint_title();

        paint_bricks();


        paint_cannon();

        paint_balls();

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        check_game_over();
        game_stop();
        vsync();
    }
    while (!keypressed());


    return 0;
}


END_OF_MAIN()

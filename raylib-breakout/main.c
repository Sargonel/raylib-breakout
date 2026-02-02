#include "raylib.h"
#define WIDTH 2000
#define HEIGHT 1000
#define MAXSPEED 1000
#define ROWS 200
#define COLS 5
#define dt GetFrameTime()



typedef struct brick{
    float x;
    float y;
    float w;
    float h;
    float gap;
    float grid_w;
    float grid_h;
}brick;

typedef struct bullet{
    float x;
    float y;
    float r;
    float speed;
    float vx;
    float vy;
    int health;
}bullet;

typedef struct player{
    float x;
    float y;
    float w;
    float h;
    float speed;
    int health;
}player;

void movement(player *p){

    if((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && p->x > 0){
        p->x -= p->speed * dt;
    }
    if((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && p->x < WIDTH - p->w){
        p->x += p->speed * dt;
    }

    Vector2 center = {p->x,p->y};
    Vector2 size = {p->w,p->h};
    
    DrawRectangleV(center,size,WHITE);
}
void ball_logic(bullet *b, player player1, player *p){

    Vector2 center = {b->x,b->y};
    Rectangle p1 = {player1.x,player1.y,player1.w,player1.h};

    bool touch = CheckCollisionCircleRec(center,b->r,p1);
    int touch_num = 0;
    
    if(touch){
        if(b->speed < MAXSPEED){
            touch_num++;
        }
        float hit_pos = (b->x - player1.x) / player1.w;
        b->vx = (hit_pos - 0.5) * 2.0f;
        b->vy = -1;
    }
    if(b->y - b->r < 0){
        if(b->speed < MAXSPEED){
            touch_num++;}
        b->vy = 1;
    }
    else if(b->x - b->r < 0){
        if(b->speed < MAXSPEED){
            touch_num++;}
        b->vx = 1;
    }
    else if(b->x + b->r > WIDTH){
        if(b->speed < MAXSPEED){
            touch_num++;}
        b->vx = -1;
    }
    else if(b->y - b->r > HEIGHT){
        b->health -= 1;
        b->speed = 100.0f;
        b->vx = 0.0f;
        b->vy = 1.0f;
        b->x = WIDTH/2.0f;
        b->y = HEIGHT/2.0f;

        p->x = WIDTH/2.0f - 50.0f;
        p->y = HEIGHT  - HEIGHT/6.0f;
    }
    for(int i=0;i<touch_num;i++){
        b->speed += 100;
    }
    b->y += b->vy * b->speed * dt;
    b->x += b->vx * b->speed * dt;
    
    DrawCircleV(center,b->r,WHITE);
}

void grid(brick *b, bullet *bu,int grid_touch[ROWS][COLS]){

    float total_w = (ROWS * b->w) + ((ROWS - 1) * b->gap);
    float total_h = (COLS * b->h) + ((COLS - 1) * b->gap);

    float start_x = (WIDTH / 2.0f) - (total_w / 2.0f);
    float start_y = (HEIGHT / 2.0f) - (total_h)*2;

    Vector2 center = {bu->x,bu->y};
    bool brick_hit = false;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {

            if(grid_touch[i][j] == 1){
                continue;
            }

            float posX = start_x + i * (b->w + b->gap);
            float posY = start_y + j * (b->h + b->gap);

            Rectangle wall = {posX,posY,b->w,b->h};
            bool touch = CheckCollisionCircleRec(center,bu->r,wall);

            if(!brick_hit&&touch){
                grid_touch[i][j] = 1;
                bu->vy *= -1;
                brick_hit = true;
            }
            DrawRectangleRec(wall,BLUE);
        }
    }
}
void reset_grid(int grid_touch[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid_touch[i][j] = 0;
        }
    }
}

void reset_logic(bullet *bu,player *p,brick *b,int grid_touch[ROWS][COLS]){
    if(bu->health == 0){
        bu->x = WIDTH/2.0f;
        bu->y = HEIGHT/2.0f;
        bu->r = 15.0f;
        bu->speed = 100.0f;
        bu->vx = 0.0f;
        bu->vy = 1.0f;
        bu->health = 3;

        p->x = WIDTH/2.0f - 50.0f;
        p->y = HEIGHT - HEIGHT/6.0f;
        p->w = 100.0f;
        p->h = 20.0f;
        p->speed = 1000.0f;

        reset_grid(grid_touch);
    }
}
int main(void)
{
    SetTargetFPS(60);
    InitWindow(WIDTH, HEIGHT, "raylib breakout game");

    int grid_touch[ROWS][COLS] = {0};

    bullet bullet1 = {
        .x = WIDTH/2.0f,
        .y = HEIGHT/2.0f,
        .r = 15.0f,
        .speed = 100.0f,
        .vx = 0.0f,
        .vy = 1.0f,
        .health = 3
    };
    player player1 = {
        .x = WIDTH/2.0f - 50.0f,
        .y = HEIGHT - HEIGHT/6.0f,
        .w = 100.0f,
        .h = 20.0f,
        .speed = 1000.0f,
    };
    brick brick1 = {
        .x = WIDTH/2,
        .y = HEIGHT/4,
        .w = 90.0f,
        .h = 40.0f,
        .gap = 10.0f,
        .grid_w = WIDTH,
        .grid_h = HEIGHT/4
    };
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
            grid(&brick1,&bullet1,grid_touch);
            movement(&player1);
            ball_logic(&bullet1,player1,&player1);
            reset_logic(&bullet1,&player1,&brick1,grid_touch);
            DrawFPS(0,0);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
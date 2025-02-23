#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include<time.h>
#include "raylib.h"

#define MAP_WIDTH  (16 * 10)
#define MAP_HEIGHT (9  * 10)
#define LINE_THICK 2
#define mod(x, y) ((x%y+y)%y)

typedef struct {
    bool    *src;
    bool   *dest;
    double timer;
    float  speed;
    char    zoom;
    bool playing;
} State;

void DrawMap(State *state);
void UpdateMap(State *state);
void HandleInput(State *state);

int main(int argc, char **argv)
{
    InitWindow(0, 0, "Game Of Life");
    ToggleFullscreen();

    int monitor = GetCurrentMonitor();
    SetTargetFPS(GetMonitorRefreshRate(monitor));

    State state = {0};
    state.speed = 1;

    srand(time(NULL));

    state.src = calloc(MAP_WIDTH*MAP_HEIGHT, sizeof(bool));
    state.dest = calloc(MAP_WIDTH*MAP_HEIGHT, sizeof(bool));

    for(int i = 0; i < MAP_HEIGHT*MAP_WIDTH; i++)
    {
        int r = rand() % 2;
        if (r == 0) state.src[i] = true;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        if (state.playing) state.timer += GetFrameTime();

        ClearBackground(GRAY);
        HandleInput(&state);
        DrawMap(&state);

        if (state.timer >= 1.0f/state.speed)
        {
            state.timer = 0.0f;

            UpdateMap(&state);

            bool *temp = state.dest;
            state.dest = state.src;
            state.src = temp;
        }

        EndDrawing();
    }

    return 0;
}

void UpdateMap(State *state)
{
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            int index = y*MAP_WIDTH+x;
            int neighbours = 0;

            for(int dx = -1; dx <= 1; dx++)
            {
                for(int dy = -1; dy <= 1; dy++)
                {
                    if(dx == 0 && dy == 0) continue;
                    int targetX = mod((x+dx), MAP_WIDTH);
                    int targetY = mod((y+dy), MAP_HEIGHT);
                    
                    int target = targetY*MAP_WIDTH+targetX;
                    neighbours += state->src[target];
                }
            }

            if(state->src[index])
            {
                if(neighbours == 2 || neighbours == 3) state->dest[index] = true;
                else state->dest[index] = false;   
            }
            else if(neighbours == 3) state->dest[index] = true;
            else state->dest[index] = false;
        }
    }
}

void DrawMap(State *state)
{
    double width = GetScreenWidth();
    double height = GetScreenHeight();

    Vector2 step = { width/MAP_WIDTH * state->zoom, height/MAP_HEIGHT * state->zoom };

    for(int x = 1; x < MAP_WIDTH; x++)
    {
        double xPos = step.x * x;
        DrawLineEx((Vector2){ xPos, 0 }, (Vector2){ xPos, height }, LINE_THICK, BLACK);
    }

    for(int y = 1; y < MAP_HEIGHT; y++)
    {
        double yPos = step.y * y;
        DrawLineEx((Vector2){ 0, yPos }, (Vector2){ width, yPos }, LINE_THICK, BLACK);
    }
    
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            int index = y*MAP_WIDTH+x;

            if (state->src[index])
            {
                Vector2 position = { step.x * x + LINE_THICK/2 * (x != 0), step.y * y + LINE_THICK/2 * (y != 0) };
                Vector2 size = step;

                if (x != 0 && x != MAP_WIDTH-1) size.x -= LINE_THICK;
                else size.x -= LINE_THICK/2;
                if (y != 0 && y != MAP_HEIGHT-1) size.y -= LINE_THICK;
                else size.y -= LINE_THICK/2;

                DrawRectangleV(position, size, YELLOW);
            }
        }
    }
}

void HandleInput(State *state)
{
    if(IsKeyPressed(KEY_SPACE)) 
    {
        state->playing = !state->playing;
        if(!state->playing) 
        { 
            state->timer = 0;
        }
    }

    if(IsKeyPressed(KEY_UP)) fmin(state->speed *= 2, 128);
    if(IsKeyPressed(KEY_DOWN)) fmax(1, state->speed /= 2);

    state->zoom += GetMouseWheelMove();
    state->zoom = fmax(state->zoom, 1);

    double width = GetScreenWidth();
    double height = GetScreenHeight();
    Vector2 MousePos = GetMousePosition();
    Vector2 step = {width/MAP_WIDTH * state->zoom, height/MAP_HEIGHT * state->zoom};
    int index = floor(MousePos.y / step.y)*MAP_WIDTH + floor(MousePos.x / step.x);
    if(!state->playing && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) state->src[index] = true;
    if(!state->playing && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) state->src[index] = false;
    if(!state->playing && IsKeyPressed('C')) memset(state->src, 0, MAP_HEIGHT*MAP_WIDTH);
}
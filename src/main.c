#include <stdio.h>

#include "raylib.h"
#include "helpers.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "c ecs");
    while (! WindowShouldClose())
    {
        // update state
        // TODO

        // drawing
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawRectangle(0, 0, 50, 50, PINK);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

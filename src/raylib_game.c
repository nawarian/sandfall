#include "raylib.h"
#include <string.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static const int screenWidth = 800 * 2;
static const int screenHeight = 480 * 2;
static const int framesPerSecond = 120;

void UpdateDrawFrame(void);

float scale = 24.0f * 2;
char grid[20][32] = { 0 };
void place_sand_at(Vector2 coords);

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Sandfall");
    SetWindowMonitor(0);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, framesPerSecond, 1);
#else
    SetTargetFPS(framesPerSecond);

    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();

    return 0;
}

void Update(void);
void Draw(void);
void DrawDebug(void);

float ticksPerSecond = 60.0f;
void UpdateDrawFrame(void)
{
    static float acc = 0.0f;
    acc += GetFrameTime();
    if (acc > 1.0f / ticksPerSecond) {
        acc = 0;
        Update();
    }

    BeginDrawing();
        ClearBackground(BLACK);
        Draw();
        DrawDebug();
    EndDrawing();
}

enum Direction {
    LEFT = 0,
    RIGHT
};

Vector2 cursor;
Color currentColor = GOLD;
void Update(void)
{
    cursor = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        place_sand_at(cursor);
    }

    char grid_cpy[20][32] = { 0 };
    memcpy(grid_cpy, grid, sizeof(grid));

    enum Direction dir;
    int slide_offset;
    for (int j = 0; j < 20; ++j) {
        for (int i = 0; i < 32; ++i) {
            // Found a grain
            if (grid[j][i] == 1) {
                if (j >= 19) {
                    continue;
                }

                // None below, keep falling
                if (grid[j+1][i] == 0) {
                    grid_cpy[j][i] = 0;
                    grid_cpy[j+1][i] = 1;
                } else { // There's one below, slide
                    dir = GetRandomValue(0, 1);
                    slide_offset = dir == LEFT ? -1 : 1;

                    if ((i + slide_offset) < 0 || (i + slide_offset) > 31) {
                        slide_offset = 0;
                    }

                    if (grid[j+1][i+slide_offset] == 0) {
                        grid_cpy[j][i] = 0;
                        grid_cpy[j+1][i+slide_offset] = 1;
                    }
                }
            }
        }
    }

    memcpy(grid, grid_cpy, sizeof(grid));
}

void Draw(void)
{
    for (int j = 0; j < 20; ++j) {
        for (int i = 0; i < 32; ++i) {
            DrawRectangleLines(i * scale, j * scale, 1 * scale, 1 * scale, ColorAlpha(WHITE, 0.2f));

            if (grid[j][i] == 1) {
                DrawRectangle(i * scale, j * scale, 1 * scale, 1 * scale, currentColor);
            }
        }
    }
}

void DrawDebug(void)
{
    DrawFPS(0, 0);
}

int clamp_int(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;

    return val;
}

void place_sand_at(Vector2 coords)
{
    int x = clamp_int((int) (coords.x / scale), 0, 31);
    int y = clamp_int((int) (coords.y / scale), 0, 19);

    grid[y][x] = 1;
}


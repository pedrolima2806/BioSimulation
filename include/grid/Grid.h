#pragma once
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

struct GridStyle {
    SDL_Color gridColor;
    SDL_Color gridLineColor;
    SDL_Color gridCursorColor;
    SDL_Color gridCursorGhost;
};

class Grid {
    private:
    float x, y, columns, rows, cellSize;

    SDL_FRect gridRect;
    SDL_FRect cursor;
    SDL_FRect cursorGhost;

    GridStyle style;

    SDL_FPoint cursorPoints[5];

    void updateCursorPoints();
    public:
    Grid (float x, float y, float columns, float rows, float cellSize, GridStyle style);

    void handleEvent (const SDL_Event &event);
    void render (SDL_Renderer* renderer) const;
};


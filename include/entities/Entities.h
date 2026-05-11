#pragma once

#include <vector>

#include "grid/Grid.h"

struct EntitiesStyle {
    SDL_Color level1;
    SDL_Color level2;
    SDL_Color level3;
    SDL_Color level4;
};

struct Entity {
    int column, row;
    int trophicLevel;
    SDL_FRect rect;
};

class Entities {
    private:
    std::vector<Entity> entities;
    std::vector<std::pair<size_t, size_t>> occupiedCells;

    EntitiesStyle style;

    double moveTimerMs = 0.0;
    double moveIntervalMs = 250.0;

    public:
    explicit Entities(const Grid *grid, EntitiesStyle style);

    void spawnEntity(const Grid *grid, int trophicLevel);

    void handleEvent();
    void update(double deltaMs, const Grid *grid);
    void render(SDL_Renderer *renderer) const;

    void moveRandomly(const Grid *grid);
};
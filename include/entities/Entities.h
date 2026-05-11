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
    int visionRange;
};

class Entities {
    private:
    std::vector<Entity> entities;
    std::vector<std::pair<int, int>> occupiedCells;

    EntitiesStyle style;

    double moveTimerMs = 0.0;
    double moveIntervalMs = 150.0;

    void rebuildOccupiedCells();
    void resolveCollisions();

    [[nodiscard]] static int getDistance(int columnA, int rowA, int columnB, int rowB);
    [[nodiscard]] static bool canSee (const Entity &observer, const Entity &target);
    [[nodiscard]] int findClosestEntityIndex(const Entity &observer, int targetTrophicLevel) const;


    public:

    explicit Entities(const Grid *grid, EntitiesStyle style);

    void spawnEntity(const Grid *grid, int trophicLevel, int visionRange);
    void spawnEntities(const Grid *grid, int trophicLevel, int quantity, int visionRange);

    void handleEvent();
    void update(double deltaMs, const Grid *grid);
    void render(SDL_Renderer *renderer) const;

    void moveLogic(const Grid *grid);
    void moveRandomly(const Grid *grid);
    void moveAway(const Grid *grid);
    void moveTowards(const Grid *grid);
};
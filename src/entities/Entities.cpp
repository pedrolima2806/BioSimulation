#include <iostream>
#include <random>

#include "entities/Entities.h"

#include <algorithm>

int randomInt (const int min, const int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

Entities::Entities(const Grid *grid, EntitiesStyle style) : style(style) {
} ;

void Entities::spawnEntity(const Grid *grid, int trophicLevel) {
    if (grid == nullptr) return;

    const int columns =static_cast<int>(grid->getColumns());
    const int rows =static_cast<int>(grid->getRows());
    const int totalCells = columns * rows;


    if (columns <= 0 || rows <= 0) return;

    if (static_cast<int>(occupiedCells.size()) >= totalCells) {
        std::cout << "no empty cell on the grid" << std::endl;
        return;
    }

    bool randomize = true;
    while (randomize) {
        int column = randomInt(0, columns - 1);
        int row = randomInt(0, rows - 1);
        std::pair<size_t, size_t> entityCoordinates {column, row};
        if (auto it = std::ranges::find(occupiedCells, entityCoordinates); it == occupiedCells.end()) {
            occupiedCells.push_back(entityCoordinates);

            SDL_FRect rect{
                grid->getGridX() + static_cast<float>(column) * grid->getCellSize(),
                grid->getGridY() + static_cast<float>(row) * grid->getCellSize(),
                grid->getCellSize(),
                grid->getCellSize()
            };

            Entity entity{
                column,
                row,
                trophicLevel,
                rect
                };

            entities.push_back(entity);
            std::cout << "Entity created." << "Column:" << column << std::endl << "Row:" << row << std::endl;

            randomize = false;
        };
    }
};

void Entities::handleEvent() {

};

void Entities::update(double deltaMs, const Grid *grid) {
    if (grid == nullptr) return;

    moveTimerMs += deltaMs;
    if (moveTimerMs >= moveIntervalMs) {
        moveRandomly(grid);
        moveTimerMs = 0.0;
    }

    for (auto &entity : entities) {
        entity.rect.x = grid->getGridX() + static_cast<float>(entity.column) * grid->getCellSize();
        entity.rect.y = grid->getGridY() + static_cast<float>(entity.row) * grid->getCellSize();
        entity.rect.w = grid->getCellSize();
        entity.rect.h = grid->getCellSize();
    }
};

void Entities::render(SDL_Renderer *renderer) const {
    for (auto &entity : entities) {
        switch (entity.trophicLevel) {
            case 1:
                SDL_SetRenderDrawColor(renderer, style.level1.r, style.level1.g, style.level1.b, style.level1.a);
                break;
            case 2:
                SDL_SetRenderDrawColor(renderer, style.level2.r, style.level2.g, style.level2.b, style.level2.a);
                break;
            case 3:
                SDL_SetRenderDrawColor(renderer, style.level3.r, style.level3.g, style.level3.b, style.level3.a);
                break;
            case 4:
                SDL_SetRenderDrawColor(renderer, style.level4.r, style.level4.g, style.level4.b, style.level4.a);
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                break;

        }
        SDL_RenderFillRect(renderer, &entity.rect);
    }
};

void Entities::moveRandomly(const Grid *grid) {
    if (grid == nullptr) return;

    const int columns = static_cast<int>(grid->getColumns());
    const int rows = static_cast<int>(grid->getRows());

    if (columns <= 0 || rows <= 0) {
        return;
    }

    for (auto &entity : entities) {
        const int direction = randomInt(0, 3);
        int nextColumn = entity.column;
        int nextRow = entity.row;
        switch (direction) {
            case 0:
                nextColumn += 1;
                break;
            case 1:
                nextRow += 1;
                break;
            case 2:
                nextColumn -= 1;
                break;
            case 3:
                nextRow -= 1;
                break;
            default:
                break;
        }

        //wall collision
        // if (nextColumn < 0 || nextColumn >= static_cast<int>(grid->getColumns())) return;
        // if (nextRow < 0 || nextRow >= static_cast<int>(grid->getRows())) return;

        //wall warp
        if (nextColumn < 0) nextColumn = static_cast<int>(grid->getColumns()) - 1;
        if (nextColumn >= static_cast<int>(grid->getColumns())) nextColumn = 0;
        if (nextRow < 0) nextRow = static_cast<int>(grid->getRows()) - 1;
        if (nextRow >= static_cast<int>(grid->getRows())) nextRow = 0;

        entity.column = nextColumn;
        entity.row = nextRow;
    }
};
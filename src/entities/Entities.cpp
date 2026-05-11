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

Entities::Entities(const Grid *grid, EntitiesStyle style) : style(style) {};

void Entities::spawnEntity(const Grid *grid, int trophicLevel, int visionRange) {
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
        std::pair<int, int> entityCoordinates {column, row};
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
                rect,
                visionRange
                };

            entities.push_back(entity);
            std::cout << "Entity created." << std::endl << "Column:" << column << std::endl << "Row:" << row << std::endl;

            randomize = false;
        };
    }
};

void Entities::spawnEntities(const Grid *grid, int trophicLevel, int quantity, int visionRange) {
    for (int i = 0; i <= quantity; i++) {
        spawnEntity(grid, trophicLevel, visionRange);
    }
};

void Entities::handleEvent() {

};

void Entities::update(double deltaMs, const Grid *grid) {
    if (grid == nullptr) return;

    moveTimerMs += deltaMs;
    if (moveTimerMs >= moveIntervalMs) {
        moveLogic(grid);
        resolveCollisions();
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

void Entities::moveLogic(const Grid *grid) {
    if (grid == nullptr) return;

    const int columns = static_cast<int>(grid->getColumns());
    const int rows = static_cast<int>(grid->getRows());

    if (columns <= 0 || rows <= 0) return;

    for (auto &entity : entities) {
        int preyLevel = entity.trophicLevel - 1;
        int predatorLevel = entity.trophicLevel + 1;

        int preyIndex = findClosestEntityIndex(entity, preyLevel);
        int predatorIndex = findClosestEntityIndex(entity, predatorLevel);

        if (predatorIndex != -1) {
            moveAway(grid);
        }

        if (preyIndex != -1) {
            moveTowards(grid);
        }

        else {
            moveRandomly(grid);
        }
    }
}

void Entities::moveRandomly(const Grid *grid) {
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

void Entities::moveAway(const Grid *grid) {
    
}

void Entities::moveTowards(const Grid *grid) {

}

void Entities::rebuildOccupiedCells() {
    occupiedCells.clear();
    for (const auto &entity : entities) {
        occupiedCells.emplace_back(entity.column, entity.row);
    }
}

void Entities::resolveCollisions() {
    bool removedEntity;
    do {
        removedEntity = false;
        for (int i = 0; i < entities.size(); i++) {
            for (int j = 0; j < entities.size(); j++) {
                bool sameCell = entities[i].column == entities[j].column && entities[i].row == entities[j].row;

                //skip check for this numbers
                if (!sameCell) continue;
                if (entities[i].trophicLevel == entities[j].trophicLevel) continue;

                //define the loser and erases it
                int loserIndex;
                if (entities[i].trophicLevel < entities[j].trophicLevel) {
                    loserIndex = i;
                }
                else {
                    loserIndex = j;
                };

                entities.erase(entities.begin() + static_cast<long>(loserIndex));
                rebuildOccupiedCells();
                removedEntity = true;
            }
        };
    }while (removedEntity);
};

int Entities::getDistance (int columnA, int rowA, int columnB, int rowB) {
    return std::abs(columnA - columnB) + std::abs(rowA - rowB);
};

bool Entities::canSee(const Entity &observer, const Entity &target) {
    int distance = getDistance(observer.column, observer.row, observer.column, observer.row);
    return distance <= observer.visionRange;
};

int Entities::findClosestEntityIndex(const Entity &observer, int targetTrophicLevel) const {
    int closestEntityIndex = -1;
    int closestDistance = std::numeric_limits<int>::max();

    for (int i = 0; i < entities.size(); i++) {
        const Entity &targetEntity = entities[i];

        if (&observer == &targetEntity) continue;
        if (targetEntity.trophicLevel != targetTrophicLevel) continue;
        if (!canSee(observer, targetEntity)) continue;

        int distance = getDistance(observer.column, observer.row, targetEntity.column, targetEntity.row);

        if (distance < closestDistance) {
            closestDistance = distance;
            closestEntityIndex = i;
        }
    }

    return closestEntityIndex;
}

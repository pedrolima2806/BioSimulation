#include "grid/Grid.h"

#include <cmath>

Grid::Grid(float x, float y, float columns, float rows, float cellSize, GridStyle style)
    : x(x), y(y), columns(columns), rows(rows), cellSize(cellSize),
    gridRect{x, y, columns * cellSize, rows * cellSize},
    cursor{gridRect.x + floorf(columns/2)*cellSize,gridRect.y + floorf(rows/2)*cellSize,cellSize, cellSize},
    cursorGhost{-100, -100,cellSize,cellSize},
    style(style),
    cursorPoints{}
    {};

void Grid::handleEvent (const SDL_Event &event) {
    SDL_FPoint point;
    switch (event.type) {
        case SDL_EVENT_MOUSE_MOTION:
            point = {event.motion.x, event.motion.y};
            if (SDL_PointInRectFloat(&point, &gridRect)) {
                cursorGhost.x = gridRect.x + floorf((point.x - gridRect.x)/cellSize)*cellSize;
                cursorGhost.y = gridRect.y + floorf((point.y - gridRect.y)/cellSize)*cellSize;
            } else {
                cursorGhost.x = -100;
                cursorGhost.y = -100;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            point = {event.button.x, event.button.y};
            if (SDL_PointInRectFloat(&point, &gridRect)) {
                cursor.x = gridRect.x + floorf((point.x - gridRect.x)/cellSize)*cellSize;
                cursor.y = gridRect.y + floorf((point.y - gridRect.y)/cellSize)*cellSize;
                updateCursorPoints();
            }
            break;
        default:
            break;
    }
};

void Grid::render (SDL_Renderer *renderer) const {
    //grid background
    SDL_SetRenderDrawColor(renderer, style.gridColor.r, style.gridColor.g, style.gridColor.b, style.gridColor.a);
    SDL_RenderFillRect(renderer, &gridRect);

    //lines
    SDL_SetRenderDrawColor(renderer, style.gridLineColor.r, style.gridLineColor.g, style.gridLineColor.b, style.gridLineColor.a);
    for (int column = 0; column <= static_cast<int>(columns); ++column) {
        const float lineX = gridRect.x + static_cast<float>(column) * cellSize;
        SDL_RenderLine(
            renderer,
            lineX,
            gridRect.y,
            lineX,
            gridRect.y + gridRect.h
        );
    }
    for (int row = 0; row <= static_cast<int>(rows); ++row) {
        const float lineY = gridRect.y + static_cast<float>(row) * cellSize;
        SDL_RenderLine(
            renderer,
            gridRect.x,
            lineY,
            gridRect.x + gridRect.w,
            lineY
        );
    }

    //ghost cursor and grid cursor
    SDL_SetRenderDrawColor(renderer, style.gridCursorGhost.r, style.gridCursorGhost.g, style.gridCursorGhost.b, style.gridCursorGhost.a);
    SDL_RenderFillRect(renderer, &cursorGhost);

    SDL_SetRenderDrawColor(renderer, style.gridCursorColor.r, style.gridCursorColor.g, style.gridCursorColor.b, style.gridCursorColor.a);
    SDL_RenderLines (renderer, cursorPoints, sizeof(cursorPoints)/sizeof(SDL_FPoint));
};

void Grid::updateCursorPoints () {
    cursorPoints[0] = {cursor.x, cursor.y};
    cursorPoints[1] = {cursor.x + cursor.w, cursor.y};
    cursorPoints[2] = {cursor.x + cursor.w, cursor.y + cursor.h};
    cursorPoints[3] = {cursor.x, cursor.y + cursor.h};
    cursorPoints[4] = {cursor.x , cursor.y};
}

float Grid::getColumns() const {
    return columns;
}

float Grid::getRows() const {
    return rows;
}

float Grid::getGridX() const {
    return gridRect.x;
}

float Grid::getGridY() const {
    return gridRect.y;
}

float Grid::getCellSize() const {
    return cellSize;
}

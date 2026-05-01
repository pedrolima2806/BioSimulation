#include <iostream>
#include <chrono>
#include <cmath>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

int main() {
    //Initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (!MIX_Init()) {
        std::cerr << "MIX_Init failed." << std::endl;
    }
    if (!TTF_Init()) {
        std::cerr << "TTF_Init failed." << std::endl;
    }

    //Window e renderer
    int windowWidth = 1536, windowHeight = 864;
    SDL_Window *window = SDL_CreateWindow("hellcat_mp", windowWidth, windowHeight, 0);
    if (!window) {
        std::cerr << "Failed to create window:" << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Failed to create renderer:" << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //Mixer device
    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer) {
        std::cerr << "Failed to open audio device:" << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //Font
    TTF_Font *font = TTF_OpenFont("../assets/fonts/Quantico/Quantico-Regular.ttf", 16);
    if (!font) {
        std::cerr << "Failed to open font:" << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //variables definition
    float gridColumns = 45, gridRows = 45, gridCellSize = 15, gridX = 100, gridY = 100;
    auto gridWidth = static_cast<float>(gridColumns * gridCellSize), gridHeight = static_cast<float>(gridRows * gridCellSize);

    //color definition
    constexpr SDL_Color backgroundColor = {0, 0, 0, 255}, gridColor = {50, 50, 50, 255}, gridLineColor =
            {100, 100, 100, 255}, gridCursorColor = {255, 0, 0, 255}, gridCursorGhost = {100, 100, 100, 100};

    SDL_FRect gridRect = {gridX, gridY, gridWidth, gridHeight};

    if (gridY + gridHeight >= static_cast<float>(windowHeight) || gridX + gridWidth >= static_cast<float>(windowWidth)) {
        std::cout << "Grid is rendering outside visible window" << std::endl;
    }

    //cursor and ghost
    SDL_FRect gridCursor = {
        .x = gridRect.x + floorf(gridColumns/2)*gridCellSize ,
        .y = gridRect.y + floorf(gridRows/2)*gridCellSize ,
        .w = gridCellSize,
        .h = gridCellSize},
    ghostCursor = {
        gridCursor.x,
        gridCursor.y,
        gridCellSize,
        gridCellSize
    };

    //cursor
    SDL_FPoint cursorPoints[] = {
        {gridCursor.x, gridCursor.y},
        {gridCursor.x + gridCursor.w, gridCursor.y},
        {gridCursor.x + gridCursor.w, gridCursor.y + gridCursor.h},
        {gridCursor.x, gridCursor.y + gridCursor.h},
        {gridCursor.x, gridCursor.y}
    };


    //Loop cycle
    auto previousTime = std::chrono::steady_clock::now();
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                }
            SDL_FPoint point;
            switch (event.type) {
                case SDL_EVENT_MOUSE_MOTION:
                    point = {event.motion.x, event.motion.y};
                    if (SDL_PointInRectFloat(&point, &gridRect)) {
                        ghostCursor.x = gridRect.x + floorf((point.x - gridRect.x)/gridCellSize)*gridCellSize;
                        ghostCursor.y = gridRect.y + floorf((point.y - gridRect.y)/gridCellSize)*gridCellSize;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    point = {event.button.x, event.button.y};
                    if (SDL_PointInRectFloat(&point, &gridRect)) {
                        gridCursor.x = gridRect.x + floorf((point.x - gridRect.x)/gridCellSize)*gridCellSize;
                        gridCursor.y = gridRect.y + floorf((point.y - gridRect.y)/gridCellSize)*gridCellSize;
                        cursorPoints[0] = {gridCursor.x, gridCursor.y};
                        cursorPoints[1] = {gridCursor.x + gridCursor.w, gridCursor.y};
                        cursorPoints[2] = {gridCursor.x + gridCursor.w, gridCursor.y + gridCursor.h};
                        cursorPoints[3] = {gridCursor.x, gridCursor.y + gridCursor.h};
                        cursorPoints[4] = {gridCursor.x , gridCursor.y};
                    }
                    break;
                default:
                    break;
            }
        }
        //outside handleEvent

        //deltaMs
        auto currentTime = std::chrono::steady_clock::now();
        double deltaMs = std::chrono::duration<double, std::milli>(currentTime - previousTime).count();
        previousTime = currentTime;

        //update


        //background
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderClear(renderer);

        //grid background
        SDL_SetRenderDrawColor(renderer, gridColor.r, gridColor.g, gridColor.b, gridColor.a);
        SDL_RenderFillRect(renderer, &gridRect);

        //lines
        SDL_SetRenderDrawColor(renderer, gridLineColor.r, gridLineColor.g, gridLineColor.b, gridLineColor.a);
        for (float x = gridRect.x; x < gridWidth + gridRect.x + static_cast<float>(gridCellSize); x += static_cast<float>(gridCellSize)) {
            SDL_RenderLine(renderer,x,gridRect.y,x,gridHeight + gridRect.y);
        }
        SDL_SetRenderDrawColor(renderer, gridLineColor.r, gridLineColor.g, gridLineColor.b, gridLineColor.a);
        for (float y = gridRect.y; y < gridHeight + gridRect.y + static_cast<float>(gridCellSize); y += static_cast<float>(gridCellSize)) {
            SDL_RenderLine(renderer,gridRect.x,y,gridWidth + gridRect.x,y);
        }

        //ghost cursor and grid cursor
        SDL_SetRenderDrawColor(renderer, gridCursorGhost.r, gridCursorGhost.g, gridCursorGhost.b, gridCursorGhost.a);
        SDL_RenderFillRect(renderer, &ghostCursor);

        SDL_SetRenderDrawColor(renderer, gridCursorColor.r, gridCursorColor.g, gridCursorColor.b, gridCursorColor.a);
        SDL_RenderLines (renderer, cursorPoints, sizeof(cursorPoints)/sizeof(SDL_FPoint));

        //rendering
        SDL_RenderPresent(renderer);
    }


    //Finalization
    MIX_DestroyMixer(mixer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    MIX_Quit();
    SDL_Quit();
    return 0;
}
#include <iostream>
#include <chrono>
#include <cmath>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "grid/Grid.h"
#include "entities/Entities.h"

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

    //color definition
    GridStyle gridStyle = {
        .gridColor = {50, 50, 50, 255},
        .gridLineColor ={100, 100, 100, 255},
        .gridCursorColor = {255, 0, 0, 255},
        .gridCursorGhost = {100, 100, 100, 100}
    };

    EntitiesStyle entitiesStyle = {
        .level1 = {0,255,0,255},
        .level2 = {255,100,0,255},
        .level3 = {255, 0, 100, 255},
        .level4 = {255, 0, 0, 255}
    };

    //grid initialization
    float gridX = 50, gridY = 50;
    float columns = 80, rows = 50;
    float cellSize = 15;
    Grid grid = {gridX, gridY, columns, rows, cellSize, gridStyle};

    //Entity
    Entities entities(&grid, entitiesStyle);
    entities.spawnEntity(&grid, 1);
    entities.spawnEntity(&grid, 2);
    entities.spawnEntity(&grid, 3);
    entities.spawnEntity(&grid, 4);


    //Loop cycle
    auto previousTime = std::chrono::steady_clock::now();
    bool running = true;
    SDL_Event event;
    while (running) {
        constexpr SDL_Color backgroundColor = {0, 0, 0, 255};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                }
            grid.handleEvent(event);
        }
        //outside handleEvent

        //deltaMs
        auto currentTime = std::chrono::steady_clock::now();
        double deltaMs = std::chrono::duration<double, std::milli>(currentTime - previousTime).count();
        previousTime = currentTime;

        //update
        entities.update(deltaMs, &grid);

        //background
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderClear(renderer);

        //render grid
        grid.render(renderer);

        //entities render
        entities.render(renderer);

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
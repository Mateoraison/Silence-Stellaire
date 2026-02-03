#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // Initialisation SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Erreur SDL_Init: %s", SDL_GetError());
        return 1;
    }

    // Création fenêtre
    SDL_Window *window = SDL_CreateWindow(
        "SDL3 Image PNG",
        800,
        600,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        SDL_Log("Erreur SDL_CreateWindow: %s", SDL_GetError());
        return 1;
    }

    // Création renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Erreur SDL_CreateRenderer: %s", SDL_GetError());
        return 1;
    }

    // Chargement image PNG
    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/menu.png");
    if (!texture) {
        SDL_Log("Erreur IMG_LoadTexture: %s", SDL_GetError());
        return 1;
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "headers/main.h"

int afficher_map(SDL_Renderer *renderer) {
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police %s, %s", "assets/police.ttf", SDL_GetError());
        TTF_Quit();
        return 1;
    }
    
    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/carte_espace/fond_map.png");
    if (!texture) {
        SDL_Log("erreur chargement texture %s", SDL_GetError());
        TTF_CloseFont(font);
        TTF_Quit();
        return 1;
    }
    
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    
    int running = 1;
    while (running){
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
        }
    }
    
    
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    return 0;
}

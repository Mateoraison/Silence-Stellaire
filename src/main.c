#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "headers/main.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *fenetre = SDL_CreateWindow("Silence Stellaire", 800, 600, SDL_WINDOW_BORDERLESS);
    if (!fenetre) {
        SDL_Log("erreur creation fenetre: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(fenetre, NULL);
    if (!renderer) {
        SDL_Log("erreur creation renderer: %s", SDL_GetError());
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        int action_menu = afficher_menu(fenetre, renderer);
        if (action_menu == 1) {
            int resultat_jeu = jeu_principal(fenetre, renderer);
            if (resultat_jeu == 0) {
                running = false;
            } else if (resultat_jeu == 1) {
            }
        } else {
            running = false;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    return 0;
}


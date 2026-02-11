#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include "headers/main.h"





int afficher_menu(SDL_Window *fenetre, SDL_Renderer *renderer) {
    /*running = 0 pour le menu 
      1 pour le jeu  


    il faut juste remplacer les image pour les boutons !

    */
    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/menu.png");
    if(!texture) {
        SDL_Log("erreur chargement texture: %s", SDL_GetError());
        return 1;
    }


    SDL_Texture *newGameBoutonText = IMG_LoadTexture(renderer, "assets/bouton_game.png");
    if(!newGameBoutonText) {
        SDL_Log("erreur chargement texture bouton: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        return 1;
    }

    SDL_Texture *quitButtonText = IMG_LoadTexture(renderer, "assets/bouton_quit.png");
    if(!quitButtonText) {
        SDL_Log("erreur chargement texture bouton quitter: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(newGameBoutonText);
        return 1;
    }

    SDL_Texture * optionsButtonText = IMG_LoadTexture(renderer, "assets/bouton_option.png");
    if(!optionsButtonText) {
        SDL_Log("erreur chargement texture bouton options: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(newGameBoutonText);
        SDL_DestroyTexture(quitButtonText);
        return 1;
    }

    Bouton bouton_game;
    Bouton_Init(&bouton_game, 300.0f, 250.0f, 200.0f, 80.0f, newGameBoutonText);

    Bouton bouton_quitter;
    Bouton_Init(&bouton_quitter, 300.0f, 350.0f, 200.0f, 80.0f, quitButtonText);

    Bouton bouton_options;
    Bouton_Init(&bouton_options, 300.0f, 450.0f, 200.0f, 80.0f, optionsButtonText);

    int running = 1;
    int action = 0;

    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                action = 0;
                running = 0;
            } 
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    action = 0; // quitter
                    running = 0;
                }
            }
            if (Bouton_GererEvenement(&bouton_game, &event)) {
                action = 1; // commencer le jeu
                running = 0;
            }
            if (Bouton_GererEvenement(&bouton_quitter, &event)) {
                action = 0; // quitter
                running = 0;
            }
            if (Bouton_GererEvenement(&bouton_options, &event)) {
                SDL_Log("Options bouton clique (pas encore implementer)");
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        Bouton_Afficher(&bouton_game, renderer);
        Bouton_Afficher(&bouton_quitter, renderer);
        Bouton_Afficher(&bouton_options, renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(newGameBoutonText);
    SDL_DestroyTexture(quitButtonText);
    return action;
}

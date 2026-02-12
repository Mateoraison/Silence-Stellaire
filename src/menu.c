#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "headers/main.h"
#include <string.h>





int afficher_menu(SDL_Renderer *renderer) {
    /*action = 
        0 pour le menu 
        1 pour le jeu  
        2 pour les options

    */


    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        TTF_Quit();
        return 1;
    }

    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/menu.png");
    if(!texture) {
        SDL_Log("erreur chargement texture: %s", SDL_GetError());
        return 1;
    }


    SDL_Texture *BoutonText = IMG_LoadTexture(renderer, "assets/bouton.png");
    if(!BoutonText) {
        SDL_Log("erreur chargement texture bouton: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);
        TTF_Quit();
        return 1;
    }


    Bouton bouton_game;
    Bouton_Init(&bouton_game, 400.0f, 250.0f, 200.0f, 80.0f, BoutonText);

    Bouton bouton_quitter;
    Bouton_Init(&bouton_quitter, 400.0f, 350.0f, 200.0f, 80.0f, BoutonText);

    Bouton bouton_options;
    Bouton_Init(&bouton_options, 400.0f, 450.0f, 200.0f, 80.0f, BoutonText);

    SDL_Color blanc = {255, 255, 255, 255};

    SDL_Surface *surface_game = TTF_RenderText_Solid(font, "Game",strlen("Game"), blanc); 
    SDL_Texture *texte_game = SDL_CreateTextureFromSurface(renderer, surface_game); 
    SDL_DestroySurface(surface_game); 

    SDL_Surface *surface_options = TTF_RenderText_Solid(font, "Options",strlen("Options"), blanc); 
    SDL_Texture *texte_options = SDL_CreateTextureFromSurface(renderer, surface_options);
    SDL_DestroySurface(surface_options);

    SDL_Surface *surface_exit = TTF_RenderText_Solid(font, "Exit",strlen("Exit"), blanc); 
    SDL_Texture *texte_exit = SDL_CreateTextureFromSurface(renderer, surface_exit); 
    SDL_DestroySurface(surface_exit);

    
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
                action = 2;// option
                running = 0;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        
        Bouton_Afficher(&bouton_game, renderer);
        afficher_texte_centre(renderer, texte_game, &bouton_game.rect);
        
        Bouton_Afficher(&bouton_quitter, renderer);
        afficher_texte_centre(renderer, texte_exit, &bouton_quitter.rect);
        
        Bouton_Afficher(&bouton_options, renderer);
        afficher_texte_centre(renderer, texte_options, &bouton_options.rect);
        
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texte_game);
    SDL_DestroyTexture(texte_options);
    SDL_DestroyTexture(texte_exit);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(BoutonText);
    TTF_CloseFont(font);
    return action;
}

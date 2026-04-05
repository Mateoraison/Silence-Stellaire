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
        return 1;
    }

    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/menu/menu.png");
    if(!texture) {
        SDL_Log("erreur chargement texture: %s", SDL_GetError());
        return 1;
    }


    SDL_Texture *BoutonText = IMG_LoadTexture(renderer, "assets/menu/bouton.png");
    if(!BoutonText) {
        SDL_Log("erreur chargement texture bouton: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);
        return 1;
    }


    float menu_x = screen_center_x() - 125.0f;

    Bouton bouton_game;
    Bouton_Init(&bouton_game, menu_x, 250.0f, 250.0f, 80.0f, BoutonText);

    Bouton bouton_continuer;
    Bouton_Init(&bouton_continuer, menu_x, 350.0f, 250.0f, 80.0f, BoutonText);

    Bouton bouton_quitter;
    Bouton_Init(&bouton_quitter, menu_x, 450.0f, 250.0f, 80.0f, BoutonText);

    Bouton bouton_options;
    Bouton_Init(&bouton_options, menu_x, 550.0f, 250.0f, 80.0f, BoutonText);

    SDL_Color blanc = {255, 255, 255, 255};

    SDL_Surface *surface_game = TTF_RenderText_Solid(font, "Nouvelle Partie",strlen("Nouvelle Partie"), blanc); 
    SDL_Texture *texte_game = SDL_CreateTextureFromSurface(renderer, surface_game); 
    SDL_DestroySurface(surface_game); 

    SDL_Surface *surface_continuer = TTF_RenderText_Solid(font, "Continuer",strlen("Continuer"), blanc); 
    SDL_Texture *texte_continuer = SDL_CreateTextureFromSurface(renderer, surface_continuer); 
    SDL_DestroySurface(surface_continuer); 

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
            if (event.type == SDL_EVENT_MOUSE_MOTION ||
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                SDL_ConvertEventToRenderCoordinates(renderer, &event);
            }

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
                jouer_son("assets/audio/click.mp3", 0.5f);
                action = 1; // commencer le jeu
                running = 0;  
            }
            if (Bouton_GererEvenement(&bouton_quitter, &event)) {
                jouer_son("assets/audio/click.mp3", 0.5f);
                action = 0; // quitter
                running = 0;
            }
            if (Bouton_GererEvenement(&bouton_options, &event)) {
                jouer_son("assets/audio/click.mp3", 0.5f);
                action = 2;// option
                running = 0;
            }
            if(Bouton_GererEvenement(&bouton_continuer,&event)) {
                jouer_son("assets/audio/click.mp3", 0.5f);
                int slot = sauvegarde_choisir_slot(renderer, "Charger une partie", true);
                if (slot > 0) {
                    int planete_chargee = Planete_actuelle;
                    if (charger_partie_slot(slot, &planete_chargee) == 0) {
                        Planete_actuelle = planete_chargee;
                        action = 3; // continuer
                        running = 0;
                    }
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        
        Bouton_Afficher(&bouton_game, renderer);
        afficher_texte_centre(renderer, texte_game, &bouton_game.rect);

        Bouton_Afficher(&bouton_continuer, renderer);
        afficher_texte_centre(renderer, texte_continuer,&bouton_continuer.rect);
        
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

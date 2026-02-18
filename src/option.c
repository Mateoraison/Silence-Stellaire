#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "headers/main.h"
#include <string.h>




int afficher_option(SDL_Renderer *renderer, MIX_Track *track_global) {
    bool running = true;
    SDL_Event event;
    bool son_active = true;

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color gris_fonce = {50, 50, 50, 255};

    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        return -1;
    }

    SDL_Texture *bouton_texture = IMG_LoadTexture(renderer, "assets/bouton.png");
    if (!bouton_texture) {
        SDL_Log("Erreur texture bouton: %s", SDL_GetError());
        SDL_Delay(20000);
        TTF_CloseFont(font);
        return 1;
    }
    
    
    Bouton bouton_son;
    Bouton_Init(&bouton_son, 350, 500, 300, 60, bouton_texture);
    
    while (running) {
        
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        SDL_Rect fenetre_rect;
        SDL_GetRenderViewport(renderer, &fenetre_rect);
        float w = fenetre_rect.w;
        float h = fenetre_rect.h;

        
        SDL_Surface *surface_titre = TTF_RenderText_Solid(font, "Commandes", strlen("Commandes"), blanc);
        SDL_Texture *texture_titre = SDL_CreateTextureFromSurface(renderer, surface_titre);
        SDL_FRect rect_titre = {
            .x = w/2 - surface_titre->w/2,
            .y = 50,
            .w = surface_titre->w,
            .h = surface_titre->h
        };

        SDL_RenderTexture(renderer, texture_titre, NULL, &rect_titre);
        SDL_DestroyTexture(texture_titre);
        SDL_DestroySurface(surface_titre);

        // Liste des commandes
        const char *commandes[] = {
            "Echap - Retour au menu principal",
            "Souris - Cliquer sur boutons",
            "ZQSD - Déplacement du personnage",
            NULL
        };
        float y_start = 200;
        float ligne_hauteur = 40;
        for (int i = 0; commandes[i] != NULL; i++) {
            SDL_FRect fond_ligne = {
                .x = 100,
                .y = y_start + i * ligne_hauteur,
                .w = w - 200,
                .h = ligne_hauteur - 5
            };
            SDL_SetRenderDrawColor(renderer, gris_fonce.r, gris_fonce.g, gris_fonce.b, 200);
            SDL_RenderFillRect(renderer, &fond_ligne);

            SDL_Surface *surface_cmd = TTF_RenderText_Solid(font, commandes[i], strlen(commandes[i]), blanc);
            SDL_Texture *texture_cmd = SDL_CreateTextureFromSurface(renderer, surface_cmd);
            SDL_FRect rect_cmd = {
                .x = 120,
                .y = y_start + i * ligne_hauteur + 5,
                .w = surface_cmd->w,
                .h = surface_cmd->h
            };
            SDL_RenderTexture(renderer, texture_cmd, NULL, &rect_cmd);
            SDL_DestroyTexture(texture_cmd);
            SDL_DestroySurface(surface_cmd);
        }

        Bouton_Afficher(&bouton_son, renderer);
        
        const char* texte_son;
        if(son_active) {
            texte_son = "Son: Activé";
        } else {
            texte_son = "Son: Désactivé";
        }
        SDL_Surface *surface_son = TTF_RenderText_Solid(font, texte_son, strlen(texte_son), blanc);
        SDL_Texture *texture_son = SDL_CreateTextureFromSurface(renderer, surface_son);
        afficher_texte_centre(renderer, texture_son, &bouton_son.rect);
        SDL_DestroyTexture(texture_son);
        SDL_DestroySurface(surface_son);

        
        SDL_Surface *surface_quit = TTF_RenderText_Solid(font, "Appuyez sur Echap pour revenir au menu", strlen("Appuyez sur Echap pour revenir au menu"), blanc);
        SDL_Texture *texture_quit = SDL_CreateTextureFromSurface(renderer, surface_quit);
        SDL_FRect rect_quit = {
            .x = w/2 - surface_quit->w/2,
            .y = h - 100,
            .w = surface_quit->w,
            .h = surface_quit->h
        };
        SDL_RenderTexture(renderer, texture_quit, NULL, &rect_quit);
        SDL_DestroyTexture(texture_quit);
        SDL_DestroySurface(surface_quit);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (Bouton_GererEvenement(&bouton_son, &event)) {
                son_active = !son_active;
                if (track_global != NULL) { 
                    if (son_active) {
                        reprendre_son(track_global);
                    } else {
                        pause_son(track_global);
                    }
                }
            }


            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                TTF_CloseFont(font);
                SDL_DestroyTexture(bouton_texture);
                return 0;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
                TTF_CloseFont(font);
                SDL_DestroyTexture(bouton_texture);
                return 1;
            }
        }
    }
    
    TTF_CloseFont(font);
    SDL_DestroyTexture(bouton_texture);
    return 1;
}

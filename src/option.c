#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "headers/main.h"
#include <string.h>


int afficher_option(SDL_Renderer *renderer) {
    bool running = true;
    SDL_Event event;

    SDL_Color noir = {0, 0, 0, 255};
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color gris_fonce = {50, 50, 50, 255};

    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        return -1;
    }


    while (running) {
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        SDL_Rect fenetre_rect; ;
        SDL_GetRenderViewport(renderer, &fenetre_rect);
        float w = fenetre_rect.w;
        float h = fenetre_rect.h;

        
        SDL_Surface *surface_titre = TTF_RenderText_Solid(font, "Commandes",strlen("Commandes"), blanc);
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
        //liste des commandes
        const char *commandes[] = {
            "Echap - Retour au menu principal",
            "Souris - Cliquer sur boutons",
            "ZQSD - Déplacement du personnage"
        };

        float y_start = 200;
        float ligne_hauteur = 40;
        /*chaque tout de boucle je creer un carrer et j'affiche le message dans le carre*/
        for (int i = 0; commandes[i][0]; i++) {
            SDL_FRect fond_ligne = {
                .x = 100,
                .y = y_start + i * ligne_hauteur,
                .w = w - 200,
                .h = ligne_hauteur - 5
            };
            SDL_SetRenderDrawColor(renderer, gris_fonce.r, gris_fonce.g, gris_fonce.b, 200);
            SDL_RenderFillRect(renderer, &fond_ligne);

            SDL_Surface *surface_cmd = TTF_RenderText_Solid(font, commandes[i],strlen(commandes[i]), blanc);
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


       
        SDL_Surface *surface_quit = TTF_RenderText_Solid(font, "Appuyez sur Echap pour revenir au menu",strlen("Appuyez sur Echap pour revenir au menu"), blanc);
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
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                TTF_CloseFont(font);
                return 0;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
                TTF_CloseFont(font);
                return 1;
            }
        }
    }
    TTF_CloseFont(font);
    return 1;
}

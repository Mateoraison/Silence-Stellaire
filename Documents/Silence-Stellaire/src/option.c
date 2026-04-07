#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "headers/main.h"
#include <string.h>




int afficher_option(SDL_Renderer *renderer, MIX_Track *track_global) {
    bool running = true;
    SDL_Event event;
    bool ambiance_active = son_ambiance_est_active() != 0;
    bool sfx_active = son_sfx_est_actif() != 0;

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color gris_fonce = {50, 50, 50, 255};

    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        return -1;
    }

    SDL_Texture *bouton_texture = IMG_LoadTexture(renderer, "assets/menu/bouton.png");
    if (!bouton_texture) {
        SDL_Log("Erreur texture bouton: %s", SDL_GetError());
        SDL_Delay(20000);
        TTF_CloseFont(font);
        return 1;
    }
    
    
    Bouton bouton_ambiance;
    Bouton bouton_sfx;
    Bouton_Init(&bouton_ambiance, 0.0f, 0.0f, 280.0f, 60.0f, bouton_texture);
    Bouton_Init(&bouton_sfx, 0.0f, 0.0f, 280.0f, 60.0f, bouton_texture);
    
    while (running) {
        
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        SDL_Rect fenetre_rect;
        SDL_GetRenderViewport(renderer, &fenetre_rect);
        float w = fenetre_rect.w;
        float h = fenetre_rect.h;

        float panel_w = (w > 900.0f) ? 800.0f : (w - 100.0f);
        if (panel_w < 320.0f) panel_w = 320.0f;
        float panel_x = (w - panel_w) * 0.5f;
        float text_x = panel_x + 20.0f;

        float gap_boutons = 20.0f;
        float largeur_total = bouton_ambiance.rect.w + bouton_sfx.rect.w + gap_boutons;
        float debut_x = (w - largeur_total) * 0.5f;
        
        bouton_ambiance.rect.x = debut_x;
        bouton_ambiance.rect.y = h - 180.0f;
        
        bouton_sfx.rect.x = debut_x + bouton_ambiance.rect.w + gap_boutons;
        bouton_sfx.rect.y = h - 180.0f;


        
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
            "M - Ouvrir le menu des cartes",
            "E - Rammasser un objet",
            "I - Ouvrir l'inventaire",
            "1-5 - Utiliser un objet de la hotbar",
            "CRTL + numéro de l'outil (1-5) - Jeter un item ",
            NULL
        };
        float y_start = 200;
        float ligne_hauteur = 40;
        for (int i = 0; commandes[i] != NULL; i++) {
            SDL_FRect fond_ligne = {
                .x = panel_x,
                .y = y_start + i * ligne_hauteur,
                .w = panel_w,
                .h = ligne_hauteur - 5
            };
            SDL_SetRenderDrawColor(renderer, gris_fonce.r, gris_fonce.g, gris_fonce.b, 200);
            SDL_RenderFillRect(renderer, &fond_ligne);

            SDL_Surface *surface_cmd = TTF_RenderText_Solid(font, commandes[i], strlen(commandes[i]), blanc);
            SDL_Texture *texture_cmd = SDL_CreateTextureFromSurface(renderer, surface_cmd);
            SDL_FRect rect_cmd = {
                .x = text_x,
                .y = y_start + i * ligne_hauteur + 5,
                .w = surface_cmd->w,
                .h = surface_cmd->h
            };
            SDL_RenderTexture(renderer, texture_cmd, NULL, &rect_cmd);
            SDL_DestroyTexture(texture_cmd);
            SDL_DestroySurface(surface_cmd);
        }

        Bouton_Afficher(&bouton_ambiance, renderer);
        Bouton_Afficher(&bouton_sfx, renderer);
        
        const char* texte_ambiance;
        if(ambiance_active) {
            texte_ambiance = "Musique: Activé";
        } else {
            texte_ambiance = "Musique: Désactivé";
        }
        SDL_Surface *surface_ambiance = TTF_RenderText_Solid(font, texte_ambiance, strlen(texte_ambiance), blanc);
        SDL_Texture *texture_ambiance = SDL_CreateTextureFromSurface(renderer, surface_ambiance);
        afficher_texte_centre(renderer, texture_ambiance, &bouton_ambiance.rect);
        SDL_DestroyTexture(texture_ambiance);
        SDL_DestroySurface(surface_ambiance);
        
        const char* texte_sfx;
        if(sfx_active) {
            texte_sfx = "SFX: Activé";
        } else {
            texte_sfx = "SFX: Désactivé";
        }
        SDL_Surface *surface_sfx = TTF_RenderText_Solid(font, texte_sfx, strlen(texte_sfx), blanc);
        SDL_Texture *texture_sfx = SDL_CreateTextureFromSurface(renderer, surface_sfx);
        afficher_texte_centre(renderer, texture_sfx, &bouton_sfx.rect);
        SDL_DestroyTexture(texture_sfx);
        SDL_DestroySurface(surface_sfx);

        
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
            if (event.type == SDL_EVENT_MOUSE_MOTION ||
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                SDL_ConvertEventToRenderCoordinates(renderer, &event);
            }

            if (Bouton_GererEvenement(&bouton_ambiance, &event)) {
                ambiance_active = !ambiance_active;
                son_definir_ambiance_active(ambiance_active ? 1 : 0);
            }
            
            if (Bouton_GererEvenement(&bouton_sfx, &event)) {
                sfx_active = !sfx_active;
                son_definir_sfx_active(sfx_active ? 1 : 0);
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

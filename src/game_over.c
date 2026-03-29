#include "headers/main.h"

int game_over(SDL_Renderer *renderer){

    int result = 1;

    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        return result;
    }

    TTF_Font *font_game_over = TTF_OpenFont("assets/BiggerBook.ttf", 24);
    if (!font_game_over) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        TTF_CloseFont(font);
        return result;
    }

    SDL_Texture *BoutonText = IMG_LoadTexture(renderer, "assets/menu/bouton.png");
    if(!BoutonText) {
        SDL_Log("erreur chargement texture bouton: %s", SDL_GetError());
        TTF_CloseFont(font);
        TTF_CloseFont(font_game_over);
        return result;
    }

    Bouton bouton_game;
    Bouton_Init(&bouton_game, screen_center_x() - 100.0f, screen_center_y() - 50.0f, 200.0f, 80.0f, BoutonText);

    Bouton bouton_quitter;
    Bouton_Init(&bouton_quitter, screen_center_x() - 100.0f, screen_center_y() + 50.0f, 200.0f, 80.0f, BoutonText);

    SDL_Color blanc = {255, 255, 255, 255};

    SDL_Surface *surface_game = TTF_RenderText_Solid(font, "Recommencer",strlen("Recommencer"), blanc); 
    SDL_Texture *texte_game = SDL_CreateTextureFromSurface(renderer, surface_game); 
    SDL_DestroySurface(surface_game); 

    SDL_Surface *surface_exit = TTF_RenderText_Solid(font, "Retour Menu",strlen("Retour Menu"), blanc); 
    SDL_Texture *texte_exit = SDL_CreateTextureFromSurface(renderer, surface_exit); 
    SDL_DestroySurface(surface_exit);

    if (!texte_game || !texte_exit) {
        SDL_Log("Erreur creation texture texte game over: %s", SDL_GetError());
        if (texte_game) SDL_DestroyTexture(texte_game);
        if (texte_exit) SDL_DestroyTexture(texte_exit);
        SDL_DestroyTexture(BoutonText);
        TTF_CloseFont(font);
        TTF_CloseFont(font_game_over);
        return result;
    }

    int continuer = 1;
    while (continuer) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_MOUSE_MOTION ||
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                SDL_ConvertEventToRenderCoordinates(renderer, &event);
            }

            if (Bouton_GererEvenement(&bouton_game, &event)) {
                perso = (Perso){screen_center_x() - 1080.0f, screen_center_y() - 900.0f, NULL, 0, 10, 10, 10, 10, SDL_GetTicks()};
                result = 1;
                continuer = 0;
            }
            if (Bouton_GererEvenement(&bouton_quitter, &event)) {
                result = 0;
                continuer = 0;
            }
        }

        if (!continuer) {
            break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, NULL);

        SDL_Color rouge = {255, 0, 0, 255};
        SDL_Surface *surface_game_over = TTF_RenderText_Solid(font_game_over, "Game Over", 0, rouge);
        if (!surface_game_over) {
            SDL_Log("TTF error: %s", SDL_GetError());
            result = -1;
            break;
        }
        SDL_Texture *texte_game_over = SDL_CreateTextureFromSurface(renderer, surface_game_over);
        SDL_DestroySurface(surface_game_over);
        if (!texte_game_over) {
            SDL_Log("Texture error: %s", SDL_GetError());
            result = -1;
            break;
        }
        SDL_FRect dest_game_over = {screen_center_x() - 200.0f, 100.0f, 400.0f, 160.0f};
        SDL_RenderTexture(renderer, texte_game_over, NULL, &dest_game_over);
        SDL_DestroyTexture(texte_game_over);


        Bouton_Afficher(&bouton_game, renderer);
        afficher_texte_centre(renderer, texte_game, &bouton_game.rect);
        
        Bouton_Afficher(&bouton_quitter, renderer);
        afficher_texte_centre(renderer, texte_exit, &bouton_quitter.rect);

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    SDL_DestroyTexture(texte_game);
    SDL_DestroyTexture(texte_exit);
    SDL_DestroyTexture(BoutonText);
    TTF_CloseFont(font);
    TTF_CloseFont(font_game_over);
    return result;
}

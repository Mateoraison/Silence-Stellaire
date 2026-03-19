#include "headers/main.h"

int game_over(SDL_Renderer *renderer){

    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        return 1;
    }

    TTF_Font *font_game_over = TTF_OpenFont("assets/BiggerBook.ttf", 24);
    if (!font_game_over) {
        SDL_Log("Erreur chargement police: %s", SDL_GetError());
        return 1;
    }

    SDL_Texture *BoutonText = IMG_LoadTexture(renderer, "assets/menu/bouton.png");
    if(!BoutonText) {
        SDL_Log("erreur chargement texture bouton: %s", SDL_GetError());
        TTF_CloseFont(font);
        return 1;
    }

    Bouton bouton_game;
    Bouton_Init(&bouton_game, 400.0f, 350.0f, 200.0f, 80.0f, BoutonText);

    Bouton bouton_quitter;
    Bouton_Init(&bouton_quitter, 400.0f, 450.0f, 200.0f, 80.0f, BoutonText);

    SDL_Color blanc = {255, 255, 255, 255};

    SDL_Surface *surface_game = TTF_RenderText_Solid(font, "Recommencer",strlen("Recommencer"), blanc); 
    SDL_Texture *texte_game = SDL_CreateTextureFromSurface(renderer, surface_game); 
    SDL_DestroySurface(surface_game); 

    SDL_Surface *surface_exit = TTF_RenderText_Solid(font, "Retour Menu",strlen("Retour Menu"), blanc); 
    SDL_Texture *texte_exit = SDL_CreateTextureFromSurface(renderer, surface_exit); 
    SDL_DestroySurface(surface_exit);

    int continuer = 1;
    while (continuer) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (Bouton_GererEvenement(&bouton_game, &event)) {
                perso = (Perso){-580.0f, -500.0f, NULL, 0, 10, 10, 10, 10, SDL_GetTicks()};
                TTF_CloseFont(font);
                TTF_CloseFont(font_game_over);
                SDL_DestroyTexture(BoutonText);
                return 1;
            }
            if (Bouton_GererEvenement(&bouton_quitter, &event)) {
                TTF_CloseFont(font);
                TTF_CloseFont(font_game_over);
                SDL_DestroyTexture(BoutonText);
                return 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, NULL);

        SDL_Color rouge = {255, 0, 0, 255};
        SDL_Surface *surface_game_over = TTF_RenderText_Solid(font_game_over, "Game Over", 0, rouge);
        if (!surface_game_over) {
            SDL_Log("TTF error: %s", SDL_GetError());
            return -1;
        }
        SDL_Texture *texte_game_over = SDL_CreateTextureFromSurface(renderer, surface_game_over);
        SDL_DestroySurface(surface_game_over);
        if (!texte_game_over) {
            SDL_Log("Texture error: %s", SDL_GetError());
            return -1;
        }
        SDL_FRect dest_game_over = {300.0f, 100.0f, 400.0f, 160.0f};
        SDL_RenderTexture(renderer, texte_game_over, NULL, &dest_game_over);
        SDL_DestroyTexture(texte_game_over);


        Bouton_Afficher(&bouton_game, renderer);
        afficher_texte_centre(renderer, texte_game, &bouton_game.rect);
        
        Bouton_Afficher(&bouton_quitter, renderer);
        afficher_texte_centre(renderer, texte_exit, &bouton_quitter.rect);

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    } 
}

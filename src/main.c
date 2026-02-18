#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "headers/main.h"



 


int main(int argc, char* argv[]) {
    MIX_Track *track_global = NULL; 
    (void)argc;
    (void)argv;
    

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
        SDL_Log("impossible a charger SDL: %s", SDL_GetError());
        return 1;
    }
    if(TTF_Init() == false) {
        SDL_Log("impossible a charger SDL_ttf: %s", SDL_GetError());
        SDL_Quit(); 
        return 1; 
    }

    if(MIX_Init() == false){
        SDL_Log("impossible a charger SDL_mixer: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }


    SDL_Window *fenetre = SDL_CreateWindow("Silence Stellaire", 1000, 800, SDL_WINDOW_MAXIMIZED);
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
    track_global = jouer_son("assets/audio/ambiance.wav");
    while (running) {
        int action_menu = afficher_menu(renderer);
        if (action_menu == 1) {
            int resultat_jeu = jeu_principal(renderer);
            if (resultat_jeu == 0) running = false;
        }else if(action_menu == 2){
            int resultat_option = afficher_option(renderer,track_global);
            if(resultat_option == 0) running = false;
        }
        else {
            running = false;
        }
        
       
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    MIX_Quit();
    SDL_Quit();
    return 0;
}


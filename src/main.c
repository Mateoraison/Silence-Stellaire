#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "headers/main.h"

typedef enum {
    ETAT_MENU,
    ETAT_JEU,
    ETAT_VAISSEAU,
    ETAT_MAP,
    ETAT_OPTIONS,
    ETAT_QUITTER
} EtatJeu;

static int init_sdl(SDL_Window **fenetre, SDL_Renderer **renderer) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Erreur SDL_Init : %s", SDL_GetError());
        return 0;
    }
    if (!TTF_Init()) {
        SDL_Log("Erreur TTF_Init : %s", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    if (!MIX_Init()) {
        SDL_Log("Erreur MIX_Init : %s", SDL_GetError());
        TTF_Quit(); SDL_Quit();
        return 0;
    }

    *fenetre = SDL_CreateWindow("Silence Stellaire", 1000, 800, SDL_WINDOW_MAXIMIZED);
    if (!*fenetre) {
        SDL_Log("Erreur création fenêtre : %s", SDL_GetError());
        MIX_Quit(); TTF_Quit(); SDL_Quit();
        return 0;
    }

    *renderer = SDL_CreateRenderer(*fenetre, NULL);
    if (!*renderer) {
        SDL_Log("Erreur création renderer : %s", SDL_GetError());
        SDL_DestroyWindow(*fenetre);
        MIX_Quit(); TTF_Quit(); SDL_Quit();
        return 0;
    }

    SDL_SetWindowIcon(*fenetre, IMG_Load("assets/logo_win.png"));
    return 1;
}

static void quitter_sdl(SDL_Window *fenetre, SDL_Renderer *renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    MIX_Quit();
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SDL_Window   *fenetre  = NULL;
    SDL_Renderer *renderer = NULL;

    if (!init_sdl(&fenetre, &renderer))
        return 1;

    init_caisse_outils(renderer);
    MIX_Track *track_global = jouer_son("assets/audio/ambiance.wav", 0.3f);

    int     planete_actuelle = 1;
    EtatJeu etat             = ETAT_MENU;

    while (etat != ETAT_QUITTER) {
        switch (etat) {
            case ETAT_MENU: {
                int action = afficher_menu(renderer);
                switch (action) {
                    case 1:  etat = ETAT_JEU;     break; // Nouvelle partie
                    case 2:  etat = ETAT_OPTIONS;  break; // Options
                    default: etat = ETAT_QUITTER;  break; // Quitter / Échap
                }
                break;
            }
            case ETAT_JEU: {
                if (track_global) reprendre_son(track_global);
                int code = jeu_principal(renderer, planete_actuelle, track_global);
                switch (code) {
                    case 1:  etat = ETAT_MENU;     break;
                    case 3:  etat = ETAT_MAP;       break;
                    case 4:  etat = ETAT_VAISSEAU;  break;
                    default: etat = ETAT_MENU;      break;
                }
                break;
            }
            case ETAT_VAISSEAU: {
                int code = vaisseau(renderer);
                switch (code) {
                    case 1:  etat = ETAT_MENU; break;
                    case 3:  etat = ETAT_MAP;  break;
                    default: etat = ETAT_JEU;  break;
                }
                break;
            }
            case ETAT_MAP: {
                int planete_choisie = afficher_map(renderer);
                if (planete_choisie >= 1 && planete_choisie <= 3)
                    planete_actuelle = planete_choisie;
                etat = ETAT_JEU;
                break;
            }
            case ETAT_OPTIONS: {
                afficher_option(renderer, track_global);
                etat = ETAT_MENU;
                break;
            }

            default:
                etat = ETAT_QUITTER;
                break;
        }
    }

    quitter_sdl(fenetre, renderer);
    return 0;
}

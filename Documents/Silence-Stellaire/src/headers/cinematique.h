#ifndef CINEMATIQUE_H
#define CINEMATIQUE_H

#include <SDL3/SDL.h>

/*
 * Retourne 0 si la cinematique se termine ou est passee,
 * retourne 1 si le joueur ferme la fenetre.
 */
int jouer_cinematique_intro(SDL_Renderer *renderer);

int jouer_cinematique_crash(SDL_Renderer *renderer);

/* Cinematique de fin jouee quand le joueur clique sur la Galaxie (planete 4).
 * Retourne 0 si la cinematique termine normalement, 1 si la fenetre a ete fermee.
 */
int jouer_cinematique_fin(SDL_Renderer *renderer);

/* Affiche les credits de fin puis retourne au menu.
 * Retourne 0 si termine normalement, 1 si la fenetre a ete fermee.
 */
int jouer_credits(SDL_Renderer *renderer);

#endif
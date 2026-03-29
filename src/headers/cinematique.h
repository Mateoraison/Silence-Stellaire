#ifndef CINEMATIQUE_H
#define CINEMATIQUE_H

#include <SDL3/SDL.h>

/*
 * Retourne 0 si la cinematique se termine ou est passee,
 * retourne 1 si le joueur ferme la fenetre.
 */
int jouer_cinematique_intro(SDL_Renderer *renderer);

#endif
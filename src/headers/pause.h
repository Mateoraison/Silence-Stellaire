/**
 * @file pause.h
 * @brief Fichier du menu pause et des actions de reprise/sauvegarde.
 */

#ifndef PAUSE_H
#define PAUSE_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

// Codes de retour de afficher_pause()
#define PAUSE_REPRENDRE   0
#define PAUSE_MENU        1
#define PAUSE_OPTIONS     2
#define PAUSE_SAUVEGARDE  3 

/**
 * @brief Affiche pause et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param track_global Piste audio globale (musique d'ambiance) partagee entre ecrans.
 * @return Code parmi PAUSE_REPRENDRE, PAUSE_MENU, PAUSE_OPTIONS ou PAUSE_SAUVEGARDE.
 */
int afficher_pause(SDL_Renderer *renderer, MIX_Track *track_global);

#endif

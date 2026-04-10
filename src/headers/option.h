/**
 * @file option.h
 * @brief Fichier de l'ecran d'options et des reglages de jeu.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/**
 * @brief Affiche option et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param track_global Piste audio globale (musique d'ambiance) partagee entre ecrans.
 * @return 1 si retour normal vers l'ecran appelant, 0 si fermeture fenetre, -1 en cas d'erreur d'initialisation.
 */
int afficher_option(SDL_Renderer *renderer, MIX_Track *track_global);

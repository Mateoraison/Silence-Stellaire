/**
 * @file menu.h
 * @brief Fichier du menu principal et des choix de navigation.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/**
 * @brief Affiche menu et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @return 0 quitter, 1 nouvelle partie, 2 options, 3 continuer depuis une sauvegarde.
 */
int afficher_menu(SDL_Renderer *renderer);

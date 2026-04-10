/**
 * @file game_over.h
 * @brief Fichier de l'ecran de fin de partie et des transitions associees.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/**
 * @brief Affiche l'ecran de game over et traite le choix du joueur.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param planete Identifiant de la planete pour repositionner correctement le joueur en cas de reprise.
 * @return 1 pour recommencer, 0 pour revenir au menu, -1 en cas d'erreur interne d'affichage.
 */
int game_over(SDL_Renderer *renderer, int planete);

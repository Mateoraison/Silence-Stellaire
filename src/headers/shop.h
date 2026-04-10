/**
 * @file shop.h
 * @brief Fichier de la boutique, des achats et de l'economie joueur.
 */

#ifndef SHOP_H
#define SHOP_H

#include <SDL3/SDL.h>

/**
 * @brief Affiche shop et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param hotbar Tableau des slots de raccourcis equipables du joueur.
 * @return 0 si la boutique est fermee normalement, 1 si la fenetre est fermee.
 */
int afficher_shop(SDL_Renderer *renderer, t_case *hotbar[HOTBAR_SIZE]);

#endif

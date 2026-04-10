/**
 * @file arcade.h
 * @brief Fichier des mini-jeux d'arcade utilises dans la progression.
 */

#ifndef ARCADE_H
#define ARCADE_H

#include <SDL3/SDL.h>

/**
 * @brief Lance arcade1 et gere son deroulement complet.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void jouer_arcade1(SDL_Renderer *renderer) ;
/**
 * @brief Lance arcade2 et gere son deroulement complet.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void jouer_arcade2(SDL_Renderer *renderer) ;
/**
 * @brief Lance arcade3 et gere son deroulement complet.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void jouer_arcade3(SDL_Renderer *renderer) ;
/**
 * @brief Lance le mini-jeu Mastermind utilise dans la progression.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void mastermind(SDL_Renderer *renderer);
/**
 * @brief Lance le mini-jeu Simon utilise dans la progression.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void simon(SDL_Renderer *renderer);

#endif // ARCADE_H

/**
 * @file vaisseau.h
 * @brief Fichier de la scene vaisseau, des interactions et du retour en planete.
 */

#ifndef VAISSEAU_H
#define VAISSEAU_H

#include <SDL3/SDL.h>

/**
 * @brief Positionne le personnage a une tuile donnee en recentrant la camera.
 * @param tileX Colonne de la tuile de spawn dans la grille du vaisseau.
 * @param tileY Ligne de la tuile de spawn dans la grille du vaisseau.
 */
void spawn_perso(int tileX, int tileY) ;
/**
 * @brief Lance la scene interieure du vaisseau (deplacement, interactions, mini-jeux, sortie).
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param planete_actuelle Identifiant de la planete active au moment de l'appel.
 * @return Code de sortie de la scene (0 retour planete, 1 menu/quit, 3 ouverture map, -1 erreur chargement).
 */
int vaisseau(SDL_Renderer *renderer, int planete_actuelle) ; 

#endif // VAISSEAU_H

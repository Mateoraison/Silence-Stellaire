/**
 * @file planete.h
 * @brief Fichier des ecrans de planetes et transitions d'environnement.
 */

#ifndef PLANETE_H
#define PLANETE_H

#include <SDL3/SDL.h>
#include "main.h"

/**
 * @brief Genere le labyrinthe procedural de la planete 3 et place l'engrenage objectif.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param engrenage_case_x Pointeur de sortie recevant la case X de l'engrenage place.
 * @param engrenage_case_y Pointeur de sortie recevant la case Y de l'engrenage place.
 */
void generer_labyrinthe_planete3(t_tile map[W_MAP][H_MAP], int *engrenage_case_x, int *engrenage_case_y);
/**
 * @brief Verifie si une case de la planete 3 peut servir de spawn boss.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param tx Case X candidate.
 * @param ty Case Y candidate.
 * @return 1 si la case est valide pour le spawn, 0 sinon.
 */
int planete3_case_spawn_boss_valide(t_tile map[W_MAP][H_MAP], int tx, int ty);
/**
 * @brief Choisit automatiquement un point de spawn valide pour le boss de la planete 3.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param spawn_x Pointeur de sortie recevant la position monde X du spawn.
 * @param spawn_y Pointeur de sortie recevant la position monde Y du spawn.
 * @return 1 si un spawn a ete trouve, 0 sinon.
 */
int choisir_spawn_boss_planete3(t_tile map[W_MAP][H_MAP], float *spawn_x, float *spawn_y);
/**
 * @brief Applique un masque de vision reduite autour du joueur sur la planete 3.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param centre_x Centre X de la zone visible.
 * @param centre_y Centre Y de la zone visible.
 * @param largeur_ecran Largeur de la vue courante en pixels.
 * @param hauteur_ecran Hauteur de la vue courante en pixels.
 */
void appliquer_vision_reduite_planete3(SDL_Renderer *renderer, float centre_x, float centre_y, float largeur_ecran, float hauteur_ecran);

#endif

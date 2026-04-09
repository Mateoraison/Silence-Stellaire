#ifndef PLANETE_H
#define PLANETE_H

#include <SDL3/SDL.h>
#include "main.h"

void generer_labyrinthe_planete3(t_tile map[W_MAP][H_MAP], int *engrenage_case_x, int *engrenage_case_y);
int planete3_case_spawn_boss_valide(t_tile map[W_MAP][H_MAP], int tx, int ty);
int choisir_spawn_boss_planete3(t_tile map[W_MAP][H_MAP], float *spawn_x, float *spawn_y);
void appliquer_vision_reduite_planete3(SDL_Renderer *renderer, float centre_x, float centre_y, float largeur_ecran, float hauteur_ecran);

#endif
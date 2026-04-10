/**
 * @file map.h
 * @brief Fichier de navigation carte et de gestion des ressources cartographiques.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

/**
 * @brief Affiche la carte et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param track_global Piste audio globale (musique d'ambiance) partagee entre ecrans.
 * @return Numero de planete choisie (1..4), 0 si fermeture/retour sans selection, -1 apres sequence de fin.
 */
int afficher_map(SDL_Renderer *renderer, MIX_Track *track_global);
/* release resources created by afficher_map */
/**
 * @brief Libere les ressources persistantes allouees par le module carte.
 */
void free_map_resources(void);

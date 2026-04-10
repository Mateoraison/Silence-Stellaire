/**
 * @file jeux.h
 * @brief Fichier de la boucle de jeu principale et de son initialisation.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

/**
 * @brief Execute la boucle principale de gameplay pour la planete courante.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param planete Identifiant de planete concernee par l'operation.
 * @param track_global Piste audio globale (musique d'ambiance) partagee entre ecrans.
 * @param reprendre_partie Indique s'il faut relancer une partie depuis une sauvegarde.
 * @return 0 si la boucle se termine normalement, 1 en cas de retour menu/fermeture ou erreur de chargement.
 */
int jeu_principal(SDL_Renderer *renderer, int planete, MIX_Track *track_global, bool reprendre_partie);
/**
 * @brief Initialise la caisse a outils et prepare les ressources necessaires.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void init_caisse_outils(SDL_Renderer *renderer);

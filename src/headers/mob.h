/**
 * @file mob.h
 * @brief Fichier des mobs: creation, mise a jour, rendu et file de respawn.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/**
 * @brief Initialise mobs et prepare les ressources necessaires.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param mobs Tableau des mobs actifs manipules par la fonction.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param nb_pawns Nombre de pawns a instancier durant l'initialisation.
 * @param nb_moutons Nombre de moutons a instancier durant l'initialisation.
 */
void init_mobs(SDL_Renderer * renderer, Mob * mobs[MAX_MOB], t_tile map[W_MAP][H_MAP], int nb_pawns, int nb_moutons);
/**
 * @brief Met a jour mobs en fonction de l'etat courant du jeu.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param mobs Tableau des mobs actifs manipules par la fonction.
 */
void update_mobs(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB]);
/**
 * @brief Affiche mob et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param mobs Tableau des mobs actifs manipules par la fonction.
 */
void afficher_mob(SDL_Renderer * renderer, Mob * mobs[MAX_MOB]);
/**
 * @brief Libere mobs et nettoie les ressources associees.
 * @param mobs Tableau des mobs actifs manipules par la fonction.
 */
void detruire_mobs(Mob * mobs[MAX_MOB]);
/**
 * @brief Libere un mob et nettoie les ressources associees.
 * @param mob Pointeur vers le mob cible a traiter.
 */
void detruire_un_mob(Mob * mob);
/**
 * @brief Reinitialise mob respawn queue pour repartir d'un etat propre.
 */
void reset_mob_respawn_queue(void);
/**
 * @brief Programme la reapparition differee d'un mob dans la file de respawn.
 * @param id Identifiant du type d'entite a manipuler.
 * @param delay_ms Delai (en millisecondes) avant execution de l'action planifiee.
 */
void schedule_respawn(int id, Uint32 delay_ms);

/**
 * @file boss.h
 * @brief API de gestion des boss, de leurs phases et de leurs comportements de combat.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/**
 * @brief Initialise boss et prepare les ressources necessaires.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param boss Structure boss cible dont l'etat est consulte ou modifie.
 * @param type Type logique de l'entite ou de l'operation a traiter.
 * @param x Coordonnee X de reference (monde ou ecran selon le contexte).
 * @param y Coordonnee Y de reference (monde ou ecran selon le contexte).
 */
void init_boss(SDL_Renderer *renderer, boss_t *boss, type_boss_t type, float x, float y);
/**
 * @brief Affiche boss et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param boss Structure boss cible dont l'etat est consulte ou modifie.
 */
void afficher_boss(SDL_Renderer *renderer, boss_t *boss);
/**
 * @brief Met a jour boss en fonction de l'etat courant du jeu.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param boss Structure boss cible dont l'etat est consulte ou modifie.
 */
void mettre_a_jour_boss(SDL_Renderer *renderer, boss_t *boss);
/**
 * @brief Execute la logique d'attaque du boss en tenant compte des cooldowns et de la phase active.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param boss Structure boss cible dont l'etat est consulte ou modifie.
 */
void boss_attaque(SDL_Renderer *renderer, boss_t *boss);
/**
 * @brief Met a jour animation boss en fonction de l'etat courant du jeu.
 * @param boss Structure boss cible dont l'etat est consulte ou modifie.
 */
void mettre_a_jour_animation_boss(boss_t *boss);
/**
 * @brief Libere boss et nettoie les ressources associees.
 * @param boss Structure boss cible dont l'etat est consulte ou modifie.
 */
void detruire_boss(boss_t *boss);
/**
 * @brief Definit la carte de navigation exploitee par l'IA des boss.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 */
void boss_set_navigation_map(t_tile map[W_MAP][H_MAP]);
/**
 * @brief Reinitialise boss for retry pour repartir d'un etat propre.
 * @param boss_ref Reference du boss a reinitialiser pour une nouvelle tentative.
 */
void reset_boss_for_retry(boss_t *boss_ref);


/**
 * @file boss.h
 * @brief Fichier du module boss (declarations publiques).
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/**
 * @brief Initialise un boss avec ses ressources graphiques et son etat de combat.
 * @param renderer Contexte SDL utilise pour charger les textures et preparer le rendu du boss.
 * @param boss Structure boss cible a initialiser.
 * @param type Type de boss a instancier (utilise pour choisir comportement et sprite).
 * @param x Position X initiale du boss en espace monde.
 * @param y Position Y initiale du boss en espace monde.
 */
void init_boss(SDL_Renderer *renderer, boss_t *boss, type_boss_t type, float x, float y);
/**
 * @brief Affiche le boss, ses indicateurs visuels et ses effets actifs.
 * @param renderer Contexte SDL utilise pour dessiner le boss a l'ecran.
 * @param boss Structure boss source pour les informations de rendu (animation, phase, vie).
 */
void afficher_boss(SDL_Renderer *renderer, boss_t *boss);
/**
 * @brief Met a jour la logique du boss pour une frame de jeu.
 * @param renderer Contexte SDL utilise pour les operations liees au rendu et aux effets.
 * @param boss Structure boss dont l'IA, les cooldowns et l'etat sont actualises.
 */
void mettre_a_jour_boss(SDL_Renderer *renderer, boss_t *boss);
/**
 * @brief Declenche et gere les attaques du boss selon sa phase active.
 * @param renderer Contexte SDL utilise pour les attaques visuelles et projectiles.
 * @param boss Structure boss cible contenant cooldowns, phase et statistiques d'attaque.
 */
void boss_attaque(SDL_Renderer *renderer, boss_t *boss);
/**
 * @brief Met a jour les animations du boss (idle, attaque, mort) selon son etat courant.
 * @param boss Structure boss dont les compteurs d'animation sont mis a jour.
 */
void mettre_a_jour_animation_boss(boss_t *boss);
/**
 * @brief Detruit le boss et libere toutes les ressources associees.
 * @param boss Structure boss cible a nettoyer (textures, projectiles, etats temporaires).
 */
void detruire_boss(boss_t *boss);
/**
 * @brief Definit la carte de navigation utilisee par l'IA des boss.
 * @param map Grille de tuiles de reference utilisee pour les deplacements et collisions du boss.
 */
void boss_set_navigation_map(t_tile map[W_MAP][H_MAP]);
/**
 * @brief Reinitialise un boss pour relancer un combat apres echec joueur.
 * @param boss_ref Reference de la structure boss a remettre dans son etat initial.
 */
void reset_boss_for_retry(boss_t *boss_ref);


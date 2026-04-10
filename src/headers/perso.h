/**
 * @file perso.h
 * @brief Fichier du personnage: deplacement, combat, stats et collisions.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>



/**
 * @brief Affiche perso et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @return 0 si l'affichage reussit, 1 si la texture du frame ne peut pas etre chargee.
 */
int afficher_perso(SDL_Renderer *renderer);
/**
 * @brief Met a jour la position du joueur selon les entrees clavier et le delta temps.
 * @param delta_time Temps ecoule depuis la precedente mise a jour, exprime en secondes.
 * @return 0 (fonction de mise a jour sans code d'erreur specifique).
 */
int deplacer_perso(float delta_time);
/**
 * @brief Met a jour animation en fonction de l'etat courant du jeu.
 */
void update_animation();
/**
 * @brief Verifie collision et retourne le resultat du test.
 * @param x Coordonnee X de reference (monde ou ecran selon le contexte).
 * @param y Coordonnee Y de reference (monde ou ecran selon le contexte).
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param est_mob Indique si la verification concerne un mob (1) ou le joueur (0).
 * @param hitbox Rectangle de collision utilise pour les tests d'intersection.
 * @return 1 si collision detectee, 0 sinon.
 */
int test_collision(int x, int y, t_tile map[W_MAP][H_MAP], int est_mob, SDL_Rect hitbox);
/**
 * @brief Affiche les statistiques et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @return 0 apres rendu des jauges de vie/faim.
 */
int afficher_stat(SDL_Renderer *renderer);
/**
 * @brief Gere combat a partir des entrees utilisateur et des evenements.
 * @param event Evenement SDL recu, interprete par la logique de traitement.
 */
void gerer_combat(SDL_Event event);
/**
 * @brief Affiche combat et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void afficher_combat(SDL_Renderer *renderer);
/**
 * @brief Met a jour combat en fonction de l'etat courant du jeu.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param mobs Tableau des mobs actifs manipules par la fonction.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param items Tableau des items presents dans la scene courante.
 */
void update_combat(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB], SDL_Renderer * renderer, t_Item * items[MAX_ITEMS]);
/**
 * @brief Evalue les collisions de combat (joueur/mobs/items) pour la frame courante.
 * @param map Grille de tuiles utilisee pour collisions, navigation et rendu contextuel.
 * @param mobs Tableau des mobs actifs manipules par la fonction.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param items Tableau des items presents dans la scene courante.
 */
void tester_collision_combat(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB], SDL_Renderer * renderer, t_Item * items[MAX_ITEMS]);
/**
 * @brief Evalue ramasser item puis applique l'action autorisee si les conditions sont reunies.
 * @param items Tableau des items presents dans la scene courante.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param hotbar Tableau des slots de raccourcis equipables du joueur.
 */
void possible_ramasser_item(t_Item * items[MAX_ITEMS], SDL_Renderer * renderer, t_case * hotbar[HOTBAR_SIZE]);



/**
 * @file inventaire.h
 * @brief Fichier de l'inventaire, de la hotbar et des interactions de slots.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>


/**
 * @brief Affiche hotbar et met a jour les elements visuels associes.
 * @param hotbar Tableau des slots de raccourcis equipables du joueur.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void afficher_hotbar(t_case * hotbar[HOTBAR_SIZE], SDL_Renderer *renderer);
/**
 * @brief Ajoute un item dans la hotbar (slot libre ou pile compatible).
 * @param hotbar Tableau des slots de raccourcis equipables du joueur.
 * @param item Item a inserer dans la hotbar.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void ajouter_item_hotbar(t_case *hotbar[HOTBAR_SIZE], t_Item *item, SDL_Renderer *renderer);
/**
 * @brief Affiche inventaire et met a jour les elements visuels associes.
 * @param inventaire Tableau des cases d'inventaire a lire ou modifier.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param inventaire_size Nombre total de cases prises en compte dans l'inventaire.
 * @param inventaire_cols Nombre de colonnes utilisees pour afficher la grille d'inventaire.
 * @param inventaire_rows Nombre de lignes utilisees pour afficher la grille d'inventaire.
 */
void afficher_inventaire(t_case *inventaire[], SDL_Renderer *renderer, int inventaire_size, int inventaire_cols, int inventaire_rows);
/**
 * @brief Ajoute un item dans l'inventaire principal (slot libre ou pile compatible).
 * @param inventaire Tableau des cases d'inventaire a lire ou modifier.
 * @param inventaire_size Nombre total de cases prises en compte dans l'inventaire.
 * @param item Item a inserer dans l'inventaire.
 */
void ajouter_item_inventaire(t_case **inventaire, int inventaire_size, t_Item *item);

/**
 * @brief Gere clic inventaire a partir des entrees utilisateur et des evenements.
 * @param inventaire Tableau des cases d'inventaire a lire ou modifier.
 * @param hotbar Tableau des slots de raccourcis equipables du joueur.
 * @param event Evenement SDL recu, interprete par la logique de traitement.
 * @param inventaire_size Nombre total de cases prises en compte dans l'inventaire.
 * @param inventaire_cols Nombre de colonnes utilisees pour afficher la grille d'inventaire.
 * @param inventaire_rows Nombre de lignes utilisees pour afficher la grille d'inventaire.
 */
void gerer_clic_inventaire(t_case **inventaire, t_case **hotbar, SDL_Event *event, int inventaire_size, int inventaire_cols, int inventaire_rows);

/**
 * @file item.h
 * @brief Fichier de creation, d'affichage et de destruction des items.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>





/**
 * @brief Initialise item et prepare les ressources necessaires.
 * @param type Type logique de l'entite ou de l'operation a traiter.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param x Coordonnee X de reference (monde ou ecran selon le contexte).
 * @param y Coordonnee Y de reference (monde ou ecran selon le contexte).
 * @return Pointeur vers l'item initialise, ou NULL en cas d'echec d'allocation/texture.
 */
t_Item * init_item(typeItem type,SDL_Renderer * renderer,float x,float y);
/**
 * @brief Affiche item et met a jour les elements visuels associes.
 * @param items Tableau des items presents dans la scene courante.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void afficher_item(t_Item * items[MAX_ITEMS],SDL_Renderer * renderer);
/**
 * @brief Libere item et nettoie les ressources associees.
 * @param item Adresse du pointeur item a detruire et remettre a NULL.
 */
void detruire_item(t_Item ** item);
/**
 * @brief Tente de transferer un item du sol vers la hotbar du joueur.
 * @param item Item cible a ramasser.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param hotbar Tableau des slots de raccourcis equipables du joueur.
 */
void ramasser_item(t_Item * item, SDL_Renderer * renderer, t_case * hotbar[HOTBAR_SIZE]);
/**
 * @brief Libere tout item et nettoie les ressources associees.
 * @param items Tableau des items presents dans la scene courante.
 */
void detruire_tout_item(t_Item * items[MAX_ITEMS]);

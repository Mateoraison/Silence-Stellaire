/**
 * @file bouton.h
 * @brief Fichier des boutons UI, de leur etat et de la gestion des interactions souris.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define BOUTON_H

#include <SDL3/SDL.h>

/**
 * @struct Bouton
 * @brief Widget bouton basique: zone de clic, texture et etats survole/appuye.
 */
typedef struct {
    SDL_FRect rect;       
    SDL_Texture *texture;
    int survole;
    int appuye;
} Bouton;

// x, y, w, h = position/taille à l'écran
// texture = image déjà chargée (SDL_Texture*)
/**
 * @fn void Bouton_Init(Bouton *b, float x, float y, float w, float h, SDL_Texture *texture)
 * @brief Initialise un bouton (zone, texture et etat d'interaction).
 * @param b Bouton cible a initialiser, afficher ou mettre a jour.
 * @param x Coordonnee X de reference (monde ou ecran selon le contexte).
 * @param y Coordonnee Y de reference (monde ou ecran selon le contexte).
 * @param w Largeur cible (en pixels ou en unites, selon la fonction).
 * @param h Hauteur cible (en pixels ou en unites, selon la fonction).
 * @param texture Texture SDL associee a l'element graphique manipule.
 */
void Bouton_Init(Bouton *b, float x, float y, float w, float h, SDL_Texture *texture);

// retourne 1 si le bouton a été cliqué à ce frame, sinon 0
/**
 * @fn int Bouton_GererEvenement(Bouton *b, const SDL_Event *e)
 * @brief Met a jour l'etat du bouton a partir d'un evenement SDL.
 * @param b Bouton cible a initialiser, afficher ou mettre a jour.
 * @param e Evenement SDL recu, interprete par la logique de traitement.
 * @return 1 si un clic valide vient d'etre detecte sur ce bouton, 0 sinon.
 */
int Bouton_GererEvenement(Bouton *b, const SDL_Event *e);

// dessine l'image du bouton dans son rect
/**
 * @fn void Bouton_Afficher(Bouton *b, SDL_Renderer *renderer)
 * @brief Dessine la texture du bouton dans son rectangle d'affichage.
 * @param b Bouton cible a initialiser, afficher ou mettre a jour.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void Bouton_Afficher(Bouton *b, SDL_Renderer *renderer);
/**
 * @fn void afficher_texte_centre(SDL_Renderer *renderer, SDL_Texture *texture, SDL_FRect *rect_bouton)
 * @brief Affiche texte centre et met a jour les elements visuels associes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param texture Texture SDL associee a l'element graphique manipule.
 * @param rect_bouton Rectangle de reference servant au placement du texte dans le bouton.
 */
void afficher_texte_centre(SDL_Renderer *renderer, SDL_Texture *texture, SDL_FRect *rect_bouton);


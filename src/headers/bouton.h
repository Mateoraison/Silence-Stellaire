#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>


#ifndef BOUTON_H
#define BOUTON_H

#include <SDL3/SDL.h>

typedef struct {
    SDL_FRect rect;       
    SDL_Texture *texture;
    int survole;
    int appuye;
} Bouton;

// x, y, w, h = position/taille à l'écran
// texture = image déjà chargée (SDL_Texture*)
void Bouton_Init(Bouton *b, float x, float y, float w, float h, SDL_Texture *texture);

// retourne 1 si le bouton a été cliqué à ce frame, sinon 0
int Bouton_GererEvenement(Bouton *b, const SDL_Event *e);

// dessine l'image du bouton dans son rect
void Bouton_Afficher(Bouton *b, SDL_Renderer *renderer);
void afficher_texte_centre(SDL_Renderer *renderer, SDL_Texture *texture, SDL_FRect *rect_bouton);


#endif

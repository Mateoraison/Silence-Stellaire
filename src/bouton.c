#include "headers/bouton.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>


//verifie si un point est dans un rectangle
int pointDansRect(float x, float y, const SDL_FRect *r) {
    return x >= r->x && x <= r->x + r->w && y >= r->y && y <= r->y + r->h;
}

//initialise un bouton
void Bouton_Init(Bouton *b, float x, float y, float w, float h, SDL_Texture *texture) {
    b->rect.x = x;
    b->rect.y = y;
    b->rect.w = w;
    b->rect.h = h;
    b->texture = texture;
    b->survole = 0;
    b->appuye = 0;
}

int Bouton_GererEvenement(Bouton *b, const SDL_Event *e) {
    int clique = 0;

    if (e->type == SDL_EVENT_MOUSE_MOTION) {
        float mx = e->motion.x;
        float my = e->motion.y;
        b->survole = pointDansRect(mx, my, &b->rect);

    } else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN && e->button.button == SDL_BUTTON_LEFT) {

        float mx = e->button.x;
        float my = e->button.y;
        if (pointDansRect(mx, my, &b->rect)) {
            b->appuye = 1;
        }

    } else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP && e->button.button == SDL_BUTTON_LEFT) {

        float mx = e->button.x;
        float my = e->button.y;
        if (b->appuye && pointDansRect(mx, my, &b->rect)) {
            clique = 1;
        }
        b->appuye = 0;
    }

    return clique;
}

void Bouton_Afficher(Bouton *b, SDL_Renderer *renderer) {
    if (!b->texture) return;
    SDL_RenderTexture(renderer, b->texture, NULL, &b->rect);
}

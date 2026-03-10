#include "headers/main.h"





void afficher_hotbar(t_case * hotbar[HOTBAR_SIZE], SDL_Renderer *renderer) {
    SDL_Texture * texture = IMG_LoadTexture(renderer, "assets/UI/slot_inventaire.png");
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        SDL_FRect dest = {340.0f + i * 70, 720, 60, 60};
        SDL_RenderTexture(renderer, texture, NULL, &dest);
        if (hotbar[i] != NULL && hotbar[i]->item != NULL) {
            SDL_FRect item_dest = {310.0f + i * 70 + 5, 720 + 5, 30, 30};
            SDL_RenderTexture(renderer, hotbar[i]->item->texture, NULL, &item_dest);
        }
    }
}

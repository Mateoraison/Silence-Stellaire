#include "main.h"





t_Item * init_item(typeItem type,SDL_Renderer * renderer){
    t_Item * item = malloc(sizeof(t_Item));
    item->texture = NULL;
    item->type =  type;
    switch (type){
        case BOIS: item->texture = IMG_LoadTexture(renderer,"assets/UI/bois.png"); break;
        case VIANDE: item->texture = IMG_LoadTexture(renderer,"assets/UI/viande.png"); break;
    }
    if(item->texture == NULL){
        SDL_Log("init item fail ");
        free(item);
        return NULL;
    }    
    return item;
}

void afficher_item(t_Item * item, float x, float y,SDL_Renderer * renderer){
    SDL_FRect dest = {x*DISPLAY_TILE_SIZE+perso.x,y*DISPLAY_TILE_SIZE+perso.y,32,32};
    SDL_RenderTexture(renderer, item->texture, NULL, &dest);
}


void detruire_item(t_Item ** item){
    SDL_DestroyTexture((*item)->texture);
    free(*item);
    *item = NULL;
}
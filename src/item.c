#include "main.h"





t_Item * init_item(typeItem type,SDL_Renderer * renderer,float x,float y){
    t_Item * item = malloc(sizeof(t_Item));
    item->texture = NULL;
    item->type =  type;
    item->x = x;
    item->y = y;
    switch (type){
        case BOIS: item->texture = IMG_LoadTexture(renderer,"assets/UI/bois.png"); break;
        case VIANDE: item->texture = IMG_LoadTexture(renderer,"assets/UI/viande.png");
        break;
    }
    if(item->texture == NULL){
        SDL_Log("init item fail ");
        free(item);
        return NULL;
    }    
    return item;
}

void afficher_item(t_Item * items[MAX_ITEMS],SDL_Renderer * renderer){
    for(int i = 0; i < index_item; i++) {
        if(items[i] != NULL) {
            SDL_FRect dest = {(items[i]->x)+perso.x,(items[i]->y)+perso.y,32,32};
            SDL_RenderTexture(renderer, items[i]->texture, NULL, &dest);
        }
    }
}


void detruire_item(t_Item ** item){
    SDL_DestroyTexture((*item)->texture);
    free(*item);
    *item = NULL;
}

void ramasser_item(t_Item * item){
    if(item != NULL){
        //ajouter_item_inventaire(item);
        printf("Item ramassé\n");
    }
}
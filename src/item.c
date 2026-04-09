#include "headers/main.h"





t_Item * init_item(typeItem type,SDL_Renderer * renderer,float x,float y){
    t_Item * item = malloc(sizeof(t_Item));
    item->texture = NULL;
    item->type =  type;
    item->x = x;
    item->y = y;
    switch (type){
        case PIECE: item->texture = IMG_LoadTexture(renderer,"assets/UI/piece.png");if(item->texture == NULL){ SDL_Log("Erreur chargement piece : %s", SDL_GetError()); } break;
        case VIANDE: item->texture = IMG_LoadTexture(renderer,"assets/UI/viande.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement viande : %s", SDL_GetError()); } break;
        case MARTEAU: item->texture = IMG_LoadTexture(renderer,"assets/UI/marteau.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement marteau : %s", SDL_GetError()); } break;
        case SOIN: item->texture = IMG_LoadTexture(renderer,"assets/UI/kit_soin.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement kit_soin : %s", SDL_GetError()); } break;
        case BRIQUET: item->texture = IMG_LoadTexture(renderer,"assets/UI/briquet.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement briquet : %s", SDL_GetError()); } break;
        case BOIS: item->texture = IMG_LoadTexture(renderer,"assets/UI/bois.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement bois : %s", SDL_GetError()); } break;
        case FEUDECAMP: item->texture = IMG_LoadTexture(renderer,"assets/UI/feu_de_camp.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement feu de camp : %s", SDL_GetError()); } break;
        case VIANDECUITE: item->texture = IMG_LoadTexture(renderer,"assets/UI/viande_cuite.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement viande cuite : %s", SDL_GetError()); } break;
    case ENGRENAGE: item->texture = IMG_LoadTexture(renderer,"assets/UI/engrenage.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement engrenage : %s", SDL_GetError()); } break;
    case CLE: item->texture = IMG_LoadTexture(renderer,"assets/tileset/V2/key.png"); if(item->texture == NULL){ SDL_Log("Erreur chargement cle : %s", SDL_GetError()); } break;
        default: SDL_Log("Type d'item inconnu : %d", type); break;
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
            if(items[i]->type == FEUDECAMP || items[i]->type == CLE){
                SDL_FRect dest = {(items[i]->x)+perso.x,(items[i]->y)+perso.y,64,64};
                SDL_RenderTexture(renderer, items[i]->texture, NULL, &dest);
                continue;
            }else{
                SDL_FRect dest = {(items[i]->x)+perso.x,(items[i]->y)+perso.y,32,32};
                SDL_RenderTexture(renderer, items[i]->texture, NULL, &dest);
            }
            
        }
    }
}


void detruire_item(t_Item ** item){
    SDL_DestroyTexture((*item)->texture);
    free(*item);
    *item = NULL;
}

void detruire_tout_item(t_Item * items[MAX_ITEMS]){
    for(int i = 0; i < index_item; i++) {
        if(items[i] != NULL) {
            detruire_item(&items[i]);
        }
    }
}

void ramasser_item(t_Item * item, SDL_Renderer * renderer, t_case * hotbar[HOTBAR_SIZE]){
    if(item != NULL){
        ajouter_item_hotbar(hotbar,item, renderer);
    }
}
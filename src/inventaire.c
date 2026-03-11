#include "headers/main.h"

// -1 = rien de sélectionné
// de 0 a INVENTAIRE_SIZE-1 = slot inventaire
// de INVENTAIRE_SIZE a INVENTAIRE_SIZE+HOTBAR_SIZE-1 = slot hotbar
static int slot_selectionne = -1;



void afficher_hotbar(t_case * hotbar[HOTBAR_SIZE], SDL_Renderer *renderer) {
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 16);
    SDL_Texture * texture = IMG_LoadTexture(renderer, "assets/UI/slot_inventaire.png");

    for (int i = 0; i < HOTBAR_SIZE; i++) {
        SDL_FRect dest = {340.0f + i * 70, 720, 60, 60};
        SDL_RenderTexture(renderer, texture, NULL, &dest);

        if (hotbar[i] != NULL && hotbar[i]->item != NULL) {
            SDL_FRect item_dest = {340.0f + i * 70 + (60 - 35) / 2.0f, 720 + (60 - 35) / 2.0f, 35, 35};
            SDL_RenderTexture(renderer, hotbar[i]->item->texture, NULL, &item_dest);

            if (font && hotbar[i]->quantiter > 0) {
                char quantiter_text[8];
                SDL_snprintf(quantiter_text, sizeof(quantiter_text), "%d", hotbar[i]->quantiter);
                SDL_Color blanc = {255, 255, 255, 255};
                SDL_Surface *surf = TTF_RenderText_Solid(font, quantiter_text, strlen(quantiter_text), blanc);
                if (surf) {
                    SDL_Texture *tex_qte = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_FRect rect_qte = {
                        340.0f + i * 70 + 60 - surf->w - 2,
                        720 + 60 - surf->h - 2,
                        (float)surf->w,
                        (float)surf->h
                    };
                    SDL_RenderTexture(renderer, tex_qte, NULL, &rect_qte);
                    SDL_DestroyTexture(tex_qte);
                    SDL_DestroySurface(surf);
                }
            }
        }
    }
    if (texture) SDL_DestroyTexture(texture);
    if (font) TTF_CloseFont(font);
}


void ajouter_item_hotbar(t_case *hotbar[HOTBAR_SIZE], t_Item *item, SDL_Renderer *renderer) {
    //ajouter +1 a quantiter quands il est deja dans la hotbar
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        if(hotbar[i] != NULL && hotbar[i]->item != NULL && hotbar[i]->item->type == item->type && hotbar[i]->stackable) {
            hotbar[i]->quantiter++;
            return;
        }
    }
    
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        if (hotbar[i] == NULL) {
            hotbar[i] = malloc(sizeof(t_case));
            if (!hotbar[i]) return;

            
            t_Item *copie = malloc(sizeof(t_Item));
            if (!copie) { 
                free(hotbar[i]); hotbar[i] = NULL; return; 
            }

            copie->type    = item->type;
            copie->texture = item->texture;
            copie->x       = 0;
            copie->y       = 0;

            hotbar[i]->item      = copie;
            hotbar[i]->quantiter = 1;
            hotbar[i]->stackable = 1;
            hotbar[i]->x = 0; hotbar[i]->y = 0;
            hotbar[i]->w = 0; hotbar[i]->h = 0;
            return;
        }
    }

}


void afficher_inventaire(t_case *inventaire[INVENTAIRE_SIZE], SDL_Renderer *renderer) {
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 16);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect fond = {0, 0, 1000, 810};
    SDL_RenderFillRect(renderer, &fond);

    // Panneau de l'inventaire centré
    float panneaux_w = INVENTAIRE_COLS * 70.0f + 20.0f;
    float panneaux_h = INVENTAIRE_ROWS * 70.0f + 60.0f;
    float panneau_x = (1000.0f - panneaux_w) / 2.0f;
    float panneau_y = (800.0f  - panneaux_h) / 2.0f;

    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 240);
    SDL_FRect panneau = {panneau_x, panneau_y, panneaux_w, panneaux_h};
    SDL_RenderFillRect(renderer, &panneau);

    // Bordure du panneau
    SDL_SetRenderDrawColor(renderer, 150, 150, 200, 255);
    SDL_RenderRect(renderer, &panneau);

    // Titre
    if (font) {
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Surface *surf_titre = TTF_RenderText_Solid(font, "Inventaire", strlen("Inventaire"), blanc);
        if (surf_titre) {
            SDL_Texture *tex_titre = SDL_CreateTextureFromSurface(renderer, surf_titre);
            SDL_FRect rect_titre = {
                panneau_x + (panneaux_w - surf_titre->w) / 2.0f,
                panneau_y + 10.0f,
                (float)surf_titre->w,
                (float)surf_titre->h
            };
            SDL_RenderTexture(renderer, tex_titre, NULL, &rect_titre);
            SDL_DestroyTexture(tex_titre);
            SDL_DestroySurface(surf_titre);
        }
    }

    SDL_Texture *slot_tex = IMG_LoadTexture(renderer, "assets/UI/slot_inventaire.png");

    for (int row = 0; row < INVENTAIRE_ROWS; row++) {
        for (int col = 0; col < INVENTAIRE_COLS; col++) {
            int i = row * INVENTAIRE_COLS + col;

            float sx = panneau_x + 10.0f + col * 70.0f;
            float sy = panneau_y + 50.0f + row * 70.0f;

            // Slot
            SDL_FRect slot_rect = {sx, sy, 60.0f, 60.0f};
            if (slot_tex)
                SDL_RenderTexture(renderer, slot_tex, NULL, &slot_rect);
            

            // Item + quantité
            if (inventaire[i] != NULL && inventaire[i]->item != NULL) {
                SDL_FRect item_rect = {
                    sx + (60.0f - 35.0f) / 2.0f,
                    sy + (60.0f - 35.0f) / 2.0f,
                    35.0f, 35.0f
                };
                SDL_RenderTexture(renderer, inventaire[i]->item->texture, NULL, &item_rect);

                if (font && inventaire[i]->quantiter > 1) {
                    char quantiter_text[8];
                    SDL_snprintf(quantiter_text, sizeof(quantiter_text), "%d", inventaire[i]->quantiter);
                    SDL_Color blanc = {255, 255, 255, 255};
                    SDL_Surface *surf = TTF_RenderText_Solid(font, quantiter_text, strlen(quantiter_text), blanc);
                    if (surf) {
                        SDL_Texture *tex_qte = SDL_CreateTextureFromSurface(renderer, surf);
                        SDL_FRect rect_qte = {
                            sx + 60.0f - surf->w - 2,
                            sy + 60.0f - surf->h - 2,
                            (float)surf->w,
                            (float)surf->h
                        };
                        SDL_RenderTexture(renderer, tex_qte, NULL, &rect_qte);
                        SDL_DestroyTexture(tex_qte);
                        SDL_DestroySurface(surf);
                    }
                }
                if (inventaire[i] != NULL && inventaire[i]->verrouille) {
                    SDL_Texture *lock_tex = IMG_LoadTexture(renderer, "assets/UI/lock.png");
                    if (lock_tex) {
                        SDL_FRect lock_rect = {sx + 2, sy + 2, 20, 20};
                        SDL_RenderTexture(renderer, lock_tex, NULL, &lock_rect);
                        SDL_DestroyTexture(lock_tex);
                    }
                }
            }
        }
    }

    if (slot_tex) SDL_DestroyTexture(slot_tex);
    if (font) TTF_CloseFont(font);
}



void ajouter_item_inventaire(t_case *inventaire[INVENTAIRE_SIZE], t_Item *item) {
    for (int i = 0; i < INVENTAIRE_SIZE; i++) {
        if (inventaire[i] != NULL && inventaire[i]->item != NULL &&
            inventaire[i]->item->type == item->type) {
            inventaire[i]->quantiter++;
            return;
        }
    }

    for (int i = 0; i < INVENTAIRE_SIZE; i++) {
        if (inventaire[i] == NULL) {
            inventaire[i] = malloc(sizeof(t_case));
            if (!inventaire[i]) return;

            t_Item *copie = malloc(sizeof(t_Item));
            if (!copie) { free(inventaire[i]); inventaire[i] = NULL; return; }
            copie->type    = item->type;
            copie->texture = item->texture;
            copie->x = 0;
            copie->y = 0;

            inventaire[i]->item      = copie;
            inventaire[i]->quantiter = 1;
            inventaire[i]->stackable = 1;
            inventaire[i]->x = 0; inventaire[i]->y = 0;
            inventaire[i]->w = 0; inventaire[i]->h = 0;
            return;
        }
    }
}

void gerer_clic_inventaire(t_case *inventaire[INVENTAIRE_SIZE], t_case *hotbar[HOTBAR_SIZE], SDL_Event *event){

    if (event->type != SDL_EVENT_MOUSE_BUTTON_DOWN || event->button.button != SDL_BUTTON_LEFT)
        return;

    float mx = event->button.x;
    float my = event->button.y;

    // Calcul position panneau inventaire (même que afficher_inventaire)
    float panneau_w = INVENTAIRE_COLS * 70.0f + 20.0f;
    float panneau_h = INVENTAIRE_ROWS * 70.0f + 60.0f;
    float panneau_x = (1000.0f - panneau_w) / 2.0f;
    float panneau_y = (800.0f  - panneau_h) / 2.0f;

    int clic_slot = -1;

    // Vérifier clic sur inventaire
    for (int row = 0; row < INVENTAIRE_ROWS; row++) {
        for (int col = 0; col < INVENTAIRE_COLS; col++) {
            float sx = panneau_x + 10.0f + col * 70.0f;
            float sy = panneau_y + 50.0f + row * 70.0f;
            if (mx >= sx && mx <= sx + 60 && my >= sy && my <= sy + 60) {
                clic_slot = row * INVENTAIRE_COLS + col;
            }
        }
    }

    // Vérifier clic sur hotbar
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        float sx = 340.0f + i * 70;
        float sy = 720.0f;
        if (mx >= sx && mx <= sx + 60 && my >= sy && my <= sy + 60) {
            clic_slot = INVENTAIRE_SIZE + i;
        }
    }

    if (clic_slot == -1) {
        // Clic en dehors : désélectionner
        slot_selectionne = -1;
        return;
    }

    if (slot_selectionne == -1) {
        // Aucun slot sélectionné : sélectionner ce slot
        slot_selectionne = clic_slot;
        return;
    }

    if (slot_selectionne == clic_slot) {
        // Clic sur le même slot : désélectionner
        slot_selectionne = -1;
        return;
    }

    t_case **slot_a = NULL;
    t_case **slot_b = NULL;

    if (slot_selectionne < INVENTAIRE_SIZE)
        slot_a = &inventaire[slot_selectionne];
    else
        slot_a = &hotbar[slot_selectionne - INVENTAIRE_SIZE];

    if (clic_slot < INVENTAIRE_SIZE)
        slot_b = &inventaire[clic_slot];
    else
        slot_b = &hotbar[clic_slot - INVENTAIRE_SIZE];

    t_case *tmp = *slot_a;
    *slot_a = *slot_b;
    *slot_b = tmp;

    slot_selectionne = -1;
}

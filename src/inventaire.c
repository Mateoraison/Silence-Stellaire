#include "headers/main.h"

// -1 = rien de sélectionné
// de 0 a INVENTAIRE_SIZE-1 = slot inventaire
// de INVENTAIRE_SIZE a INVENTAIRE_SIZE+HOTBAR_SIZE-1 = slot hotbar
static int slot_selectionne = -1;



void afficher_hotbar(t_case * hotbar[HOTBAR_SIZE], SDL_Renderer *renderer) {
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 16);
    SDL_Texture * texture = IMG_LoadTexture(renderer, "assets/UI/slot_inventaire.png");
    float hotbar_x = screen_center_x() - ((HOTBAR_SIZE * 70.0f - 10.0f) * 0.5f);
    float hotbar_y = screen_heightf() - 80.0f;

    for (int i = 0; i < HOTBAR_SIZE; i++) {
        SDL_FRect dest = {hotbar_x + i * 70.0f, hotbar_y, 60, 60};
        SDL_RenderTexture(renderer, texture, NULL, &dest);

        if (hotbar[i] != NULL && hotbar[i]->item != NULL) {
            SDL_FRect item_dest = {hotbar_x + i * 70.0f + (60.0f - 35.0f) / 2.0f, hotbar_y + (60.0f - 35.0f) / 2.0f, 35, 35};
            SDL_RenderTexture(renderer, hotbar[i]->item->texture, NULL, &item_dest);

            if (font && hotbar[i]->quantiter > 0) {
                char quantiter_text[8];
                SDL_snprintf(quantiter_text, sizeof(quantiter_text), "%d", hotbar[i]->quantiter);
                SDL_Color blanc = {255, 255, 255, 255};
                SDL_Surface *surf = TTF_RenderText_Solid(font, quantiter_text, strlen(quantiter_text), blanc);
                if (surf) {
                    SDL_Texture *tex_qte = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_FRect rect_qte = {
                        hotbar_x + i * 70.0f + 60.0f - surf->w - 2.0f,
                        hotbar_y + 60.0f - surf->h - 2.0f,
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
    // Evite le warning
    (void)renderer;

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


void afficher_inventaire(t_case *inventaire[], SDL_Renderer *renderer, int inventaire_size, int inventaire_cols, int inventaire_rows) {
    // Evite les warnings
    (void)inventaire_size;

    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 16);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect fond = {0.0f, 0.0f, screen_widthf(), screen_heightf()};
    SDL_RenderFillRect(renderer, &fond);

    // Panneau de l'inventaire centré
    float panneaux_w = inventaire_cols * 70.0f + 20.0f;
    float panneaux_h = inventaire_rows * 70.0f + 60.0f;
    float panneau_x = (screen_widthf() - panneaux_w) / 2.0f;
    float panneau_y = (screen_heightf() - panneaux_h) / 2.0f;

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

    for (int row = 0; row < inventaire_rows; row++) {
        for (int col = 0; col < inventaire_cols; col++) {
            int i = row * inventaire_cols + col;

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
            }else{
                SDL_Texture * texture_lock = IMG_LoadTexture(renderer, "assets/UI/locker.png");
                SDL_RenderTexture(renderer, texture_lock, NULL, &slot_rect);
                SDL_DestroyTexture(texture_lock);
            }
        }
    }

    if (slot_tex) SDL_DestroyTexture(slot_tex);
    if (font) TTF_CloseFont(font);
}



void ajouter_item_inventaire(t_case **inventaire, int inventaire_size, t_Item *item) {
    for (int i = 0; i < inventaire_size; i++) {
        if (inventaire[i] != NULL && inventaire[i]->item != NULL &&
            inventaire[i]->item->type == item->type) {
            inventaire[i]->quantiter++;
            return;
        }
    }

    for (int i = 0; i < inventaire_size; i++) {
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

void gerer_clic_inventaire(t_case **inventaire, t_case **hotbar, SDL_Event *event, int inventaire_size, int inventaire_cols, int inventaire_rows) {
    if (event->type != SDL_EVENT_MOUSE_BUTTON_DOWN || event->button.button != SDL_BUTTON_LEFT)
        return;

    float mx = event->button.x;
    float my = event->button.y;

    // Calcul position panneau inventaire (même que afficher_inventaire)
    float panneau_w = inventaire_cols * 70.0f + 20.0f;
    float panneau_h = inventaire_rows * 70.0f + 60.0f;
    float panneau_x = (screen_widthf() - panneau_w) / 2.0f;
    float panneau_y = (screen_heightf() - panneau_h) / 2.0f;
    float hotbar_x = screen_center_x() - ((HOTBAR_SIZE * 70.0f - 10.0f) * 0.5f);
    float hotbar_y = screen_heightf() - 80.0f;

    int clic_slot = -1;

    // Vérifier clic sur inventaire
    for (int row = 0; row < inventaire_rows; row++) {
        for (int col = 0; col < inventaire_cols; col++) {
            int i = row * inventaire_cols + col;
            if (i >= inventaire_size) continue;
            float sx = panneau_x + 10.0f + col * 70.0f;
            float sy = panneau_y + 50.0f + row * 70.0f;
            if (mx >= sx && mx <= sx + 60 && my >= sy && my <= sy + 60) {
                clic_slot = i;
            }
        }
    }

    // Vérifier clic sur hotbar
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        float sx = hotbar_x + i * 70.0f;
        float sy = hotbar_y;
        if (mx >= sx && mx <= sx + 60 && my >= sy && my <= sy + 60) {
            clic_slot = inventaire_size + i;
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

    if (slot_selectionne < inventaire_size)
        slot_a = &inventaire[slot_selectionne];
    else
        slot_a = &hotbar[slot_selectionne - inventaire_size];

    if (clic_slot < inventaire_size)
        slot_b = &inventaire[clic_slot];
    else
        slot_b = &hotbar[clic_slot - inventaire_size];

    t_case *tmp = *slot_a;
    *slot_a = *slot_b;
    *slot_b = tmp;

    slot_selectionne = -1;
}

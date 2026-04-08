#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "headers/main.h"
#include <stdbool.h>
#include <stdio.h>

int afficher_perso(SDL_Renderer *renderer) {
    const char* dirs[] = {"south", "north", "west", "east"};
    const char* dir_name = dirs[perso.direction];
    
    char full_path[512];
    sprintf(full_path, "assets/personnage/Astronaute/animations/walk/%s/frame_00%d.png", dir_name, animation_frame);

    SDL_Texture *perso_texture = IMG_LoadTexture(renderer, full_path);
    if (!perso_texture) {
        SDL_Log("Erreur %s : %s", full_path, SDL_GetError());
        return 1;
    }

    SDL_FRect src = {0, 0, 64, 64};
    SDL_FRect dest = {screen_center_x(), screen_center_y(), 90, 90};

    SDL_RenderTexture(renderer, perso_texture, &src, &dest);
    SDL_DestroyTexture(perso_texture);
    return 0;
}



int deplacer_perso(float delta_time) {
    const bool *keys = SDL_GetKeyboardState(NULL);

    float vitesse = 200.0f + vitesse_bonus;

    perso_bouge = false;

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_Z]) {
        perso.y += vitesse * delta_time;
        perso.direction = 1;
        perso_bouge = true;
    } else if (keys[SDL_SCANCODE_S]) {
        perso.y -= vitesse * delta_time;
        perso.direction = 0;
        perso_bouge = true;
    } else if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_Q]) {
        perso.x += vitesse * delta_time;
        perso.direction = 2;
        perso_bouge = true;
    } else if (keys[SDL_SCANCODE_D]) {
        perso.x -= vitesse * delta_time;
        perso.direction = 3;
        perso_bouge = true;
    }
    
    return 0;
}


void update_animation() {
    Uint32 maintenant = SDL_GetTicks();

    if (perso_bouge) {
        if (animation_timer == 0) animation_timer = maintenant;
        if (maintenant - animation_timer >= FRAME_DUREE) {
            animation_frame = (animation_frame + 1) % 5;
            animation_timer = maintenant;
        }
    } else {
        animation_frame = 0;
        animation_timer = 0;
    }
}

int test_collision(int x, int y, t_tile map[W_MAP][H_MAP], int est_mob, SDL_Rect hitbox) {
    /**
     * Cette fonction teste si la tile à la position (x, y) est une tile d'un type avec collision.
     */

    if(Planete_actuelle == 2) {
        switch (map[x][y].type){
            case terreB : {
                SDL_Rect hitbox_eau = {x*DISPLAY_TILE_SIZE , y*DISPLAY_TILE_SIZE + 20, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
                if(SDL_HasRectIntersection(&hitbox_eau, &hitbox)) {
                    return 1;
                }
                break;
            }
            case terreH : {
                SDL_Rect hitbox_eau = {x*DISPLAY_TILE_SIZE, y*DISPLAY_TILE_SIZE - 20, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
                if(SDL_HasRectIntersection(&hitbox_eau, &hitbox)) {
                    return 1;
                }
                break;
            }
            case terreG : {
                SDL_Rect hitbox_eau = {x*DISPLAY_TILE_SIZE - 20, y*DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
                if(SDL_HasRectIntersection(&hitbox_eau, &hitbox)) {
                    return 1;
                }
                break;
            }
            case terreD : {
                SDL_Rect hitbox_eau = {x*DISPLAY_TILE_SIZE + 20, y*DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
                if(SDL_HasRectIntersection(&hitbox_eau, &hitbox)) {
                    return 1;
                }
                break;
            }
            case terreCBD :
            case terreCBG :
            case terreCHD :
            case terreCHG : {
                SDL_Rect hitbox_terre = {x*DISPLAY_TILE_SIZE, y*DISPLAY_TILE_SIZE, 80, 80};
                if(SDL_HasRectIntersection(&hitbox_terre, &hitbox)) {
                    return 1;
                }
                break;
            }
            case cterreHBG : {
                SDL_Rect hitbox_terre = {x*DISPLAY_TILE_SIZE, y*DISPLAY_TILE_SIZE, 50, 50};
                if(SDL_HasRectIntersection(&hitbox_terre, &hitbox)) {
                    return 1;
                }
                break;
            }
            case cterreBBG : {
                SDL_Rect hitbox_terre = {x*DISPLAY_TILE_SIZE , y*DISPLAY_TILE_SIZE + 20, 80, 90};
                if(SDL_HasRectIntersection(&hitbox_terre, &hitbox)) {
                    return 1;
                }
                break;
            }
            case cterreHHD : {
                SDL_Rect hitbox_terre = {x*DISPLAY_TILE_SIZE + 20, y*DISPLAY_TILE_SIZE, 50, 50};
                if(SDL_HasRectIntersection(&hitbox_terre, &hitbox)) {
                    return 1;
                }
                break;
            }
            case cterrBHD : {
                SDL_Rect hitbox_terre = {x*DISPLAY_TILE_SIZE + 20, y*DISPLAY_TILE_SIZE, 50, 90};
                if(SDL_HasRectIntersection(&hitbox_terre, &hitbox)) {
                    return 1;
                }
                break;
            }
            default: break;
        }
    }

    switch (map[x][y].type)
    {
            case vide:
            case eau: {
                SDL_Rect hitbox_eau = {x*DISPLAY_TILE_SIZE, y*DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
                if(SDL_HasRectIntersection(&hitbox_eau, &hitbox)) {
                    return 1;
                }
                break;
            }
            case pierre: {
                SDL_Rect hitbox_pierre = {x*DISPLAY_TILE_SIZE + 8, y*DISPLAY_TILE_SIZE + 4, 84, 76};
                if(SDL_HasRectIntersection(&hitbox_pierre, &hitbox)) {
                    return 1;
                }
                break;
            }
            case arbrecoupe: {
                SDL_Rect hitbox_arbrecoupe = {x*DISPLAY_TILE_SIZE + 13, y*DISPLAY_TILE_SIZE + 8, 62, 51};
                if(SDL_HasRectIntersection(&hitbox_arbrecoupe, &hitbox)) {
                    return 1;
                }
                break;
            }
        case arbreEntier:
            if(map[x][y].type == arbreEntier && map[x-1][y].type == arbreEntier && map[x][y-1].type == arbreEntier && map[x][y+1].type == arbreEntier){
                x -= 1;
                y -= 1;
            }else if(map[x][y].type == arbreEntier && map[x+1][y].type == arbreEntier && map[x][y-1].type == arbreEntier && map[x][y+1].type == arbreEntier){
                y -= 1;
            }else if(map[x][y].type == arbreEntier && map[x+1][y].type == arbreEntier && map[x][y-1].type == arbreEntier && map[x][y+1].type != arbreEntier){
                y -= 2;
            }else if(map[x][y].type == arbreEntier && map[x-1][y].type == arbreEntier && map[x][y-1].type == arbreEntier && map[x][y+1].type != arbreEntier){
                x -= 1;
                y -= 2;
            }else if(map[x][y].type == arbreEntier && map[x-1][y].type == arbreEntier && map[x][y-1].type != arbreEntier && map[x][y+1].type == arbreEntier){
                x -= 1;
            }

            SDL_Rect hitbox_arbreEntier1 = {x*DISPLAY_TILE_SIZE + 59*1.4, y*DISPLAY_TILE_SIZE + 38*1.4, 13, 38};
            SDL_Rect hitbox_arbreEntier2 = {x*DISPLAY_TILE_SIZE + 48*1.4, y*DISPLAY_TILE_SIZE + 65*1.4, 48, 22};
            SDL_Rect hitbox_arbreEntier3 = {x*DISPLAY_TILE_SIZE + 39*1.4, y*DISPLAY_TILE_SIZE + 82*1.4, 70, 31};
            SDL_Rect hitbox_arbreEntier4 = {x*DISPLAY_TILE_SIZE + 33*1.4, y*DISPLAY_TILE_SIZE + 105*1.4, 91, 35};
            SDL_Rect hitbox_arbreEntier5 = {x*DISPLAY_TILE_SIZE + 23*1.4, y*DISPLAY_TILE_SIZE + 130*1.4, 115, 34};
            SDL_Rect hitbox_arbreEntier6 = {x*DISPLAY_TILE_SIZE + 34*1.4, y*DISPLAY_TILE_SIZE + 154*1.4, 87, 14};
            SDL_Rect hitbox_arbreEntier7 = {x*DISPLAY_TILE_SIZE + 50*1.4, y*DISPLAY_TILE_SIZE + 164*1.4, 42, 17};
            
            if(SDL_HasRectIntersection(&hitbox_arbreEntier1, &hitbox) || SDL_HasRectIntersection(&hitbox_arbreEntier2, &hitbox) || SDL_HasRectIntersection(&hitbox_arbreEntier3, &hitbox) || SDL_HasRectIntersection(&hitbox_arbreEntier4, &hitbox) || SDL_HasRectIntersection(&hitbox_arbreEntier5, &hitbox) || SDL_HasRectIntersection(&hitbox_arbreEntier6, &hitbox) || SDL_HasRectIntersection(&hitbox_arbreEntier7, &hitbox)) {
                return 1;
            }
            break;
        case feu:
            if (est_mob == 0 && perso.vie > 0 && perso.invincibiliter_timer == 0){
                jouer_son("assets/audio/dammage.wav", 0.5f);
                perso.vie -= 1;
                perso.invincibiliter_timer = SDL_GetTicks();
            }
            return 0;
        default:
            return 0;
    }
    return 0;

}

int afficher_stat(SDL_Renderer *renderer) {
    /**
     * Cette fonction affiche la vie et la faim du personnage à l'écran en utilisant des textures de coeur.
     */

    SDL_FRect srcC = {0, 0, 64, 64};
    SDL_FRect destC = {10.0f, 10.0f, 30, 30};

    SDL_FRect srcF = {0, 0, 64, 64};
    SDL_FRect destF = {10.0f, 50.0f, 30, 30};

    SDL_Texture * coeurP_texture = IMG_LoadTexture(renderer, "assets/UI/coeurP.png");
    SDL_Texture * coeurPVide_texture = IMG_LoadTexture(renderer, "assets/UI/coeurPVide.png");
    SDL_Texture * coeurM_texture = IMG_LoadTexture(renderer, "assets/UI/coeurM.png");

    SDL_Texture * faimP_texture = IMG_LoadTexture(renderer, "assets/UI/faimP.png");
    SDL_Texture * faimPVide_texture = IMG_LoadTexture(renderer, "assets/UI/faimPVide.png");
    SDL_Texture * faimM_texture = IMG_LoadTexture(renderer, "assets/UI/faimM.png");
    
    for (int i = 0; i < (perso.vie_max/2); i++) {
        SDL_RenderTexture(renderer, coeurPVide_texture, &srcC, &destC);
        destC.x += 35;
    }

    for (int i = 0; i < (perso.faim_max/2); i++) {
        SDL_RenderTexture(renderer, faimPVide_texture, &srcF, &destF);
        destF.x += 35;
    }

    destC = (SDL_FRect){10.0f , 10.0f, 30, 30};
    destF = (SDL_FRect){10.0f , 50.0f, 30, 30};

    for (int i = 0; i < (perso.vie/2); i++) {
        SDL_RenderTexture(renderer, coeurP_texture, &srcC, &destC);
        destC.x += 35; 
    }

    for (int i = 0; i < (perso.faim/2); i++) {
        SDL_RenderTexture(renderer, faimP_texture, &srcF, &destF);
        destF.x += 35; 
    }

    if (perso.vie % 2 == 1) {
        SDL_RenderTexture(renderer, coeurM_texture, &srcC, &destC);
    }

    if (perso.faim % 2 == 1) {
        SDL_RenderTexture(renderer, faimM_texture, &srcF, &destF);
    }

    SDL_DestroyTexture(coeurP_texture);
    SDL_DestroyTexture(coeurPVide_texture);
    SDL_DestroyTexture(coeurM_texture);
    SDL_DestroyTexture(faimP_texture);
    SDL_DestroyTexture(faimPVide_texture);
    SDL_DestroyTexture(faimM_texture);
    return 0;
}

void gerer_combat(SDL_Event event) {
    /**
     * Cette fonction gère le déclenchement du combat lors d'un clic gauche.
     */
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (!combat_en_cours) {
            combat_en_cours = true;
            combat_frame = 0;
            combat_timer = SDL_GetTicks();
        }
    }
}

void afficher_combat(SDL_Renderer *renderer) {
    /**
     * Cette fonction affiche l'animation du coup de poing.
     */
    if (!combat_en_cours) {
        return;
    }

    const char* dirs[] = {"south", "north", "west", "east"};
    const char* dir_name = dirs[perso.direction];

    char full_path[512];
    sprintf(full_path, "assets/personnage/Astronaute/animations/lead-jab/%s/frame_00%d.png", dir_name, combat_frame);

    SDL_Texture *combat_texture = IMG_LoadTexture(renderer, full_path);
    if (!combat_texture) {
        SDL_Log("Erreur %s : %s", full_path, SDL_GetError());
        return;
    }

    SDL_FRect src = {0, 0, 64, 64};
    SDL_FRect dest = {screen_center_x(), screen_center_y(), 90, 90};

    SDL_RenderTexture(renderer, combat_texture, &src, &dest);
    SDL_DestroyTexture(combat_texture);
}

void update_combat(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB], SDL_Renderer * renderer, t_Item * items[MAX_ITEMS]) {
    /**
     * Cette fonction met à jour l'état du combat et les frames d'animation.
     */
    if (!combat_en_cours) {
        return;
    }

    jouer_son("assets/audio/kick.mp3", 0.2f);

    Uint32 maintenant = SDL_GetTicks();

    if (maintenant - combat_timer >= FRAME_DUREE) {
        combat_frame++;
        combat_timer = maintenant;
        
        if (combat_frame >= 3) {
            combat_en_cours = false;
            combat_frame = 0;
            tester_collision_combat(map, mobs, renderer, items);
        }
    }
}

void tester_collision_combat(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB], SDL_Renderer * renderer, t_Item * items[MAX_ITEMS]) {
    /**
     * Cette fonction teste si un mob est touché par le coup de poing et enlève sa vie.
     * La zone d'attaque dépend de la direction du personnage.
     */

    const float char_screen_x = screen_center_x();
    const float char_screen_y = screen_center_y();
    const float char_w = DISPLAY_TILE_SIZE;
    const float char_h = DISPLAY_TILE_SIZE;

    float zone_attaque_w = DISPLAY_TILE_SIZE * 0.6f; 
    float zone_attaque_h = DISPLAY_TILE_SIZE * 0.5f; 
    float zone_attaque_x = char_screen_x + (char_w - zone_attaque_w) / 2.0f;
    float zone_attaque_y = char_screen_y + (char_h - zone_attaque_h) / 2.0f;

    switch (perso.direction) {
        case 0: /* sud */
            zone_attaque_x = char_screen_x + (char_w - zone_attaque_w) / 2.0f;
            zone_attaque_y = char_screen_y + char_h;
            break;
        case 1: /* nord */
            zone_attaque_x = char_screen_x + (char_w - zone_attaque_w) / 2.0f;
            zone_attaque_y = char_screen_y - zone_attaque_h;
            break;
        case 2: /* ouest */
            zone_attaque_x = char_screen_x - zone_attaque_w;
            zone_attaque_y = char_screen_y + (char_h - zone_attaque_h) / 2.0f;
            break;
        case 3: /* est */
            zone_attaque_x = char_screen_x + char_w;
            zone_attaque_y = char_screen_y + (char_h - zone_attaque_h) / 2.0f;
            break;
        default:
            break;
    }

    // Tester les collisions avec les mobs
    int mob_touche_lim = 5;
    for (int i = 0; mobs[i] != NULL; i++) {
        float mob_x = mobs[i]->x + perso.x;
        float mob_y = mobs[i]->y + perso.y;
        float mob_w = mobs[i]->largeur * DISPLAY_TILE_SIZE;
        float mob_h = mobs[i]->hauteur * DISPLAY_TILE_SIZE;
        if(mob_touche_lim > 0){
            if (zone_attaque_x < mob_x + mob_w &&
                zone_attaque_x + zone_attaque_w > mob_x &&
                zone_attaque_y < mob_y + mob_h &&
                zone_attaque_y + zone_attaque_h > mob_y) {
                mobs[i]->vie--;
                mob_touche_lim--;

                if (mobs[i]->vie <= 0) {
                    if(mobs[i]->id == 1){
                        float x = mobs[i]->x;
                        float y = mobs[i]->y;
                        int drop = mobs[i]->drop_chance;
                        schedule_respawn(1, 5000);
                        detruire_un_mob(mobs[i]);
                        if(index_item < MAX_ITEMS && rand()%100 < drop) {
                            t_Item * item = init_item(VIANDE, renderer, x, y);
                            if(item != NULL) items[index_item++] = item;
                        }
                    }
                    else if(mobs[i]->id == 2){
                        float x = mobs[i]->x;
                        float y = mobs[i]->y;
                        int drop = mobs[i]->drop_chance;
                        schedule_respawn(2, 5000);
                        detruire_un_mob(mobs[i]);
                        if(index_item < MAX_ITEMS && rand()%100 < drop) {
                            t_Item * item = init_item(PIECE, renderer, x, y);
                            if(item != NULL) items[index_item++] = item;
                        }
                    }
                    int j = i;
                    while(mobs[j+1] != NULL) {
                        mobs[j] = mobs[j+1];
                        j++;
                    }
                    mobs[j] = NULL;
                } else {

                    float recul = 25.0f;
                    float dir_x = 0.0f, dir_y = 0.0f;
                    switch (perso.direction) {
                        case 0: /* south */ dir_y =  1.0f; break;
                        case 1: /* north */ dir_y = -1.0f; break;
                        case 2: /* west  */ dir_x = -1.0f; break;
                        case 3: /* east  */ dir_x =  1.0f; break;
                    }

                    float best_recul = 0.0f;
                    for (float push = recul; push > 0.0f; push -= 1.0f) {
                        float nx = mobs[i]->x + dir_x * push;
                        float ny = mobs[i]->y + dir_y * push;
                        int tile_x = (int)((nx + mobs[i]->largeur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
                        int tile_y = (int)((ny + mobs[i]->hauteur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
                        if (!test_collision(tile_x, tile_y, map, 1, (SDL_Rect){nx, ny, mobs[i]->largeur * DISPLAY_TILE_SIZE, mobs[i]->hauteur * DISPLAY_TILE_SIZE})) {
                            best_recul = push;
                            break;
                        }
                    }

                    mobs[i]->x += dir_x * best_recul;
                    mobs[i]->y += dir_y * best_recul;
                }
            }
        }
    }
}


void possible_ramasser_item(t_Item * items[MAX_ITEMS], SDL_Renderer * renderer, t_case * hotbar[HOTBAR_SIZE]) {
    SDL_Rect rect_perso = {(int)screen_center_x(), (int)screen_center_y(), 40, 60};

    for (int i = 0; i < index_item; i++) {
        if (items[i] != NULL && items[i]->type != FEUDECAMP) {
            SDL_Rect rect_item = {(int)(items[i]->x + perso.x), (int)(items[i]->y + perso.y), 32, 32};

            if (SDL_HasRectIntersection(&rect_perso, &rect_item)) {
                TTF_Font *font_text = TTF_OpenFont("assets/police.ttf", 24);
                if (font_text) {
                    char text_touche[64];
                    if(items[i]->type == BOIS)SDL_snprintf(text_touche, sizeof(text_touche), " Utiliser briquet pour faire feu de camp");
                    else SDL_snprintf(text_touche, sizeof(text_touche), "Appuyez sur E pour ramasser");
                    SDL_Color blanc = {255, 255, 255, 255};
                    SDL_Surface *surf_text = TTF_RenderText_Solid(font_text, text_touche, strlen(text_touche), blanc);
                    if (surf_text) {
                        SDL_Texture *tex_text = SDL_CreateTextureFromSurface(renderer, surf_text);
                        SDL_FRect rect_text = {
                            screen_center_x() - ((float)surf_text->w * 0.5f),
                            screen_center_y() - 50.0f,
                            (float)surf_text->w,
                            (float)surf_text->h
                        };
                        SDL_RenderTexture(renderer, tex_text, NULL, &rect_text);
                        SDL_DestroyTexture(tex_text);
                        SDL_DestroySurface(surf_text);
                    }
                    TTF_CloseFont(font_text);
                }
                const bool *keys = SDL_GetKeyboardState(NULL);
                if (keys[SDL_SCANCODE_E]) {
                    // Les pièces convertissent directement en argent
                    if (items[i]->type == PIECE) {
                        argent++;
                    } else {
                        if (Planete_actuelle == 3 && items[i]->type == ENGRENAGE) {
                            g_planete3_engrenage_recupere = true;
                        }
                        ajouter_item_hotbar(hotbar, items[i], renderer);
                    }

                    free(items[i]);
                    items[i] = NULL;

                    for (int j = i; j < index_item - 1; j++){
                        items[j] = items[j + 1];
                    }
                    items[index_item - 1] = NULL;
                    index_item--;
                    i--;
                }
            }
        }
    }
}
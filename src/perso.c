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
    SDL_FRect dest = {500.0f , 400.0f, 90, 90};

    SDL_RenderTexture(renderer, perso_texture, &src, &dest);
    SDL_DestroyTexture(perso_texture);
    return 0;
}



int deplacer_perso(float delta_time) {
    const bool *keys = SDL_GetKeyboardState(NULL);

    float vitesse = 200.0f;

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

    switch (map[x][y].type)
    {
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
                perso.vie -= 1;
                perso.invincibiliter_timer = SDL_GetTicks();
            }
            return 0;
        default:
            return 0;
    }
    return 0;

}

int afficher_vie(SDL_Renderer *renderer) {
    /**
     * Cette fonction affiche la vie du personnage à l'écran en utilisant des textures de coeur.
     */

    SDL_FRect src = {0, 0, 64, 64};
    SDL_FRect dest = {10.0f, 10.0f, 30, 30};

    SDL_Texture * coeurP_texture = IMG_LoadTexture(renderer, "assets/UI/coeurP.png");
    SDL_Texture * coeurPVide_texture = IMG_LoadTexture(renderer, "assets/UI/coeurPVide.png");
    SDL_Texture * coeurM_texture = IMG_LoadTexture(renderer, "assets/UI/coeurM.png");
    
    for (int i = 0; i < (perso.vie_max/2); i++) {
        SDL_RenderTexture(renderer, coeurPVide_texture, &src, &dest);
        dest.x += 35;
    }

    dest = (SDL_FRect){10.0f , 10.0f, 30, 30};

    for (int i = 0; i < (perso.vie/2); i++) {
        SDL_RenderTexture(renderer, coeurP_texture, &src, &dest);
        dest.x += 35; 
    }

    if (perso.vie % 2 == 1) {
        SDL_RenderTexture(renderer, coeurM_texture, &src, &dest);
    }

    SDL_DestroyTexture(coeurP_texture);
    SDL_DestroyTexture(coeurPVide_texture);
    SDL_DestroyTexture(coeurM_texture);
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
    SDL_FRect dest = {500.0f , 400.0f, 90, 90};

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

    const float char_screen_x = 500.0f;
    const float char_screen_y = 400.0f;
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
                    if(mobs[i]->id == 1){ // Si c'est un mouton, drop du viande
                        float x, y;
                        x = mobs[i]->x;
                        y = mobs[i]->y;
                        detruire_un_mob(mobs[i]);
                        if(index_item < MAX_ITEMS && rand()%100 < mobs[i]->drop_chance) {
                            t_Item * item = init_item(VIANDE, renderer, x, y);
                            if(item != NULL) {
                                items[index_item++] = item;
                            }
                        }
                    }
                    else if(mobs[i]->id == 2){ // Si c'est un pawn, drop de la piece
                        float x, y;
                        x = mobs[i]->x;
                        y = mobs[i]->y;
                        detruire_un_mob(mobs[i]);
                        if(index_item < MAX_ITEMS && rand()%100 < mobs[i]->drop_chance) {
                            t_Item * item = init_item(PIECE, renderer, x, y);
                            if(item != NULL) {
                                items[index_item++] = item;
                            }
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
    SDL_Rect rect_perso = {500, 400, 40, 60};

    for (int i = 0; i < index_item; i++) {
        if (items[i] != NULL) {
            SDL_Rect rect_item = {(int)(items[i]->x + perso.x), (int)(items[i]->y + perso.y), 32, 32};

            if (SDL_HasRectIntersection(&rect_perso, &rect_item)) {
                const bool *keys = SDL_GetKeyboardState(NULL);
                if (keys[SDL_SCANCODE_E]) {
                    ajouter_item_hotbar(hotbar, items[i], renderer);

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
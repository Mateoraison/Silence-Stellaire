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



int deplacer_perso(SDL_Event event) {
    if (event.type != SDL_EVENT_KEY_DOWN && event.type != SDL_EVENT_KEY_UP) return 0;


    if (event.type == SDL_EVENT_KEY_DOWN) {
        perso_bouge = true;
        bouge_timer = SDL_GetTicks();
        switch (event.key.key) {
            case SDLK_Z :
            case SDLK_W : perso.y += 5;
            perso.texture = IMG_LoadTexture(SDL_GetRenderer(SDL_GetWindowFromID(event.key.windowID)), "assets/personnage/Astronaute/rotations/north.png");
            perso.direction = 1;
            break;

            case SDLK_S: perso.y -= 5; 
            perso.texture = IMG_LoadTexture(SDL_GetRenderer(SDL_GetWindowFromID(event.key.windowID)), "assets/personnage/Astronaute/rotations/south.png");
            perso.direction = 0;
            break;

            case SDLK_Q:
            case SDLK_A: perso.x += 5;
            perso.texture = IMG_LoadTexture(SDL_GetRenderer(SDL_GetWindowFromID(event.key.windowID)), "assets/personnage/Astronaute/rotations/west.png");
            perso.direction = 2;
            break;

            case SDLK_D: perso.x -= 5; 
            perso.texture = IMG_LoadTexture(SDL_GetRenderer(SDL_GetWindowFromID(event.key.windowID)), "assets/personnage/Astronaute/rotations/east.png");
            perso.direction = 3;
            break;

            default: break;
        }
    }else{
        perso_bouge = false;
    }
    
    return 0;
}


void update_animation() {
    Uint32 maintenant = SDL_GetTicks();
    
    if (perso_bouge && (maintenant - bouge_timer > FRAME_DUREE)) {
        perso_bouge = false;
    }
    
    if (perso_bouge) {
        if (maintenant - animation_timer >= FRAME_DUREE) {
            animation_frame++;
            if (animation_frame >= 5) animation_frame = 0;
            animation_timer = maintenant;
        }
    } else {
        animation_frame = 0;
    }
}

int test_collision(int x, int y, t_tile map[W_MAP][H_MAP], int est_mob) {
    /**
     * Cette fonction teste si la tile à la position (x, y) est une tile d'un type avec collision.
     */

    switch (map[x][y].type)
    {
    case eau:
    case pierre:
    case arbrecoupe:
    case arbreEntier:
        return 1;
    case feu:
        if (est_mob == 0 && perso.vie > 0 && perso.invincibiliter_timer == 0) {
            perso.vie -= 1;
            perso.invincibiliter_timer = SDL_GetTicks();
        }
        return 1;
    default:
        return 0;
    }

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

void update_combat() {
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
            tester_collision_combat();
        }
    }
}

void tester_collision_combat() {
    /**
     * Cette fonction teste si un mob est touché par le coup de poing et enlève sa vie.
     * La zone d'attaque dépend de la direction du personnage.
     */

    float zone_attaque_x = 500.0f;
    float zone_attaque_y = 400.0f;
    float zone_attaque_w = 90.0f;
    float zone_attaque_h = 90.0f;

    switch (perso.direction) {
        case 0: 
            zone_attaque_y += zone_attaque_h;
            zone_attaque_h = 60.0f;
            break;
        case 1: 
            zone_attaque_y -= 60.0f;
            zone_attaque_h = 60.0f;
            break;
        case 2: 
            zone_attaque_x -= 60.0f;
            zone_attaque_w = 60.0f;
            break;
        case 3:
            zone_attaque_x += zone_attaque_w;
            zone_attaque_w = 60.0f;
            break;
    }

    // Tester les collisions avec les mobs
    for (int i = 0; i < nb_mobs; i++) {
        float mob_x = mobs[i].x + perso.x;
        float mob_y = mobs[i].y + perso.y;
        float mob_w = mobs[i].largeur * DISPLAY_TILE_SIZE;
        float mob_h = mobs[i].hauteur * DISPLAY_TILE_SIZE;

        if (zone_attaque_x < mob_x + mob_w &&
            zone_attaque_x + zone_attaque_w > mob_x &&
            zone_attaque_y < mob_y + mob_h &&
            zone_attaque_y + zone_attaque_h > mob_y) {
            mobs[i].vie--;
            if (mobs[i].vie <= 0) {
                for (int j = i; j < nb_mobs - 1; j++) {
                    mobs[j] = mobs[j + 1];
                }
                nb_mobs--;
            } else {
                float recul = 25.0f;
                switch (perso.direction) {
                    case 0: 
                        mobs[i].y += recul;
                        break;
                    case 1: 
                        mobs[i].y -= recul;
                        break;
                    case 2: 
                        mobs[i].x -= recul;
                        break;
                    case 3: 
                        mobs[i].x += recul;
                        break;
                }
            }
        }
    }
}


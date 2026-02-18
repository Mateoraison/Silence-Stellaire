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

    // SDL_Color noir = {0, 0, 0, 255};
    // SDL_Color blanc = {255, 255, 255, 255};
    // SDL_Color gris_fonce = {50, 50, 50, 255};
    // SDL_RenderFillRect(renderer, &dest);

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

int test_collision(int x, int y, t_tile map[W_MAP][H_MAP]) {
    /**
     * Cette fonction teste si la tile à la position (x, y) est une tile d'un type avec collision.
     */
    switch (map[x][y].type)
    {
    case eau:
    case pierre:
    case feu:
    case arbrecoupe:
        return 1;
    default:
        return 0;
    }
}
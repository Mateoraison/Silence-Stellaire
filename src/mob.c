#include "headers/main.h"


int nb_mobs;
Mob mobs[MAX_MOB];

void init_mobs(SDL_Renderer * renderer){
    nb_mobs = 100;
    for(int i = 0 ; i<nb_mobs; i++){
        mobs[i].x = 580+rand()%50;
        mobs[i].y = 500+rand()%50;
        mobs[i].vitesse_y = 10;
        mobs[i].vitesse_x = 20;
        mobs[i].texture = IMG_LoadTexture(renderer,"assets/tileset/V2/Tiny_Swords/Units/BlackUnits/Pawn/Pawn.png");
        mobs[i].direction = 0;
        mobs[i].largeur = 1.5;
        mobs[i].hauteur = 1.5;
        mobs[i].time_change_dir = SDL_GetTicks();
    }
}

void update_mobs() {          
    

    for (int i = 0; i < nb_mobs; i++) {
        Uint32 timer = SDL_GetTicks();
        if (timer >= mobs[i].time_change_dir) {
            mobs[i].direction = rand() % 4;
            mobs[i].time_change_dir = timer;
        }

        switch (mobs[i].direction) {
            case 0: mobs[i].y -= 0.1; break; 
            case 1: mobs[i].y += 0.1; break; 
            case 2: mobs[i].x -= 0.1; break; 
            case 3: mobs[i].x += 0.1; break;

        }
    }
}

void afficher_mob(SDL_Renderer * renderer){
    for(int i = 0; i<nb_mobs; i++){
        SDL_FRect dest = {
            .x = mobs[i].x + perso.x,
            .y = mobs[i].y + perso.y,
            .w = mobs[i].largeur * DISPLAY_TILE_SIZE,
            .h = mobs[i].hauteur * DISPLAY_TILE_SIZE
        };
        SDL_FRect src = {
            .x = 0,
            .y = 0,
            .w = 192,
            .h = 192
        };
        if(mobs[i].texture == NULL) SDL_Log("erreur mob : %s",SDL_GetError());
        SDL_RenderTexture(renderer, mobs[i].texture, &src, &dest);
    }
}
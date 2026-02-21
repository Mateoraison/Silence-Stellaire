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
        mobs[i].vie = 3;
    }
}

void update_mobs(t_tile map[W_MAP][H_MAP]) {          
    for (int i = 0; i < nb_mobs; i++) {
        Uint32 timer = SDL_GetTicks();
        
        if (timer >= mobs[i].time_change_dir) {
            mobs[i].direction = rand() % 4;
            mobs[i].time_change_dir = timer + 2000;
        }

        float nouvelle_x = mobs[i].x;
        float nouvelle_y = mobs[i].y;
        
        float vitesse = 1.0f;
        
        switch (mobs[i].direction) {
            case 0: nouvelle_y -= vitesse; break;  // south
            case 1: nouvelle_y += vitesse; break;  // north
            case 2: nouvelle_x -= vitesse; break;  // west
            case 3: nouvelle_x += vitesse; break;  // east
        }
        
        int tile_x = (int)((nouvelle_x + mobs[i].largeur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
        int tile_y = (int)((nouvelle_y + mobs[i].hauteur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
        
        if (tile_x >= 0 && tile_x < W_MAP && tile_y >= 0 && tile_y < H_MAP) {
            if (!test_collision(tile_x, tile_y, map, 1)) {
                mobs[i].x = nouvelle_x;
                mobs[i].y = nouvelle_y;
            } else {
                mobs[i].direction = rand() % 4;
                mobs[i].time_change_dir = timer + 500;
            }
        } else {
            mobs[i].direction = rand() % 4;
            mobs[i].time_change_dir = timer + 500;
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
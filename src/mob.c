#include "headers/main.h"

static SDL_Texture * texture_pawns = NULL;
static SDL_Texture * texture_mouton = NULL;

void init_mobs(SDL_Renderer * renderer, Mob * mobs[MAX_MOB]){
    if (texture_pawns == NULL) {
        texture_pawns = IMG_LoadTexture(renderer, "assets/tileset/V2/Tiny_Swords/Units/BlackUnits/Pawn/Pawn.png");
        if (texture_pawns == NULL) {
            SDL_Log("Erreur chargement texture mob: %s", SDL_GetError());
        }
    }

    if (texture_mouton == NULL) {
        texture_mouton = IMG_LoadTexture(renderer, "assets/tileset/V2/Tiny_Swords/Terrain/Resources/Meat/Sheep/Sheep_Idle.png");
        if (texture_mouton == NULL) {
            SDL_Log("Erreur chargement texture mob: %s", SDL_GetError());
        }
    }

    mobs[0] = malloc(sizeof(Mob));
    if (mobs[0] == NULL) {
        SDL_Log("Erreur allocation mob: %s", SDL_GetError());
        return;
    }
    mobs[0]->x = 7*DISPLAY_TILE_SIZE;
    mobs[0]->y = 5*DISPLAY_TILE_SIZE;
    mobs[0]->direction = 0;
    mobs[0]->vitesse_x = 0;
    mobs[0]->vitesse_y = 0;
    mobs[0]->largeur = 1;
    mobs[0]->hauteur = 1;
    mobs[0]->time_change_dir = 0;
    mobs[0]->texture = texture_mouton;
    mobs[0]->vie = 3;

}

void update_mobs(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB]) {
    for (int i = 0; mobs[i] != NULL; i++) {
        Uint32 timer = SDL_GetTicks();

        if (timer >= mobs[i]->time_change_dir) {
            mobs[i]->direction = rand() % 4;
            mobs[i]->time_change_dir = timer + 2000;
        }

        float nouvelle_x = mobs[i]->x;
        float nouvelle_y = mobs[i]->y;

        float vitesse = 0.1f;

        switch (mobs[i]->direction) {
            case 0: nouvelle_y -= vitesse; break;  // south
            case 1: nouvelle_y += vitesse; break;  // north
            case 2: nouvelle_x -= vitesse; break;  // west
            case 3: nouvelle_x += vitesse; break;  // east
        }

        int tile_x = (int)((nouvelle_x + mobs[i]->largeur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
        int tile_y = (int)((nouvelle_y + mobs[i]->hauteur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);

        if (tile_x >= 0 && tile_x < W_MAP && tile_y >= 0 && tile_y < H_MAP) {
            if (!test_collision(tile_x, tile_y, map, 1)) {
                mobs[i]->x = nouvelle_x;
                mobs[i]->y = nouvelle_y;
            } else {
                mobs[i]->direction = rand() % 4;
                mobs[i]->time_change_dir = timer + 500;
            }
        } else {
            mobs[i]->direction = rand() % 4;
            mobs[i]->time_change_dir = timer + 500;
        }
    }
}

void afficher_mob(SDL_Renderer * renderer, Mob * mobs[MAX_MOB]){
    for(int i = 0; mobs[i] != NULL; i++){
        SDL_FRect dest = {
            .x = mobs[i]->x + perso.x,
            .y = mobs[i]->y + perso.y,
            .w = mobs[i]->largeur * DISPLAY_TILE_SIZE,
            .h = mobs[i]->hauteur * DISPLAY_TILE_SIZE
        };
        SDL_FRect src_pawn = {
            .x = 0,
            .y = 0,
            .w = 192,
            .h = 192
        };
        SDL_FRect src_mouton = {
            .x = 0,
            .y = 0,
            .w = 128,
            .h = 128
        };
        if (mobs[i]->texture == NULL) SDL_Log("erreur mob : %s", SDL_GetError());
        SDL_RenderTexture(renderer, mobs[i]->texture, mobs[i]->texture == texture_pawns ? &src_pawn : &src_mouton, &dest);
    }
}

void detruire_mobs(Mob * mobs[MAX_MOB]) {
    if (texture_pawns != NULL) {
        SDL_DestroyTexture(texture_pawns);
        texture_pawns = NULL;
    }
    if (texture_mouton != NULL) {
        SDL_DestroyTexture(texture_mouton);
        texture_mouton = NULL;
    }
    for (int i = 0; mobs[i] != NULL; i++) {
        mobs[i]->texture = NULL;
    }
}
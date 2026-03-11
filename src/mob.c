#include "headers/main.h"

static SDL_Texture * texture_pawns = NULL;
static SDL_Texture * texture_mouton = NULL;

void init_mobs(SDL_Renderer * renderer, Mob * mobs[MAX_MOB], t_tile map[W_MAP][H_MAP], int nb_pawns, int nb_moutons) {
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

    int index = 0;

    /* Apparition autour du joueur (en tuiles) */
    int rayon_app = 2; /* en tuiles */
    /* Position du joueur en coordonnées monde (mêmes constantes que dans jeux.c) */
    float joueur_monde_x = -perso.x + 500.0f; /* pixels */
    float joueur_monde_y = -perso.y + 400.0f; /* pixels */
    int tuile_joueur_x = (int)(joueur_monde_x / DISPLAY_TILE_SIZE);
    int tuile_joueur_y = (int)(joueur_monde_y / DISPLAY_TILE_SIZE);

    for(int i = 0; i < nb_moutons && index < MAX_MOB; i++){
        mobs[index]  = malloc(sizeof(Mob));

        /* choisir une tuile aléatoire dans le carré de rayon et clamp aux bords */
        int min_tuile_x = tuile_joueur_x - rayon_app;
        int max_tuile_x = tuile_joueur_x + rayon_app;
        int min_tuile_y = tuile_joueur_y - rayon_app;
        int max_tuile_y = tuile_joueur_y + rayon_app;
        if (min_tuile_x < 0) min_tuile_x = 0;
        if (min_tuile_y < 0) min_tuile_y = 0;
        if (max_tuile_x >= W_MAP) max_tuile_x = W_MAP - 1;
        if (max_tuile_y >= H_MAP) max_tuile_y = H_MAP - 1;

        int tuile_x = min_tuile_x + (rand() % (max_tuile_x - min_tuile_x + 1));
        int tuile_y = min_tuile_y + (rand() % (max_tuile_y - min_tuile_y + 1));
        if(test_collision(tuile_x, tuile_y, map, 1, (SDL_Rect){tuile_x * DISPLAY_TILE_SIZE, tuile_y * DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE})) {
            free(mobs[index]);
            continue;
        }

        mobs[index]->x = tuile_x * DISPLAY_TILE_SIZE;
        mobs[index]->y = tuile_y * DISPLAY_TILE_SIZE;
        mobs[index]->direction = 0;
        mobs[index]->vitesse_x = 0;
        mobs[index]->vitesse_y = 0;
        mobs[index]->largeur = 1;
        mobs[index]->hauteur = 1;
        mobs[index]->time_change_dir = 0;
        mobs[index]->texture = texture_mouton;
        mobs[index]->vie = 3;
        mobs[index]->id = 1; // 1 = mouton
        mobs[index]->drop_chance = 100;
        index++;
    }

    for(int i = 0; i < nb_pawns && index < MAX_MOB; i++){
        mobs[index]  = malloc(sizeof(Mob));

        /* apparition des pawns avec la même logique */
        int min_tuile_x = tuile_joueur_x - rayon_app;
        int max_tuile_x = tuile_joueur_x + rayon_app;
        int min_tuile_y = tuile_joueur_y - rayon_app;
        int max_tuile_y = tuile_joueur_y + rayon_app;
        if (min_tuile_x < 0) min_tuile_x = 0;
        if (min_tuile_y < 0) min_tuile_y = 0;
        if (max_tuile_x >= W_MAP) max_tuile_x = W_MAP - 1;
        if (max_tuile_y >= H_MAP) max_tuile_y = H_MAP - 1;

        int tuile_x = min_tuile_x + (rand() % (max_tuile_x - min_tuile_x + 1));
        int tuile_y = min_tuile_y + (rand() % (max_tuile_y - min_tuile_y + 1));
        if(test_collision(tuile_x, tuile_y, map, 1, (SDL_Rect){tuile_x * DISPLAY_TILE_SIZE, tuile_y * DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE})) {
            free(mobs[index]);
            continue;
        }

        mobs[index]->x = tuile_x * DISPLAY_TILE_SIZE;
        mobs[index]->y = tuile_y * DISPLAY_TILE_SIZE;
        mobs[index]->direction = 0;
        mobs[index]->vitesse_x = 0;
        mobs[index]->vitesse_y = 0;
        mobs[index]->largeur = 1;
        mobs[index]->hauteur = 1;
        mobs[index]->time_change_dir = 0;
        mobs[index]->texture = texture_pawns;
        mobs[index]->vie = 3;
        mobs[index]->id = 2; // 2 = pawns
        mobs[index]->drop_chance = 100;
        index++;
    }
    mobs[index] = NULL;

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
            if (!test_collision(tile_x, tile_y, map, 1, (SDL_Rect){nouvelle_x, nouvelle_y, mobs[i]->largeur * DISPLAY_TILE_SIZE, mobs[i]->hauteur * DISPLAY_TILE_SIZE})) {
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


void detruire_un_mob(Mob * mob) {
    if (mob != NULL) {
        free(mob);
    }
}
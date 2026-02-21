#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>


#define TILE_SIZE 32
#define TILESET_TILE_SIZE 32
#define TILESET_COLUMNS 10 // Ton image a 10 tuiles par ligne
#define MAP_W 24
#define MAP_H 19

/*
---- Sols ----
0 Sol métal clar 
1 Sol métal sombre 
2 Sol avec lignes 
3 Sol grillagé 

---- Murs ----
10 Mur plein 
11 Mur avec panneaux 
12 Mur avec tuyaux 
13 Mur avec lumière 
14 Mur coin gauche 
15 Mur coin droit 

---- Structures ----
20 Porte fermée 
21 Porte ouverte 
22 Sas 
23 Hublot 
24 Fenêtre spatiale 

---- Consoles & déco ----
30 Console éteinte 
31 Console allumée 
32 Ecran radar 
33 Terminal
34 Panneau d'alerte 

---- Détails techniques ----
40 Grille d'aération 
41 Câbles 
42 Générateur 
43 Réacteur 
44 Batterie 
*/

int tile_map[MAP_H][MAP_W]; 

void remplir_tileset2(int tile_map[MAP_H][MAP_W]){

    FILE *file = fopen("assets/map/vaisseau.txt", "r");
    if (!file) {
        SDL_Log("Erreur ouverture fichier Map : %s", SDL_GetError());
        return;
    }

    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            fscanf(file, "%d-", &tile_map[y][x]);
            printf("Type de la tuile [%d][%d] : %d\n", x, y, tile_map[y][x]);

        }
        fscanf(file, "\n"); // Lire le saut de ligne à la fin de chaque ligne de la carte
    }

    fclose(file);
}


void draw_map(SDL_Renderer *renderer, SDL_Texture *tileset) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            int tile_id = tile_map[y][x];

            // Calcul de la position dans le tileset (grille 10x10)
            // Ligne = ID / Colonnes, Colonne = ID % Colonnes

            SDL_FRect srcRect = {
                (float)(tile_id % TILESET_COLUMNS) * TILESET_TILE_SIZE,
                (float)(tile_id / TILESET_COLUMNS) * TILESET_TILE_SIZE,
                TILESET_TILE_SIZE,
                TILESET_TILE_SIZE
            };
            SDL_FRect dstRect = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderTexture(renderer, tileset, &srcRect, &dstRect);
        }
    }
}

bool is_blocking(int tile) {
    return (
        ((tile >= 10) && (tile <= 19)) ||   // murs
        tile == 20 ||                 // porte fermée
        tile == 22                    // sas
    );
}

int vaisseau(SDL_Renderer *renderer) {

    SDL_Texture *tileset =
        IMG_LoadTexture(renderer,
        "assets/tileset/V2/Interieur_Vaisseau/free_tiles.png");

    if (!tileset) {
        SDL_Log("Erreur: %s", SDL_GetError());
        return 1;
    }

    SDL_SetTextureScaleMode(tileset, SDL_SCALEMODE_NEAREST);

    remplir_tileset2(tile_map);

    SDL_FRect player = {400, 300, 20, 20};
    bool running = true;
    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                return 0; // quitter le jeu
        }

        const bool *state = SDL_GetKeyboardState(NULL);

        float next_x = player.x;
        float next_y = player.y;

        if (state[SDL_SCANCODE_W]) next_y -= 3;
        if (state[SDL_SCANCODE_S]) next_y += 3;
        if (state[SDL_SCANCODE_A]) next_x -= 3;
        if (state[SDL_SCANCODE_D]) next_x += 3;

        int grid_x = (int)(next_x + 10) / TILE_SIZE;
        int grid_y = (int)(next_y + 10) / TILE_SIZE;

        if (!is_blocking(tile_map[grid_y][grid_x])) {
            player.x = next_x;
            player.y = next_y;
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        draw_map(renderer, tileset);

        SDL_SetRenderDrawColor(renderer,0,255,255,255);
        SDL_RenderFillRect(renderer,&player);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(tileset);
    return 1;
}
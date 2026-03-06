#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#define TILE_SIZE 64
#define MAP_W 24
#define MAP_H 19

int tile_map[MAP_H][MAP_W];

typedef struct {
    int tileX;
    int tileY;
} TileRef;

void charger_map(const char* filename) {
    // 1. Initialisation forcée à -1
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) tile_map[y][x] = -1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        SDL_Log("ERREUR : Impossible d'ouvrir %s", filename);
        return;
    }

    int count = 0;
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            // " %d-" avec l'espace au début permet d'ignorer les retours à la ligne
            if (fscanf(file, " %d-", &tile_map[y][x]) == 1) {
                count++;
            }
        }
    }
    fclose(file);
    SDL_Log("Map chargee : %d tuiles lues.", count);
}

/*
Comment trouver les bonnes coordonnées X, Y ?

Ouvre ton fichier tileset_sf.png dans un logiciel de dessin (Paint, Photoshop, GIMP) :
S'aider de piskelapp.com pour trouver les bonnes coordonnées X, Y de chaque tuile dans le tileset.

Place ton curseur sur la tuile voulue.

Regarde les coordonnées en pixels (ex: x=384, y=320).

Divise par 64 (ex: 384/64 = 12, 320/64 = 10).

Tes coordonnées sont {12, 10} dans la palette.
*/

void draw_map(SDL_Renderer *renderer, SDL_Texture *tileset) {
    /*
    Non obligatoire, mais si je veux faire du scaling pour que ça prenne toute la fenêtre :
    SDL_Rect fenetre ;
    SDL_GetRenderViewport(renderer, &fenetre);
    float scaleX = (float)fenetre.w / (MAP_W * TILE_SIZE);
    float scaleY = (float)fenetre.h / (MAP_H * TILE_SIZE);
    */

    // Les coordonnées (X, Y) correspondent au nombre de cases de 64px 
    // en partant du haut à gauche du PNG.
    TileRef palette[] = {

        // Coutours & Extérieur 
        {10, 0},    // [0] Tuile vide
        {19, 6},   // [1] Contour coin haut gauche 
        {21, 6},   // [2] Contour coin haut droit 
        {19, 8},   // [3] Contour coin bas gauche 
        {21, 8},   // [4] Contour coin bas droit 
        {21, 7},   // [5] Contour vertical 
        {20, 6},  // [6] Contour horizontal haut
        {20, 8},  // [7] Contour horizontal bas
        {16, 8}, // [8] Bout de couloir horizontal gauche 
        {18, 8}, // [9] Bout de couloir horizontal droit 
        {8, 6}, // [10] Premier bout du réacteur 
        {9, 6}, // [11] Deucième bout du réacteur 


        // Sols
        {3, 12},  // [12] Sol simple 
        {5, 10}, // [13] Sol avec grille 
        {5, 11}, // [14] Sol avec ventilation 
        {3, 10}, // [15] Sol avec Motif 1
        {4,10}, // [16] Sol avec Motif 2
        {3, 11}, // [17] Sol avec Motif 3 
        {4, 11}, // [18] Sol avec Motif 4 


        // Murs avec Sols pour omnbre 
        {10, 9}, // [19] Mur Simple 
        {10, 10}, // [20] Sol avec ombre 

        {13, 6}, // [21] Mur avec LED Verte 
        {13, 7}, // [22] Mur avec LED Bleu 
        {13, 8}, // [23] Mur avec LED Jaune 
        {13, 9}, // [24] Mur avec LED Rouge 
        {13,10}, // [25] Sol avec ombre 

        {11, 9}, // [26] Mur avec Fenêtre 
        {11, 10}, // [27] Sol avec ombre 

        {12,9}, // [28] Mur avec Poteau 
        {12,10}, // [29] Sol avec ombre 

        {14, 9}, // [30] Mur avec chiffre 
        {14,10}, // [31] Sol avec ombre 

        {15, 9}, // [32] Mur avec Soin 
        {15, 10}, // [33] Sol avec ombre 

        {16, 9}, // [34] Mur O3
        {16, 10}, // [35] Sol avec ombre 

        {17, 9}, // [36] Mur Ouvert sombre 
        {17, 10}, // [37] Sol avec ombre 

        {18, 9}, // [38] Mur Ouvert 
        {18, 10}, // [39] Sol avec ombre 

        {19, 9}, // [40] Mur O2
        {19, 10}, // [41] Sol avec ombre 

        {20, 9}, // [42] Mur avec Cable 
        {20, 10}, // [43] Sol avec Ombre 

        {22, 9}, // [44] Mur avec Aération 
        {22, 10}, // [45] Sol avec ombre 


        // Accessoires 
        {9, 16}, // [46] Premier bout de l'écran 
        {10, 16}, // [47] Deucième bout de l'écran 

        {15, 5}, // [48] Flèche vers la droite 
        {15, 6}, // [49] Flèche vers le bas 
        {15, 7}, // [50] Flèche vers le haut 
        {15, 8}, // [51] Flèche vers la gauche 

        {0, 0}, // [52] Caméra 1
        {0, 1}, // [53] Caméra 2
        {1, 0}, // [54] Caméra 3

    };

    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            int type = tile_map[y][x];

            // Si type est -1, on ne dessine rien
            if (type < 0) continue; 
            
            // Sécurité pour ne pas dépasser la taille de la palette 
            if (type >= sizeof(palette)/sizeof(palette[0])) type = 0;

            SDL_FRect src = { 
                (float)palette[type].tileX * TILE_SIZE, 
                (float)palette[type].tileY * TILE_SIZE, 
                TILE_SIZE, TILE_SIZE 
            };
            
            SDL_FRect dst = { 
                (x * TILE_SIZE / 2 ), 
                (y * TILE_SIZE / 2 ), 
                TILE_SIZE / 2, TILE_SIZE / 2
            };
            
            SDL_RenderTexture(renderer, tileset, &src, &dst);
        }
    }
}

int vaisseau(SDL_Renderer *renderer) {
    SDL_Texture *t_tiles = IMG_LoadTexture(renderer, "assets/tileset/V2/Interieur_Vaisseau/tileset_sf.png");
    SDL_Texture *t_bg = IMG_LoadTexture(renderer, "assets/tileset/V2/Interieur_Vaisseau/Grafika/bg.png"); 

    if (!t_tiles) {
        SDL_Log("Erreur Texture Tileset: %s", SDL_GetError());
        return -1;
    }

    SDL_SetTextureScaleMode(t_tiles, SDL_SCALEMODE_NEAREST);
    charger_map("assets/map/vaisseau.txt");

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        if (t_bg) {
            SDL_RenderTexture(renderer, t_bg, NULL, NULL);
        }

        draw_map(renderer, t_tiles);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(t_tiles);
    SDL_DestroyTexture(t_bg);
    return 0;
}
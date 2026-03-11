#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include "headers/main.h"

#define TILE_SIZE 64
#define MAP_W 24
#define MAP_H 19

int tile_map[MAP_H][MAP_W];

typedef struct {
    int tileX;
    int tileY;
} TileRef;

// --- FONCTIONS DE COLLISION ---

int en_collision(int tileX, int tileY) {
    // Sécurité pour ne pas sortir du tableau
    if (tileX < 0 || tileX >= MAP_W || tileY < 0 || tileY >= MAP_H) return 1;

    int type = tile_map[tileY][tileX];

    switch(type) {
        // Contours du vaisseau 
        case 1: case 2: case 3: case 4: case 5: 
        case 6: case 7: case 8: case 9: 

        // Murs du vaisseau
        case 19: case 21: case 22: case 23:
        case 24: case 26: case 28: case 30: 
        case 32: case 34: case 36: case 38: 
        case 40: case 42: case 44: 
            return 1;

        default:
            return 0;
    }
}

// Vérifie si un rectangle (hitbox) touche une tuile solide
bool est_en_collision_rect(SDL_FRect hitbox) {
    // On calcule les tuiles qui sont couvertes par la hitbox
    int startX = (int)(hitbox.x / DISPLAY_TILE_SIZE);
    int startY = (int)(hitbox.y / DISPLAY_TILE_SIZE);
    int endX   = (int)((hitbox.x + hitbox.w) / DISPLAY_TILE_SIZE);
    int endY   = (int)((hitbox.y + hitbox.h) / DISPLAY_TILE_SIZE);

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (en_collision(x, y)) {
                SDL_FRect tile_rect = {
                    (float)x * DISPLAY_TILE_SIZE,
                    (float)y * DISPLAY_TILE_SIZE,
                    (float)DISPLAY_TILE_SIZE,
                    (float)DISPLAY_TILE_SIZE
                };

                // SDL3 : Vérifie l'intersection entre la hitbox et la tuile
                if (SDL_HasRectIntersectionFloat(&hitbox, &tile_rect)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// --- GESTION DE LA MAP ---

void spawn_perso(int tileX, int tileY) {
    // Centre le spawn (500, 400 étant le centre de l'écran)
    perso.x = -(tileX * DISPLAY_TILE_SIZE) + 500;
    perso.y = -(tileY * DISPLAY_TILE_SIZE) + 400;
}

void charger_map(const char* filename) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) tile_map[y][x] = -1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        SDL_Log("ERREUR : Impossible d'ouvrir %s", filename);
        return;
    }

    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (fscanf(file, " %d-", &tile_map[y][x]) == 1) {
                // Spawn du perso quand on lit la tuile 36
                if(tile_map[y][x] == 99){
                    spawn_perso(x,y);
                    tile_map[y][x] = 37; 
                }
            }
        }
    }
    fclose(file);
}

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
            if (type < 0) continue; 
            if (type >= (int)(sizeof(palette)/sizeof(palette[0]))) type = 0;

            SDL_FRect src = { (float)palette[type].tileX * TILE_SIZE, (float)palette[type].tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_FRect dst = { (x * DISPLAY_TILE_SIZE + perso.x), (y * DISPLAY_TILE_SIZE + perso.y), (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_RenderTexture(renderer, tileset, &src, &dst);
        }
    }
}

// --- FONCTION PRINCIPALE ---

int vaisseau(SDL_Renderer *renderer) {
    int code_sortie = 0;
    float old_x, old_y;

    int hitbox_x = 30 ;
    int hitbox_y = 60 ; 

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
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                code_sortie = 1;
            } 
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) { running = false; code_sortie = 1; }
                if (event.key.key == SDLK_M)      { running = false; code_sortie = 3; }
            }
        }

        Uint32 maintenant_dt = SDL_GetTicks();
        static Uint32 dernier_frame_dt = 0;
        float delta = (dernier_frame_dt == 0) ? 0.016f : (maintenant_dt - dernier_frame_dt) / 1000.0f;
        dernier_frame_dt = maintenant_dt;

        // On sauvegarde l'ancienne position
        old_x = perso.x;
        old_y = perso.y;

        // On tente le déplacement
        deplacer_perso(delta);

        // On calcule la hitbox uniquement au niveau des PIEDS
        // Le perso fait 64x64. On veut une zone de collision en bas.
        SDL_FRect hitbox_pieds = {
            .x = (500.0f - perso.x) + hitbox_x, // On centre la hitbox sur le personnage
            .y = (400.0f - perso.y) + hitbox_y, // On descend vers les pieds
            .w = 32.0f,                   // Largeur étroite pour passer partout
            .h = 16.0f                    // Hauteur très fine (juste la semelle)
        };

        // Test de collision avec cette nouvelle zone
        if (est_en_collision_rect(hitbox_pieds)) {
            perso.x = old_x;
            perso.y = old_y;
        }


        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        if (t_bg) SDL_RenderTexture(renderer, t_bg, NULL, NULL);

        draw_map(renderer, t_tiles);
        update_animation();
        afficher_perso(renderer);

        // --- CODE DE DEBUG POUR VOIR LA HITBOX ---
        // On choisit une couleur (Rouge : 255, 0, 0)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // On définit le rectangle à l'écran (coordonnées fixes du perso)
        // On utilise exactement les mêmes offsets (+16, +44) et tailles (32, 16)
        SDL_FRect debug_rect = {
            .x = 500.0f + hitbox_x, 
            .y = 400.0f + hitbox_y, 
            .w = 32.0f, 
            .h = 16.0f
        };

        // On dessine le contour du rectangle
        SDL_RenderRect(renderer, &debug_rect);
        // -----------------------------------------

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(t_tiles);
    SDL_DestroyTexture(t_bg);
    return code_sortie;
}
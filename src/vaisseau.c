#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include "headers/main.h"
#include <SDL3_ttf/SDL_ttf.h>
#include "headers/arcade.h"

#define TILE_SIZE 64
#define MAP_W 24
#define MAP_H 19
#define MAX_OBJETS 20

int tile_map[MAP_H][MAP_W];
int accessoire_map[MAP_H][MAP_W];

// --- STRUCTURES ---

// Référence pour une tuile dans le tileset
typedef struct {
    int tileX;
    int tileY;
} TileRef;

// Position d'un objet dans le monde (en pixels)
typedef struct {
    int type;      // L'ID de la tuile 
    float x, y;    // Sa position en pixels dans le monde
} ObjetInteractif;

ObjetInteractif liste_objets[MAX_OBJETS];
int nb_objets = 0;

// --- FONCTIONS DE LOCALISATION ---

int est_proche(float x1, float y1, float x2, float y2, float seuil) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return (dx * dx + dy * dy) <= (seuil * seuil);
}

// --- FONCTIONS D'INTERACTIONS ---

void afficher_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, float x, float y, bool centrer) {
    if (!font || !text) return;

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        float finalX = x;
        if (centrer) {
            finalX = x - ((float)surface->w / 2.0f);
        }

        SDL_FRect dstRect = { finalX, y, (float)surface->w, (float)surface->h };
        SDL_RenderTexture(renderer, texture, NULL, &dstRect);
        SDL_DestroyTexture(texture);
    }

    SDL_DestroySurface(surface);
}

void soigner(){
    perso.vie = perso.vie_max; 
}


// --- FONCTIONS DE COLLISION ---

int en_collision(int tileX, int tileY) {
    // Sécurité pour ne pas sortir du tableau
    if (tileX < 0 || tileX >= MAP_W || tileY < 0 || tileY >= MAP_H) return 1;

    int type_mur = tile_map[tileY][tileX];
    int type_accessoires = accessoire_map[tileY][tileX];

    switch(type_mur) {
        // Contours du vaisseau 
        case 1: case 2: case 3: case 4: case 5: 
        case 6: case 7: case 8: case 9: 

        // Murs du vaisseau
        case 19: case 21: case 22: case 23:
        case 24: case 26: case 28: case 30: 
        case 32: case 34: case 36: case 38: 
        case 40: case 42: case 44: 
            return 1;
    }

    switch (type_accessoires){
        // Borne d'arcade 
        case 55: case 56: case 57:

        // Le tableau
        case 46: case 47:
            return 1;
    }

    return 0; 
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

void charger_map(const char* filename_map, const char* filename_accessoire) {
    nb_objets = 0;

    // Initialisation : on met tout à 0 (vide)
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            tile_map[y][x] = -1;
            accessoire_map[y][x] = 0;
        }
    }

    // --- LECTURE COUCHE 1 (Sols et Murs) ---
    FILE *file_map = fopen(filename_map, "r");
    if (!file_map){
        SDL_Log("ERREUR CRITIQUE : Impossible d'ouvrir %s", filename_map);
    } else {
        for (int y = 0; y < MAP_H; y++) {
            for (int x = 0; x < MAP_W; x++) {
                if (fscanf(file_map, " %d-", &tile_map[y][x]) == 1) {
                    if(tile_map[y][x] == 99) {
                        spawn_perso(x, y);
                        tile_map[y][x] = 12; // On remplace le spawn par du sol
                    } 
                    // On n'ajoute ici QUE les objets interactifs liés à la map de base (comme le soin 32)
                    else if (tile_map[y][x] == 32) {
                        if (nb_objets < MAX_OBJETS) {
                            liste_objets[nb_objets].type = 32;
                            liste_objets[nb_objets].x = (float)(x * DISPLAY_TILE_SIZE);
                            liste_objets[nb_objets].y = (float)(y * DISPLAY_TILE_SIZE);
                            nb_objets++;
                        }
                    }
                }
            }
        }
        fclose(file_map);
    }

    // --- LECTURE COUCHE 2 (Accessoires) ---
    FILE *file_accessoire = fopen(filename_accessoire, "r");
    if (!file_accessoire){
        SDL_Log("ERREUR CRITIQUE : Impossible d'ouvrir %s", filename_accessoire);
    } else {
        for (int y = 0; y < MAP_H; y++) {
            for (int x = 0; x < MAP_W; x++) {
                if (fscanf(file_accessoire, " %d-", &accessoire_map[y][x]) != 1) {
                    accessoire_map[y][x] = 0;
                }

                if (accessoire_map[y][x] != 0 ) {
                    if (nb_objets < MAX_OBJETS) {
                        liste_objets[nb_objets].type = accessoire_map[y][x];
                        liste_objets[nb_objets].x = (float)(x * DISPLAY_TILE_SIZE);
                        liste_objets[nb_objets].y = (float)(y * DISPLAY_TILE_SIZE);
                        nb_objets++;
                    } 
                }
            }
        }
        fclose(file_accessoire);
    }
}

void draw_map(SDL_Renderer *renderer, SDL_Texture *tileset) {
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
        {0,2}, // [55] Borne d'arcade pour le jeux 1
        {0,2}, // [56] Borne d'arcade pour le jeux 2 
        {0,2}, // [57] Borne d'arcade pour le jeux 3

    };

    int palette_size = (int)(sizeof(palette)/sizeof(palette[0]));

    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            
            // Calcul de la position de destination (commune aux deux couches)
            float posX = (float)(x * DISPLAY_TILE_SIZE) + perso.x;
            float posY = (float)(y * DISPLAY_TILE_SIZE) + perso.y;
            SDL_FRect dst = { posX, posY, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };

            // --- COUCHE 1 : SOL ET MURS ---
            int base_type = tile_map[y][x];
            if (base_type >= 0 && base_type < palette_size) { 
                SDL_FRect src = { (float)palette[base_type].tileX * TILE_SIZE, (float)palette[base_type].tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderTexture(renderer, tileset, &src, &dst);
            }

            // --- COUCHE 2 : ACCESSOIRES ---
            int acc_type = accessoire_map[y][x];
            // CORRECTION : On ignore l'ID 0 car dans ton TXT c'est le vide
            if (acc_type > 0 && acc_type < palette_size) {
                SDL_FRect src_acc = { (float)palette[acc_type].tileX * TILE_SIZE, (float)palette[acc_type].tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderTexture(renderer, tileset, &src_acc, &dst);
            }
        }
    }
}

// --- FONCTION PRINCIPALE ---

int vaisseau(SDL_Renderer *renderer) {
    int code_sortie = 0;
    float old_offset_x, old_offset_y;
    float world_x, world_y;

    int hitbox_x = 30 ;
    int hitbox_y = 60 ; 

    SDL_Texture *t_tiles = IMG_LoadTexture(renderer, "assets/tileset/V2/Interieur_Vaisseau/tileset_sf.png");
    SDL_Texture *t_bg = IMG_LoadTexture(renderer, "assets/tileset/V2/Interieur_Vaisseau/Grafika/bg.png"); 

    if (!t_tiles) return -1;

    // Charge une police
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24); 

    SDL_SetTextureScaleMode(t_tiles, SDL_SCALEMODE_NEAREST);
    charger_map("assets/map/vaisseau.txt", "assets/map/accessoires.txt");

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
        old_offset_x = perso.x;
        old_offset_y = perso.y;

        // On tente le déplacement
        deplacer_perso(delta);

        // Position du perso dans le monde
        world_x = 500.0f - perso.x;
        world_y = 400.0f - perso.y;

        // Hitbox aux pieds
        SDL_FRect hitbox_pieds = { world_x + hitbox_x, world_y + hitbox_y, 32.0f, 16.0f };

        // Test de collision
        if (est_en_collision_rect(hitbox_pieds)) {
            perso.x = old_offset_x ;
            perso.y = old_offset_y;
            world_x = 500.0f - perso.x;
            world_y = 400.0f - perso.y;
        }

        // Test de proximité et interaction
        char message_interaction[128] = ""; 
        bool afficher_message = false;

        for (int i = 0; i < nb_objets; i++) {
            if (est_proche(liste_objets[i].x, liste_objets[i].y, world_x, world_y, 60.0f)) {
                if (liste_objets[i].type == 32) {
                    snprintf(message_interaction, sizeof(message_interaction), "Appuyez sur E pour vous soigner");
                    afficher_message = true;
                    const bool *keys = SDL_GetKeyboardState(NULL);
                    if (keys[SDL_SCANCODE_E]) soigner(); 
                }else if (liste_objets[i].type == 46 || liste_objets[i].type == 47) {
                    snprintf(message_interaction, sizeof(message_interaction), "Appuyez sur E pour voir la map");
                    afficher_message = true;
                    const bool *keys = SDL_GetKeyboardState(NULL);
                    if (keys[SDL_SCANCODE_E]) afficher_map(renderer); 
                }else if (liste_objets[i].type == 55 || liste_objets[i].type == 56 || liste_objets[i].type == 57 ) {
                    snprintf(message_interaction, sizeof(message_interaction), "Appuyez sur E pour jouer");
                    afficher_message = true ; 
                    const bool *keys = SDL_GetKeyboardState(NULL);
                    if (keys[SDL_SCANCODE_E] && liste_objets[i].type == 55) jouer_arcade1(renderer);
                    if (keys[SDL_SCANCODE_E] && liste_objets[i].type == 56) jouer_arcade2(renderer);
                    if (keys[SDL_SCANCODE_E] && liste_objets[i].type == 57) jouer_arcade3(renderer);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        if (t_bg) SDL_RenderTexture(renderer, t_bg, NULL, NULL);

        draw_map(renderer, t_tiles);
        update_animation();
        afficher_stat(renderer);
        afficher_perso(renderer);

        /*
        // --- CODE DE DEBUG POUR VOIR LA HITBOX ---
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_FRect debug_rect = { 500.0f + hitbox_x, 400.0f + hitbox_y, 32.0f, 16.0f };
        SDL_RenderRect(renderer, &debug_rect);
        // -----------------------------------------
        */
        

        if (afficher_message) {
            afficher_text(renderer, font, message_interaction, 550.0f, 330.0f, true);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(t_tiles);
    SDL_DestroyTexture(t_bg);
    if (font) TTF_CloseFont(font);
    return code_sortie;
}
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "headers/main.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


#define W_MAP 21
#define H_MAP 20
#define SOURCE_TILE_SIZE 64
#define DISPLAY_TILE_SIZE 90

Perso  perso = {0.0f, 0.0f, NULL};
int animation_frame = 0;
Uint32 animation_timer = 0;
bool perso_bouge = false;
Uint32 bouge_timer = 0;


typedef enum {vide, terreP, eau, terreCHG, terreCHD, terreCBG, terreCBD, terreH, terreB, terreG, terreD, pierre} type_t;

typedef struct tile_{
    type_t type;
    int width;
    int height;
    int colision;
}t_tile;

typedef struct tileset_{
    int width;
    int height;
    t_tile tab[W_MAP][H_MAP];
}t_tileset;


void remplir_tileset(t_tile map[W_MAP][H_MAP]){
    FILE *file = fopen("assets/map/map.txt", "r");
    if (!file) {
        SDL_Log("Erreur ouverture fichier Map : %s", SDL_GetError());
        return;
    }

    for (int y = 0; y < H_MAP; y++) {
        for (int x = 0; x < W_MAP; x++) {
            fscanf(file, "%d-", &map[x][y].type);
            map[x][y].width = DISPLAY_TILE_SIZE;
            map[x][y].height = DISPLAY_TILE_SIZE;
            if(map[x][y].type == terreP || map[x][y].type == terreCHG || map[x][y].type == terreCHD || map[x][y].type == terreCBG || map[x][y].type == terreCBD || map[x][y].type == terreH || map[x][y].type == terreB || map[x][y].type == terreG || map[x][y].type == terreD || map[x][y].type == pierre || map[x][y].type == eau){
                map[x][y].colision = 1;
            } else {
                map[x][y].colision = 0;
            }
        }
        fscanf(file, "\n"); // Lire le saut de ligne à la fin de chaque ligne de la carte
    }

    fclose(file);
}



void charger_tilemap(SDL_Renderer *renderer, SDL_Texture *tileset,
                     t_tile map[W_MAP][H_MAP], int foam[W_MAP][H_MAP]){



    if (!tileset){
        SDL_Log("Erreur chargement tileset : %s", SDL_GetError());
        SDL_Delay(2000);
        return;
    }



    for(int x = 0; x < W_MAP; x++){
        for(int y = 0; y < H_MAP; y++){
            SDL_FRect dest = {
                x * DISPLAY_TILE_SIZE + perso.x,
                y * DISPLAY_TILE_SIZE +  perso.y,
                DISPLAY_TILE_SIZE,
                DISPLAY_TILE_SIZE
            };



            SDL_FRect src;
            float type_x = 0;
            float type_y = 0;



            switch(map[x][y].type){
                case terreP:
                    type_x = 64;
                    type_y = 64;
                    break;
                case terreCHG:
                    type_x = 0;
                    type_y = 0;
                    break;
                case terreCHD:
                    type_x = 128;
                    type_y = 0;
                    break;
                case terreCBG:
                    type_x = 0;
                    type_y = 128;
                    break;
                case terreCBD:
                    type_x = 128;
                    type_y = 128;
                    break;
                case terreH:
                    type_x = 64;
                    type_y = 0;
                    break;
                case terreB:
                    type_x = 64;
                    type_y = 128;
                    break;
                case terreG:
                    type_x = 0;
                    type_y = 64;
                    break;
                case terreD:
                    type_x = 128;
                    type_y = 64;
                    break;
                case eau:
                    type_x = 125;
                    type_y = 265;
                    break;
                case vide:
                    type_x = 256;
                    type_y = 6 * 64;
                    break;
            }
            if(map[x][y].type != vide){
                src = (SDL_FRect){125, 265, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};
                SDL_RenderTexture(renderer, tileset, &src, &dest);
            }
            if(map[x][y].type != eau){
                src = (SDL_FRect){type_x, type_y, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};
                SDL_RenderTexture(renderer, tileset, &src, &dest);
            }
        }
    }
}





int jeu_principal(SDL_Renderer *renderer) {
    srand(time(NULL));
    SDL_Texture *tileset = IMG_LoadTexture(renderer, "assets/tileset/V2/Tilemap_color1.png");
    if (!tileset){
        SDL_Log("Erreur chargement tileset : %s", SDL_GetError());
        SDL_Delay(2000);
        return 1;
    }

    
    t_tile map[W_MAP][H_MAP];
    int foam[W_MAP][H_MAP];
    remplir_tileset(map);
    for (int x = 0; x < W_MAP; x++) {
        for (int y = 0; y < H_MAP; y++) {
            if(map[x][y].type <= 10 && map[x][y].type >= 3 ) foam[x][y] = rand() % 16 ;
            else foam[x][y] = 100 ;  
        }
    }

    bool running = true;
    SDL_Event event;

    int code_sortie = 0;

    while (running){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
                code_sortie = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                    running = false;
                    code_sortie = 1; 
            }
            deplacer_perso(event);
        }

        SDL_RenderClear(renderer);
        
        charger_tilemap(renderer, tileset, map, foam);
        update_animation();
        afficher_perso(renderer);

        
        static Uint32 foam_timer = 0;
        Uint32 maintenant = SDL_GetTicks();
        if (maintenant - foam_timer > 800) {  
            for (int x = 0; x < W_MAP; x++) {
                for (int y = 0; y < H_MAP; y++) {
                    if(map[x][y].type == eau) foam[x][y] = (foam[x][y] + 1) % 3;
                }
            }
            foam_timer = maintenant;
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(tileset);
    return code_sortie;
}

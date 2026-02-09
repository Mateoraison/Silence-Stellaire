#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "headers/main.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>


#define W_MAP 20
#define H_MAP 20
#define SOURCE_TILE_SIZE 32
#define DISPLAY_TILE_SIZE 100


typedef enum {terre, eau, pierre} type_t;

typedef struct tile_{
    type_t type;
    int width;
    int height;
}t_tile;

typedef struct tileset_{
    int width;
    int height;
    t_tile tab[W_MAP][H_MAP];
}t_tileset;


void remplir_tileset(t_tile map[W_MAP][H_MAP]){
    for(int x = 0; x < W_MAP; x++){
        for(int y = 0; y < H_MAP; y++){
            map[x][y].width = DISPLAY_TILE_SIZE;
            map[x][y].height = DISPLAY_TILE_SIZE;

            if(x == 0 || x == W_MAP - 1 || y == 0 || y == H_MAP - 1){
                map[x][y].type = eau;
            } else {
                map[x][y].type = terre;
            }
        }
    }
}


typedef struct {
    float x;
    float y;
} Camera;



void charger_tilemap(SDL_Renderer *renderer, SDL_Texture *tileset,
                     t_tile map[W_MAP][H_MAP], Camera camera){

    if (!tileset){
        SDL_Log("Erreur chargement tileset : %s", SDL_GetError());
        SDL_Delay(2000);
        return;
    }

    for(int x = 0; x < W_MAP; x++){
        for(int y = 0; y < H_MAP; y++){
            SDL_FRect dest = {
                x * DISPLAY_TILE_SIZE + camera.x,
                y * DISPLAY_TILE_SIZE + camera.y,
                DISPLAY_TILE_SIZE,
                DISPLAY_TILE_SIZE
            };

            SDL_FRect src;
            float type_y = 0;

            switch(map[x][y].type){
                case terre:
                    type_y = 0;
                    break;
                case eau:
                    type_y = 320;
                    break;
                default:
                    type_y = 0;
                    break;
            }

            src = (SDL_FRect){0, type_y, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};
            SDL_RenderTexture(renderer, tileset, &src, &dest);
        }
    }
}



int jeu_principal(SDL_Window *window, SDL_Renderer *renderer) {
    srand(time(NULL));
    SDL_Texture *tileset = IMG_LoadTexture(renderer, "assets/tileset/V1/tileset.png");
    if (!tileset){
        SDL_Log("Erreur chargement tileset : %s", SDL_GetError());
        SDL_Delay(2000);
        return 1;
    }
    
    t_tile map[W_MAP][H_MAP];
    remplir_tileset(map);
    Camera camera = (Camera){0, 0};
    bool running = true;
    SDL_Event event;

    int code_sortie = 0;

    while (running){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
                code_sortie = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN){
                switch(event.key.key){
                    case SDLK_Z: camera.y += 10; break;
                    case SDLK_S: camera.y -= 10; break;
                    case SDLK_Q: camera.x += 10; break;
                    case SDLK_D: camera.x -= 10; break;
                    case SDLK_ESCAPE: running = false; code_sortie = 1; break;
                }
            }
        }

        SDL_RenderClear(renderer);
        charger_tilemap(renderer, tileset, map, camera);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(tileset);
    return code_sortie;
}

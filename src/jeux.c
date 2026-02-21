#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "headers/main.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Perso  perso = {-580.0f, -500.0f, NULL, 0, 10, 10, 2000};
int animation_frame = 0;
Uint32 animation_timer = 0;
bool perso_bouge = false;
Uint32 bouge_timer = 0;
bool combat_en_cours = false;
int combat_frame = 0;
Uint32 combat_timer = 0;

void remplir_tileset(t_tile map[W_MAP][H_MAP]){
    FILE *file = fopen("assets/map/map.txt", "r");
    if (!file) {
        SDL_Log("Erreur ouverture fichier Map : %s", SDL_GetError());
        return;
    }

    for (int y = 0; y < H_MAP; y++) {
        for (int x = 0; x < W_MAP; x++) {
            fscanf(file, "%d-", (int*)&map[x][y].type);
            map[x][y].width = DISPLAY_TILE_SIZE;
            map[x][y].height = DISPLAY_TILE_SIZE;
        }
        fscanf(file, "\n"); // Lire le saut de ligne à la fin de chaque ligne de la carte
    }

    fclose(file);
}



void charger_tilemap(SDL_Renderer *renderer, SDL_Texture *tileset,
                     t_tile map[W_MAP][H_MAP], int foam[W_MAP][H_MAP]){


    SDL_Color bleu_eau = {71, 171, 169, 255};
    SDL_SetRenderDrawColor(renderer, bleu_eau.r, bleu_eau.g, bleu_eau.b, bleu_eau.a);
    
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

            SDL_FRect dest2;

            bool src_remplit = false;
            SDL_FRect src;
            SDL_FRect src3 = (SDL_FRect){64, 64, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};


            switch(map[x][y].type) {
                case terreP:   src = (SDL_FRect){64, 64, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case terreCHG: src = (SDL_FRect){0, 0, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true;break;
                case terreCHD: src = (SDL_FRect){128,0, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true;break;
                case terreCBG: src = (SDL_FRect){0, 128,SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case terreCBD: src = (SDL_FRect){128,128,SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case terreH:   src = (SDL_FRect){64, 0, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case terreB:   src = (SDL_FRect){64, 128,SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case terreG:   src = (SDL_FRect){0, 64, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case terreD:   src = (SDL_FRect){128,64, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true;break;
                case eau:      src = (SDL_FRect){125, 265,SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case pierre:   src = (SDL_FRect){576, 0, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case feu:      src = (SDL_FRect){640, 128, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case arbrecoupe:    src = (SDL_FRect){640, 0, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
                case arbreEntier: if((map[x-1][y].type != arbreEntier) && (map[x][y-1].type != arbreEntier)){ src = (SDL_FRect){768,0,128,192}; dest2 = (SDL_FRect){x*DISPLAY_TILE_SIZE + perso.x, y*DISPLAY_TILE_SIZE+perso.y,180,270};src_remplit = true;}break;
            }
            if(map[x][y].type != vide && src_remplit != false){
                SDL_FRect src2 = (SDL_FRect){125, 265, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};
                SDL_RenderTexture(renderer, tileset, &src2, &dest);
            }
            if((map[x][y].type == pierre && map[x+1][y].type == terreP) || map[x][y].type == feu || map[x][y].type == arbrecoupe){
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                SDL_RenderTexture(renderer, tileset, &src,&dest);
            }
            else if(map[x][y].type == arbreEntier && src_remplit != false){
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                dest.y = (y+1)*DISPLAY_TILE_SIZE + perso.y;
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                dest.y = (y+2)*DISPLAY_TILE_SIZE + perso.y;
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                dest.x = (x+1)*DISPLAY_TILE_SIZE + perso.x;
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                dest.y = (y+1)*DISPLAY_TILE_SIZE + perso.y;
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                dest.y = y*DISPLAY_TILE_SIZE + perso.y;
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                dest.x = x*DISPLAY_TILE_SIZE + perso.x;
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                SDL_RenderTexture(renderer, tileset, &src, &dest2);
            }
            else if(map[x][y].type != eau && src_remplit != false){
                SDL_RenderTexture(renderer, tileset, &src, &dest);
            }
        }
    }
}





int jeu_principal(SDL_Renderer *renderer) {

    perso = (Perso){-580.0f, -500.0f, NULL, 0, 10, 10, SDL_GetTicks()};
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
            if((map[x][y].type <= 10) && (map[x][y].type >= 3 )) foam[x][y] = rand() % 16 ;
            else foam[x][y] = 100 ;  
        }
    }
    

    init_mobs(renderer);

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
            gerer_combat(event);
            
            const float joueur_ecran_x = -perso.x + 500.0f;
            const float joueur_ecran_y = -perso.y + 400.0f;

            const float largeur_src = (float)SOURCE_TILE_SIZE;   
            const float hauteur_src = (float)SOURCE_TILE_SIZE;   
            const float largeur_dest = (float)DISPLAY_TILE_SIZE; 
            const float hauteur_dest = (float)DISPLAY_TILE_SIZE; 
            const float echelle_x = largeur_dest / largeur_src;
            const float echelle_y = hauteur_dest / hauteur_src;

            const float decalage_src_x = 15.0f;
            const float decalage_src_y = 7.0f;
            const float sprite_src_largeur = 26.0f;
            const float sprite_src_hauteur = 44.0f;

            float boite_x = joueur_ecran_x + decalage_src_x * echelle_x;
            float boite_y = joueur_ecran_y + decalage_src_y * echelle_y;
            float boite_w = sprite_src_largeur * echelle_x;
            float boite_h = sprite_src_hauteur * echelle_y;

            int tuile_gauche = (int)floorf(boite_x / DISPLAY_TILE_SIZE);
            int tuile_droite = (int)floorf((boite_x + boite_w - 1.0f) / DISPLAY_TILE_SIZE);
            int tuile_haut = (int)floorf((boite_y + boite_h - 10.0f) / DISPLAY_TILE_SIZE);
            int tuile_bas = (int)floorf((boite_y + boite_h - 1.0f) / DISPLAY_TILE_SIZE);

            int collision_trouve = 0;
            for (int tx = tuile_gauche; tx <= tuile_droite && !collision_trouve; tx++) {
                for (int ty = tuile_haut; ty <= tuile_bas; ty++) {
                    if (tx < 0 || ty < 0 || tx >= W_MAP || ty >= H_MAP) continue;
                    if (test_collision(tx, ty, map, 0)) { collision_trouve = 1; break; }
                }
            }

            if (collision_trouve) {
                switch (perso.direction) {
                    case 0: perso.y += 5; break;
                    case 1: perso.y -= 5; break;
                    case 2: perso.x -= 5; break;
                    case 3: perso.x += 5; break;
                }
            }
        }

        SDL_RenderClear(renderer);
        
        Uint32 maintenant = SDL_GetTicks();

        charger_tilemap(renderer, tileset, map, foam);
        update_animation();
        update_combat();
        update_mobs(map);
        if (combat_en_cours == false) afficher_perso(renderer);
        afficher_combat(renderer);
        afficher_mob(renderer);
        afficher_vie(renderer);

        perso.invincibiliter_timer = (maintenant - perso.invincibiliter_timer > 2000) ? 0 : perso.invincibiliter_timer;
        
        static Uint32 foam_timer = 0;
        
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

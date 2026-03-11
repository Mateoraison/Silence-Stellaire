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
int est_animation_degat = 0;
Uint32 animation_degat_timer = 0;
int tramblement_degat_camera = 0;
Uint32 tramblement_camera_timer = 0;
float tramblement_camera_x = 0.0f;
float tramblement_camera_y = 0.0f;


t_Item * items[MAX_ITEMS] = {NULL};
Mob * mobs[MAX_MOB] = {NULL};
int index_item = 0;

t_case * hotbar[HOTBAR_SIZE] = {NULL};
t_case *inventaire[INVENTAIRE_SIZE] = {NULL};
bool inventaire_ouvert = false;

void remplir_tileset(t_tile map[W_MAP][H_MAP], char * map_txt){

    char chemin[50] = "assets/map/";
    strcat(chemin,map_txt);
    FILE *file = fopen(chemin, "r");

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
                case vide:
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
                case pierre:   src = (SDL_FRect){577, 0, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};src_remplit = true; break;
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





int jeu_principal(SDL_Renderer *renderer, int planete) {
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
    switch (planete){
    case 1: remplir_tileset(map,"map.txt");break;
    case 2: remplir_tileset(map,"map2.txt");break;
    case 3: remplir_tileset(map,"map3.txt");break;
    default: break;
    }
    

    for (int x = 0; x < W_MAP; x++) {
        for (int y = 0; y < H_MAP; y++) {
            if((map[x][y].type <= 10) && (map[x][y].type >= 3 )) foam[x][y] = rand() % 16 ;
            else foam[x][y] = 100 ;  
        }
    }
    

    init_mobs(renderer,mobs,map,50,50);

    bool running = true;
    SDL_Event event;

    int code_sortie = 0;
    SDL_Texture * exterieure = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/ext_vaisseau1.png");

    

    while (running){
        int vie_avant = perso.vie;

        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
                code_sortie = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if(event.key.key == SDLK_ESCAPE){
                    running = false;
                    code_sortie = 1; 
                }
                if(event.key.key == SDLK_M){
                    running = false;
                    code_sortie = 3;
                }
                if(event.key.key == SDLK_V){
                    running = false;
                    code_sortie = 4;
                }
                if(event.key.key == SDLK_I){
                    inventaire_ouvert = !inventaire_ouvert;
                }
                    
            }
            gerer_clic_inventaire(inventaire, hotbar, &event);
            gerer_combat(event);
        }
        Uint32 maintenant_dt = SDL_GetTicks();
        static Uint32 dernier_frame_dt = 0;
        float delta = (dernier_frame_dt == 0) ? 0.016f : (maintenant_dt - dernier_frame_dt) / 1000.0f;
        dernier_frame_dt = maintenant_dt;
        
        float old_x = perso.x;
        float old_y = perso.y;

        deplacer_perso(delta);

        SDL_Rect hitbox = {
            .x = (500 - perso.x) + 30, 
            .y = (400 - perso.y) + 60, 
            .w = 32,                   
            .h = 16                    
        };

        int tx0 = hitbox.x / DISPLAY_TILE_SIZE;
        int ty0 = hitbox.y / DISPLAY_TILE_SIZE;
        int tx1 = (hitbox.x + hitbox.w) / DISPLAY_TILE_SIZE;
        int ty1 = (hitbox.y + hitbox.h) / DISPLAY_TILE_SIZE;

        if (tx0 < 0) tx0 = 0;
        if (ty0 < 0) ty0 = 0;
        if (tx1 >= W_MAP) tx1 = W_MAP - 1;
        if (ty1 >= H_MAP) ty1 = H_MAP - 1;

        bool collision_trouve = false;
        for (int tx = tx0; tx <= tx1 && !collision_trouve; tx++) {
            for (int ty = ty0; ty <= ty1; ty++) {
                if (test_collision(tx, ty, map, 0, hitbox)) {
                    collision_trouve = true;
                    continue;
                }
            }
        }

        if (collision_trouve) {
            perso.x = old_x;
            perso.y = old_y;
        }
        
        SDL_RenderClear(renderer);
        
        Uint32 maintenant = SDL_GetTicks();

        float perso_x_original = perso.x;
        float perso_y_original = perso.y;

        if (tramblement_degat_camera && (maintenant - tramblement_camera_timer < 500)) {
            tramblement_camera_x = (float)(rand() % 11 - 5);
            tramblement_camera_y = (float)(rand() % 11 - 5);
            perso.x += tramblement_camera_x;
            perso.y += tramblement_camera_y;
        } else if (tramblement_degat_camera && (maintenant - tramblement_camera_timer >= 500)) {
            tramblement_degat_camera = 0;
            tramblement_camera_x = 0.0f;
            tramblement_camera_y = 0.0f;
        }


        

        charger_tilemap(renderer, tileset, map, foam);
        update_animation();
        update_combat(map, mobs, renderer, items);
        update_mobs(map, mobs);
        possible_ramasser_item(items, renderer, hotbar);
        afficher_item(items, renderer);
        if (combat_en_cours == false) afficher_perso(renderer);

        SDL_FRect src = {0, 0, 644, 388};
        SDL_FRect dest = {750.0f + perso.x, 550.0f + perso.y, 644.0f, 388.0f};
        SDL_RenderTexture(renderer, exterieure, &src, &dest);

        afficher_combat(renderer);
        afficher_mob(renderer, mobs);
        afficher_hotbar(hotbar, renderer);

        afficher_vie(renderer);

        
        if(inventaire_ouvert) {
            afficher_inventaire(inventaire, renderer);
        }

        perso.x = perso_x_original;
        perso.y = perso_y_original;
        if (vie_avant > perso.vie) {
            est_animation_degat = 1;
            tramblement_degat_camera = 1;
            animation_degat_timer = maintenant;
            tramblement_camera_timer = maintenant;
        }

        if (est_animation_degat) {
            SDL_Texture *animation_degat = IMG_LoadTexture(renderer, "assets/UI/flash.png");
            SDL_FRect src = {0, 0, 64, 64};
            SDL_FRect dest = {0.0f, 0.0f, 1000.0f, 800.0f};
            SDL_RenderTexture(renderer, animation_degat, &src, &dest);
            SDL_DestroyTexture(animation_degat);
        }

        if(est_animation_degat && (maintenant - animation_degat_timer > 500)) {
            est_animation_degat = 0;
        }

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
    SDL_DestroyTexture(exterieure);
    detruire_mobs(mobs);
    SDL_DestroyTexture(tileset);
    return code_sortie;
}

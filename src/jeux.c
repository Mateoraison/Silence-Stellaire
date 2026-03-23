#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "headers/main.h"
#include "headers/pause.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Perso  perso;
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
t_case *caisse_outils[CAISSE_OUTILS_SIZE] = {NULL};
bool inventaire_ouvert = false;
bool caisse_outils_ouvert = false;
Uint32 faim_degat_timer = 0;
int argent = 0;

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

void init_caisse_outils(SDL_Renderer *renderer) {
    if (caisse_outils[0] == NULL && caisse_outils[1] == NULL && caisse_outils[2] == NULL) {
        t_Item *marteau = init_item(MARTEAU, renderer, 0.0f, 0.0f);
        t_Item *soin = init_item(SOIN, renderer, 0.0f, 0.0f);
        t_Item *piece = init_item(PIECE, renderer, 0.0f, 0.0f);
        t_Item *briquet = init_item(BRIQUET, renderer, 0.0f, 0.0f);
        t_Item *bois = init_item(BOIS, renderer, 0.0f, 0.0f);
        t_Item *engrenage = init_item(ENGRENAGE,renderer,0.0f,0.0f);
        ajouter_item_inventaire(caisse_outils, marteau);
        ajouter_item_inventaire(caisse_outils, soin);
        ajouter_item_inventaire(caisse_outils, piece);
        ajouter_item_inventaire(caisse_outils, briquet);
        ajouter_item_inventaire(caisse_outils, bois);
        ajouter_item_inventaire(caisse_outils,engrenage);
    }
}

typedef struct {
    int actif;
    int slot_hotbar;  
    Uint32 debut_cuisson;
    SDL_Renderer *renderer;
} t_Cuisson;

static t_Cuisson cuisson = {0, -1, 0, NULL};



int jeu_principal(SDL_Renderer *renderer, int planete, MIX_Track *track_global) {



    perso = (Perso){-580.0f, -500.0f, NULL, 0, 10, 10, 10, 10, SDL_GetTicks()};
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
    SDL_Texture * exterieure = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/vaisseau_casser.png");

    SDL_Texture *texture_caisse_outils = IMG_LoadTexture(renderer, "assets/UI/caisse_outils.png");



    Uint32 CYCLE_MS = 120000;
    Uint32 cycle_debut = SDL_GetTicks();

    Uint32 faim_timer = SDL_GetTicks();


    bool reset_delta = false;
    while (running){
        int vie_avant = perso.vie;

        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
                code_sortie = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if(event.key.key == SDLK_ESCAPE){
                    // Ouvrir le menu pause SANS quitter la boucle
                    int pause_result = afficher_pause(renderer, track_global);
                    if (pause_result == PAUSE_MENU) {
                        running = false;
                        code_sortie = 1;
                    }
                    // PAUSE_REPRENDRE ou PAUSE_OPTIONS: on reprend simplement
                    reset_delta = true;
                    faim_timer = SDL_GetTicks();
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
                if(event.key.key == SDLK_E) {
                    SDL_Rect rect_perso_caisse = {500, 400, 40, 60};
                    SDL_Rect rect_caisse = {10*DISPLAY_TILE_SIZE + perso.x, 11*DISPLAY_TILE_SIZE + perso.y, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
                    if (SDL_HasRectIntersection(&rect_perso_caisse, &rect_caisse)) {
                        caisse_outils_ouvert = !caisse_outils_ouvert;
                    }
                }
                
                if ((event.key.mod & SDL_KMOD_CTRL) != 0) {
                    for (int slot_drop = 0; slot_drop < HOTBAR_SIZE; ++slot_drop) {
                        if (event.key.key == SDLK_1 + slot_drop) {
                            if (hotbar[slot_drop] && hotbar[slot_drop]->item) {
                                if (index_item < MAX_ITEMS) {
                                    t_Item *drop = malloc(sizeof(t_Item));
                                    if (drop) {
                                        *drop = *(hotbar[slot_drop]->item);
                                        drop->x = -perso.x + 500.0f;
                                        drop->y = -perso.y + 400.0f;
                                        items[index_item++] = drop;
                                    }
                                }
                                hotbar[slot_drop]->quantiter--;
                                if (hotbar[slot_drop]->quantiter <= 0) {
                                    free(hotbar[slot_drop]->item);
                                    free(hotbar[slot_drop]);
                                    hotbar[slot_drop] = NULL;
                                }
                            }
                        }
                    }
                }

                
                for (int k = 0; k < 5; ++k) {
                    if (event.key.key == SDLK_1 + k) {
                        if (hotbar[k] && hotbar[k]->item) {
                            t_Item *outil = hotbar[k]->item;
                            if (outil->type == SOIN) {
                                perso.vie += 2;
                                if (perso.vie > perso.vie_max) perso.vie = perso.vie_max;
                                hotbar[k]->quantiter--;
                                if (hotbar[k]->quantiter <= 0) { free(hotbar[k]->item); free(hotbar[k]); hotbar[k] = NULL; }

                            } else if (outil->type == MARTEAU) {
                                SDL_Rect rect_perso_vaiseau = {500,400,40,60};
                                SDL_Rect rect_vaisseau = {750.0f + perso.x, 550.0f + perso.y, 644.0f, 388.0f};
                                int tout_engrenage = 0;
                                int nb = 0;
                                for(int i = 0 ; i<HOTBAR_SIZE;i++){
                                    if(hotbar[i] != NULL){
                                        printf("%d",tout_engrenage);
                                        if(hotbar[i]->item->type == ENGRENAGE){
                                            nb += hotbar[i]->quantiter--;
                                            if (hotbar[i]->quantiter <= 0) { free(hotbar[i]->item); free(hotbar[i]); hotbar[i] = NULL; }
                                        }
                                        if(nb == 1) tout_engrenage = 1;
                                    }
                                }
                                
                                if(SDL_HasRectIntersection(&rect_perso_vaiseau,&rect_vaisseau)){
                                    if(tout_engrenage){
                                        SDL_DestroyTexture(exterieure);
                                        exterieure = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/vaisseau_non_casser.png");
                                    }
                                }
                            } else if (outil->type == PIECE) {
                                argent += 1;
                                hotbar[k]->quantiter--;
                                if (hotbar[k]->quantiter <= 0) { free(hotbar[k]->item); free(hotbar[k]); hotbar[k] = NULL; }
                                
                            }else if(outil->type == BRIQUET){
                                float perso_monde_x = -perso.x + 500.0f;
                                float perso_monde_y = -perso.y + 400.0f;
                                float rayon = 120.0f;

                                for (int b = 0; b < index_item; b++) {
                                    if (items[b] != NULL && items[b]->type == BOIS) {
                                        float dx = items[b]->x - perso_monde_x;
                                        float dy = items[b]->y - perso_monde_y;
                                        if (dx*dx + dy*dy <= rayon*rayon) {
                                            float fx = items[b]->x;
                                            float fy = items[b]->y;

                                            free(items[b]);
                                            for (int j = b; j < index_item - 1; j++)
                                                items[j] = items[j+1];
                                            items[index_item - 1] = NULL;
                                            index_item--;

                                            if (index_item < MAX_ITEMS) {
                                                t_Item *fdc = init_item(FEUDECAMP, renderer, fx, fy);
                                                if (fdc) items[index_item++] = fdc;
                                            }
                                        }
                                    }
                                }
                            }else if(outil->type == VIANDE){
                                float perso_monde_x = -perso.x + 500.0f;
                                float perso_monde_y = -perso.y + 400.0f;
                                float rayon = 120.0f;

                                for (int b = 0; b < index_item; b++) {
                                    if (items[b] != NULL && items[b]->type == FEUDECAMP) {
                                        float dx = items[b]->x - perso_monde_x;
                                        float dy = items[b]->y - perso_monde_y;
                                        if (dx*dx + dy*dy <= rayon*rayon && !cuisson.actif) {
                                            // utilise une viande
                                            hotbar[k]->quantiter--;
                                            if (hotbar[k]->quantiter <= 0) {
                                                free(hotbar[k]->item);
                                                free(hotbar[k]);
                                                hotbar[k] = NULL;
                                            }
                                            // Lancer la cuisson
                                            cuisson.actif = 1;
                                            cuisson.slot_hotbar = k;
                                            cuisson.debut_cuisson = SDL_GetTicks();
                                            cuisson.renderer = renderer;
                                            break;
                                        }
                                    }
                                }
                            }else if(outil->type == VIANDECUITE){
                                perso.faim += 1;
                                if(perso.faim > perso.faim_max) perso.faim = perso.faim_max;
                                hotbar[k]->quantiter--;
                                if(hotbar[k]->quantiter <= 0){free(hotbar[k]->item); free(hotbar[k]); hotbar[k] = NULL; }
                            } else {
                                SDL_Log("Outil utilisé (slot %d): type %d", k+1, outil->type);
                            }
                        }
                    }
                }
            }
            if (inventaire_ouvert)
                gerer_clic_inventaire(inventaire, hotbar, &event, INVENTAIRE_SIZE, INVENTAIRE_COLS, INVENTAIRE_ROWS);
            if (caisse_outils_ouvert)
                gerer_clic_inventaire(caisse_outils, hotbar, &event, CAISSE_OUTILS_SIZE, CAISSE_OUTILS_SIZE, 1);
            gerer_combat(event);
        }
        Uint32 maintenant_dt = SDL_GetTicks();
        static Uint32 dernier_frame_dt = 0;
        if (reset_delta) { dernier_frame_dt = maintenant_dt; reset_delta = false; }
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
        if (cuisson.actif && SDL_GetTicks() - cuisson.debut_cuisson >= 3000) {
            cuisson.actif = 0;

            int slot_dest = -1;

            // 1. Priorité : chercher un slot VIANDE_CUITE existant pour stacker
            for (int s = 0; s < HOTBAR_SIZE; s++) {
                if (hotbar[s] != NULL && hotbar[s]->item != NULL &&
                    hotbar[s]->item->type == VIANDECUITE) {
                    slot_dest = s;
                    break;
                }
            }

            // 2. Sinon, chercher un slot vide
            if (slot_dest == -1) {
                for (int s = 0; s < HOTBAR_SIZE; s++) {
                    if (hotbar[s] == NULL) { slot_dest = s; break; }
                }
            }

            if (slot_dest != -1) {
                if (hotbar[slot_dest] != NULL && hotbar[slot_dest]->item != NULL &&
                    hotbar[slot_dest]->item->type == VIANDECUITE) {
                    // Stack sur l'existant
                    hotbar[slot_dest]->quantiter++;
                } else {
                    // Nouveau slot
                    t_Item *vc = init_item(VIANDECUITE, renderer, 0.0f, 0.0f);
                    if (vc) {
                        hotbar[slot_dest] = malloc(sizeof(t_case));
                        hotbar[slot_dest]->item      = vc;
                        hotbar[slot_dest]->quantiter = 1;
                        hotbar[slot_dest]->stackable = 1;
                        hotbar[slot_dest]->x = hotbar[slot_dest]->y = 0;
                        hotbar[slot_dest]->w = hotbar[slot_dest]->h = 0;
                    }
                }
            } else {
                SDL_Log("Hotbar pleine, viande cuite perdue !");
            }
        }
        if (cuisson.actif) {
            Uint32 ecoule = SDL_GetTicks() - cuisson.debut_cuisson;
            int secondes_restantes = 3 - (int)(ecoule / 1000);
            if (secondes_restantes < 1) secondes_restantes = 1;

            char texte_cuisson[64];
            if (ecoule < 1000)
                SDL_snprintf(texte_cuisson, sizeof(texte_cuisson), "Cuisson... [*  ] %ds", secondes_restantes);
            else if (ecoule < 2000)
                SDL_snprintf(texte_cuisson, sizeof(texte_cuisson), "Cuisson... [** ] %ds", secondes_restantes);
            else
                SDL_snprintf(texte_cuisson, sizeof(texte_cuisson), "Presque prêt ! [***] %ds", secondes_restantes);

            TTF_Font *font_cuisson = TTF_OpenFont("assets/police.ttf", 22);
            if (font_cuisson) {
                SDL_Color orange = {255, 140, 0, 255};
                SDL_Surface *surf = TTF_RenderText_Solid(font_cuisson, texte_cuisson, strlen(texte_cuisson), orange);
                if (surf) {
                    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                    // Centré en haut de l'écran
                    SDL_FRect rect = {
                        (1000.0f - surf->w) / 2.0f,
                        550.0f,
                        (float)surf->w,
                        (float)surf->h
                    };
                    SDL_RenderTexture(renderer, tex, NULL, &rect);
                    SDL_DestroyTexture(tex);
                    SDL_DestroySurface(surf);
                }
                TTF_CloseFont(font_cuisson);
            }
        }

        
        update_combat(map, mobs, renderer, items);
        update_mobs(map, mobs);
        possible_ramasser_item(items, renderer, hotbar);


        
        SDL_FRect src_caisse_outils = {0, 0, 64, 64};
        SDL_FRect dest_caisse_outils = {10*DISPLAY_TILE_SIZE + perso.x, 11*DISPLAY_TILE_SIZE + perso.y, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
        SDL_RenderTexture(renderer, texture_caisse_outils, &src_caisse_outils, &dest_caisse_outils);

        afficher_item(items, renderer);
        if (combat_en_cours == false) afficher_perso(renderer);

        SDL_FRect src_vaiseaux = {0, 0, 644, 388};
        SDL_FRect dest_vaisseau = {750.0f + perso.x, 550.0f + perso.y, 644.0f, 388.0f};
        SDL_RenderTexture(renderer, exterieure, &src_vaiseaux, &dest_vaisseau);

        afficher_combat(renderer);
        afficher_mob(renderer, mobs);

        Uint32 cycle_etat = (SDL_GetTicks() - cycle_debut) % CYCLE_MS;
        float phase = (float)cycle_etat / (float)CYCLE_MS;
        float mod = (phase < 0.5f) ? (phase * 2.0f) : ((1.0f - phase) * 2.0f);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(mod * 140.0f));
        SDL_RenderFillRect(renderer, NULL);

        afficher_hotbar(hotbar, renderer);

        if ((maintenant - faim_timer) > 10000) {
            if (perso.faim > 0) perso.faim--;
            faim_timer = maintenant;
        }

        if(perso.faim == 0 && (maintenant-faim_degat_timer)>3000){
            if((rand()%100<20)){
                perso.vie--;
            }
            faim_degat_timer = maintenant;
        }

        afficher_stat(renderer);

        //Horloge jour/nuit 
        const float bar_x     = 430.0f;  // début de la barre
        const float bar_w     = 140.0f;  // largeur totale
        const float bar_y     = 18.0f;   // hauteur centre barre
        const float bar_h     = 6.0f;    // épaisseur barre
        const float icone_size  = 24.0f;   // taille icones
        const float gap       = 6.0f;    // espace entre icone et barre

        // Icone lune à gauche, soleil à droite
        float soleil_x  = bar_x - gap - icone_size;
        float lune_x= bar_x + bar_w + gap;
        float icone_y  = bar_y - icone_size * 0.5f;

        SDL_Texture *tex_lune   = IMG_LoadTexture(renderer, "assets/UI/moon.png");
        SDL_Texture *tex_soleil = IMG_LoadTexture(renderer, "assets/UI/sun.png");

        if (tex_lune) {
            SDL_FRect dst = { lune_x, icone_y, icone_size, icone_size };
            SDL_RenderTexture(renderer, tex_lune, NULL, &dst);
            SDL_DestroyTexture(tex_lune);
        }
        if (tex_soleil) {
            SDL_FRect dst = { soleil_x, icone_y, icone_size, icone_size };
            SDL_RenderTexture(renderer, tex_soleil, NULL, &dst);
            SDL_DestroyTexture(tex_soleil);
        }

        // Fond de la barre
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 60, 60, 80, 180);
        SDL_FRect barre_fond = { bar_x, bar_y - bar_h * 0.5f, bar_w, bar_h };
        SDL_RenderFillRect(renderer, &barre_fond);

        // phase 0 = nuit (lune, gauche) ; phase 0.5 = jour (soleil, droite)
        // phase 0->0.5 = nuit->jour  ; phase 0.5->1.0 = jour->nuit
        float t = (phase < 0.5f) ? (phase * 2.0f) : ((1.0f - phase) * 2.0f);
        // t=0 nuit, t=1 jour
        SDL_SetRenderDrawColor(renderer, 255, 210, 60, 200);
        SDL_FRect barre_jour = { bar_x, bar_y - bar_h * 0.5f, bar_w * t, bar_h };
        SDL_RenderFillRect(renderer, &barre_jour);

        // Bordure barre
        SDL_SetRenderDrawColor(renderer, 150, 150, 180, 220);
        SDL_RenderRect(renderer, &barre_fond);

        // Curseur (cercle) qui glisse sur la barre
        float curseur_x = bar_x + bar_w * t;
        float cr = 7.0f;
        // Couleur : jaune le jour, bleu la nuit
        Uint8 cr_r = (Uint8)(255 * t + 100 * (1.0f - t));
        Uint8 cr_g = (Uint8)(210 * t + 120 * (1.0f - t));
        Uint8 cr_b = (Uint8)(60  * t + 220 * (1.0f - t));
        SDL_SetRenderDrawColor(renderer, cr_r, cr_g, cr_b, 255);
        for (float dy = -cr; dy <= cr; dy += 1.0f) {
            float dx = sqrtf(cr * cr - dy * dy);
            SDL_RenderLine(renderer, curseur_x - dx, bar_y + dy,
                                        curseur_x + dx, bar_y + dy);
        }


        
        TTF_Font *font_argent = TTF_OpenFont("assets/police.ttf", 22);
        char texte_argent[64];
        SDL_snprintf(texte_argent, sizeof(texte_argent), "Argent : %d$", argent);
        if (font_argent) {
                SDL_Color couleur_argent = {255, 255, 255, 255};
                SDL_Surface *surf = TTF_RenderText_Solid(font_argent,texte_argent, strlen(texte_argent), couleur_argent);
                if (surf) {
                    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_FRect rect = {
                        800.0f,
                        30.0f,
                        (float)surf->w,
                        (float)surf->h
                    };
                    SDL_RenderTexture(renderer, tex, NULL, &rect);
                    SDL_DestroyTexture(tex);
                    SDL_DestroySurface(surf);
                }
                TTF_CloseFont(font_argent);
        }

        
        


        if(caisse_outils_ouvert) {
            afficher_inventaire(caisse_outils, renderer, CAISSE_OUTILS_SIZE, CAISSE_OUTILS_SIZE, 1);
        }
        
        if(inventaire_ouvert) {
            afficher_inventaire(inventaire, renderer, INVENTAIRE_SIZE, INVENTAIRE_COLS, INVENTAIRE_ROWS);
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
            SDL_FRect dest = {0.0f, 0.0f, 1000.0f, 800.0f};
            SDL_RenderTexture(renderer, animation_degat, NULL, &dest);
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
        if(perso.vie == 0) running = game_over(renderer);
    }


    SDL_DestroyTexture(exterieure);
    SDL_DestroyTexture(texture_caisse_outils);
    detruire_mobs(mobs);
    SDL_DestroyTexture(tileset);
    return code_sortie;
}

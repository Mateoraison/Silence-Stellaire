/**
 * @file jeux.c
 * @brief Boucle principale et orchestration des etats de jeu.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "headers/main.h"
#include "headers/planete.h"
#include "headers/pause.h"
#include "headers/arcade.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ENGRENAGES_MAX 3


int engrenages_poses = 0;
bool vaisseau_repare = false;
bool g_planete3_engrenage_recupere = false;
bool g_planete3_boss_spawned = false;
bool g_planete3_spawn_engrenage_defini = false;
float g_planete3_spawn_engrenage_x = 0.0f;
float g_planete3_spawn_engrenage_y = 0.0f;
static bool planete2_mastermind_engrenage_donne = false;
static bool planete2_simon_termine = false;
static bool planete1_engrenage_objectifs_donne = false;
static bool planete2_engrenage_objectifs_donne = false;
static bool planete3_engrenage_objectifs_donne = false;
static bool planete2_barriere_4_ouverte = false;
static bool g_objectifs_valides_par_planete[4][MAX_OBJECTIFS] = {{false}};
int nb_engrenages_requis = ENGRENAGES_MAX;
/* tracke si une planète a déjà été débloquée (index 1..4) */
static bool planete_debloquee[5] = { false, true, false, false, false };

/* Petit systeme de message global a l'ecran (HUD) */
static char g_message_hud[128] = "";
static Uint32 g_message_hud_expire = 0;

static void hud_set_message(const char *msg, Uint32 duree_ms) {
    if (!msg) return;
    SDL_strlcpy(g_message_hud, msg, sizeof(g_message_hud));
    g_message_hud_expire = SDL_GetTicks() + duree_ms;
}

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
Uint32 faim_regen_timer = 0;
int argent = 0;
float vitesse_bonus = 0.0f;
bool sortie_vaisseau = false;

boss_t boss1;
boss_t boss3;

t_objectifs objectifs_jeu;
TTF_Font *font_objectifs = NULL;

static int objectifs_planete_index(int planete) {
    switch (planete) {
        case 1: return 0;
        case 2: return 1;
        case 3: return 2;
        default: return 3;
    }
}

static void objectifs_restaurer_etat(t_objectifs *obj, int planete) {
    if (!obj) return;

    int index_planete = objectifs_planete_index(planete);
    for (int i = 0; i < obj->nb; i++) {
        obj->objectifs[i].valide = g_objectifs_valides_par_planete[index_planete][i];
    }
}

static void objectifs_sauvegarder_etat(const t_objectifs *obj, int planete) {
    if (!obj) return;

    int index_planete = objectifs_planete_index(planete);
    memset(g_objectifs_valides_par_planete[index_planete], 0, sizeof(g_objectifs_valides_par_planete[index_planete]));
    for (int i = 0; i < obj->nb && i < MAX_OBJECTIFS; i++) {
        g_objectifs_valides_par_planete[index_planete][i] = obj->objectifs[i].valide;
    }
}

static void objectifs_reinitialiser_progression(void) {
    memset(g_objectifs_valides_par_planete, 0, sizeof(g_objectifs_valides_par_planete));
}

void jeu_get_progression(progression_jeu_t *out) {
    if (!out) return;

    out->planete2_mastermind_engrenage_donne = planete2_mastermind_engrenage_donne ? 1 : 0;
    out->planete2_simon_termine = planete2_simon_termine ? 1 : 0;
    out->planete1_engrenage_objectifs_donne = planete1_engrenage_objectifs_donne ? 1 : 0;
    out->planete2_engrenage_objectifs_donne = planete2_engrenage_objectifs_donne ? 1 : 0;
    out->planete3_engrenage_objectifs_donne = planete3_engrenage_objectifs_donne ? 1 : 0;
    out->planete2_barriere4_ouverte = planete2_barriere_4_ouverte ? 1 : 0;
    out->planete3_engrenage_recupere = g_planete3_engrenage_recupere ? 1 : 0;
    out->planete3_boss_spawned = g_planete3_boss_spawned ? 1 : 0;
    out->mastermind_reussi = g_mastermind_reussi ? 1 : 0;
    out->simon_reussi = g_simon_reussi ? 1 : 0;

    memset(out->planete1_objectifs_valides, 0, sizeof(out->planete1_objectifs_valides));
    memset(out->planete2_objectifs_valides, 0, sizeof(out->planete2_objectifs_valides));
    memset(out->planete3_objectifs_valides, 0, sizeof(out->planete3_objectifs_valides));

    for (int i = 0; i < MAX_OBJECTIFS; i++) {
        out->planete1_objectifs_valides[i] = g_objectifs_valides_par_planete[0][i] ? 1 : 0;
        out->planete2_objectifs_valides[i] = g_objectifs_valides_par_planete[1][i] ? 1 : 0;
        out->planete3_objectifs_valides[i] = g_objectifs_valides_par_planete[2][i] ? 1 : 0;
    }
}

void jeu_set_progression(const progression_jeu_t *in) {
    if (!in) return;

    planete2_mastermind_engrenage_donne = (in->planete2_mastermind_engrenage_donne != 0);
    planete2_simon_termine = (in->planete2_simon_termine != 0);
    planete1_engrenage_objectifs_donne = (in->planete1_engrenage_objectifs_donne != 0);
    planete2_engrenage_objectifs_donne = (in->planete2_engrenage_objectifs_donne != 0);
    planete3_engrenage_objectifs_donne = (in->planete3_engrenage_objectifs_donne != 0);
    planete2_barriere_4_ouverte = (in->planete2_barriere4_ouverte != 0);
    g_planete3_engrenage_recupere = (in->planete3_engrenage_recupere != 0);
    g_planete3_boss_spawned = (in->planete3_boss_spawned != 0);
    g_mastermind_reussi = (in->mastermind_reussi != 0);
    g_simon_reussi = (in->simon_reussi != 0);

    for (int i = 0; i < MAX_OBJECTIFS; i++) {
        g_objectifs_valides_par_planete[0][i] = (in->planete1_objectifs_valides[i] != 0);
        g_objectifs_valides_par_planete[1][i] = (in->planete2_objectifs_valides[i] != 0);
        g_objectifs_valides_par_planete[2][i] = (in->planete3_objectifs_valides[i] != 0);
    }
}

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

/* Supprime tous les ENGRENAGE du joueur (hotbar, inventaire, caisse outils)
   pour simuler la consommation entre planètes */
void retirer_engrenages_joueur(void) {
    /* Hotbar */
    for (int i = 0; i < HOTBAR_SIZE; ++i) {
        if (hotbar[i] && hotbar[i]->item && hotbar[i]->item->type == ENGRENAGE) {
            free(hotbar[i]->item);
            free(hotbar[i]);
            hotbar[i] = NULL;
        }
    }

    /* Inventaire */
    for (int i = 0; i < INVENTAIRE_SIZE; ++i) {
        if (inventaire[i] && inventaire[i]->item && inventaire[i]->item->type == ENGRENAGE) {
            free(inventaire[i]->item);
            free(inventaire[i]);
            inventaire[i] = NULL;
        }
    }

    /* Caisse d'outils */
    for (int i = 0; i < CAISSE_OUTILS_SIZE; ++i) {
        if (caisse_outils[i] && caisse_outils[i]->item && caisse_outils[i]->item->type == ENGRENAGE) {
            free(caisse_outils[i]->item);
            free(caisse_outils[i]);
            caisse_outils[i] = NULL;
        }
    }
}

/* Retire jusqu'a n engrenages du joueur en priorisant hotbar, inventaire, puis caisse.
   Retourne le nombre effectivement retire. */
int retirer_n_engrenages_joueur(int n) {
    if (n <= 0) return 0;
    int rest = n;

    for (int i = 0; i < HOTBAR_SIZE && rest > 0; ++i) {
        if (hotbar[i] && hotbar[i]->item && hotbar[i]->item->type == ENGRENAGE) {
            if (hotbar[i]->quantiter > rest) {
                hotbar[i]->quantiter -= rest;
                rest = 0;
            } else {
                rest -= hotbar[i]->quantiter;
                free(hotbar[i]->item);
                free(hotbar[i]);
                hotbar[i] = NULL;
            }
        }
    }

    for (int i = 0; i < INVENTAIRE_SIZE && rest > 0; ++i) {
        if (inventaire[i] && inventaire[i]->item && inventaire[i]->item->type == ENGRENAGE) {
            if (inventaire[i]->quantiter > rest) {
                inventaire[i]->quantiter -= rest;
                rest = 0;
            } else {
                rest -= inventaire[i]->quantiter;
                free(inventaire[i]->item);
                free(inventaire[i]);
                inventaire[i] = NULL;
            }
        }
    }

    for (int i = 0; i < CAISSE_OUTILS_SIZE && rest > 0; ++i) {
        if (caisse_outils[i] && caisse_outils[i]->item && caisse_outils[i]->item->type == ENGRENAGE) {
            if (caisse_outils[i]->quantiter > rest) {
                caisse_outils[i]->quantiter -= rest;
                rest = 0;
            } else {
                rest -= caisse_outils[i]->quantiter;
                free(caisse_outils[i]->item);
                free(caisse_outils[i]);
                caisse_outils[i] = NULL;
            }
        }
    }

    return n - rest;
}

static void retirer_item_type_depuis_caisse(t_case *inv[], int size, typeItem type) {
    for (int i = 0; i < size; i++) {
        if (inv[i] != NULL && inv[i]->item != NULL && inv[i]->item->type == type) {
            free(inv[i]->item);
            free(inv[i]);
            inv[i] = NULL;
        }
    }
}



void charger_tilemap(SDL_Renderer *renderer, SDL_Texture *tileset,
                     t_tile map[W_MAP][H_MAP], int foam[W_MAP][H_MAP], int planete){

    (void)foam;


    SDL_Color bleu_eau = (planete == 2) ? (SDL_Color){13, 25, 24, 255} : (SDL_Color){71, 171, 169, 255};
    SDL_SetRenderDrawColor(renderer, bleu_eau.r, bleu_eau.g, bleu_eau.b, bleu_eau.a);

    if (!tileset){
        SDL_Log("Erreur chargement tileset : %s", SDL_GetError());
        SDL_Delay(2000);
        return;
    }

    // Background tileset pour Planet 2, fix a la camera avec repetition continue.
    if (planete == 2) {
        float src_size = PLANETE3_TILE_SIZE;
        SDL_FRect src_lave = (SDL_FRect){48, 304, src_size, src_size};
        const float bg_y_shift = 18.0f;

        float screen_w = screen_widthf();
        float screen_h = screen_heightf();
        float offset_x = fmodf(perso.x, DISPLAY_TILE_SIZE);
        float offset_y = fmodf(perso.y, DISPLAY_TILE_SIZE);

        if (offset_x > 0.0f) offset_x -= DISPLAY_TILE_SIZE;
        if (offset_y > 0.0f) offset_y -= DISPLAY_TILE_SIZE;

        for (float bx = -DISPLAY_TILE_SIZE; bx < screen_w + DISPLAY_TILE_SIZE; bx += DISPLAY_TILE_SIZE) {
            for (float by = -DISPLAY_TILE_SIZE; by < screen_h + DISPLAY_TILE_SIZE; by += DISPLAY_TILE_SIZE) {
                SDL_FRect dest_bg = {bx + offset_x, by + offset_y + bg_y_shift, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
                SDL_RenderTexture(renderer, tileset, &src_lave, &dest_bg);
            }
        }
    }

    for(int x = 0; x < W_MAP; x++){
        for(int y = 0; y < H_MAP; y++){
            SDL_FRect dest = {
                roundf(x * DISPLAY_TILE_SIZE + perso.x),
                roundf(y * DISPLAY_TILE_SIZE + perso.y),
                DISPLAY_TILE_SIZE,
                DISPLAY_TILE_SIZE
            };

            SDL_FRect dest2;

            bool src_remplit = false;
            SDL_FRect src;
            SDL_FRect src3 = (SDL_FRect){64, 64, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};
            float src_size = SOURCE_TILE_SIZE;
            bool est_planete3 = (planete == 3);
            bool est_planete2 = (planete == 2);

            if (est_planete3 || est_planete2) {
                src_size = PLANETE3_TILE_SIZE;
                src3 = (SDL_FRect){16, 16, src_size, src_size};
            }


            if (est_planete3 || est_planete2) {
                switch(map[x][y].type) {
                    case vide:
                        if (est_planete2) {
                            src = (SDL_FRect){48, 304, src_size, src_size};
                            src_remplit = true;
                        } else {
                            src = (SDL_FRect){16, 16, src_size, src_size};
                            src_remplit = true;
                        }
                        break;
                    case terreP:
                        if (est_planete2) {
                            src = (SDL_FRect){16, 192, src_size, src_size};
                        } else {
                            src = (SDL_FRect){16, 16, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreCHG:
                        if (est_planete2) {
                            src = (SDL_FRect){0, 176, src_size, src_size};
                        } else {
                            src = (SDL_FRect){0, 0, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreH:
                        if (est_planete2) {
                            src = (SDL_FRect){16, 176, src_size, src_size};
                        } else {
                            src = (SDL_FRect){16, 0, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreCHD:
                        if (est_planete2) {
                            src = (SDL_FRect){32, 176, src_size, src_size};
                        } else {
                            src = (SDL_FRect){32, 0, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreG:
                        if (est_planete2) {
                            src = (SDL_FRect){0, 192, src_size, src_size};
                        } else {
                            src = (SDL_FRect){0, 16, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreD:
                        if (est_planete2) {
                            src = (SDL_FRect){32, 192, src_size, src_size};
                        } else {
                            src = (SDL_FRect){32, 16, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreCBG:
                        if (est_planete2) {
                            src = (SDL_FRect){0, 208, src_size, src_size};
                        } else {
                            src = (SDL_FRect){0, 32, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreB:
                        if (est_planete2) {
                            src = (SDL_FRect){16, 208, src_size, src_size};
                        } else {
                            src = (SDL_FRect){16, 32, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case terreCBD:
                        if (est_planete2) {
                            src = (SDL_FRect){32, 208, src_size, src_size};
                        } else {
                            src = (SDL_FRect){32, 32, src_size, src_size};
                        }
                        src_remplit = true;
                        break;
                    case pierre:
                        if (est_planete2) {
                            src = (SDL_FRect){48, 304, src_size, src_size};
                            src_remplit = true;
                        } else {
                            src = (SDL_FRect){16, 16, src_size, src_size};
                            src_remplit = true;
                        }
                        break;
                    case cterreHBG:
                        if (est_planete2) {
                            src = (SDL_FRect){0, 224, src_size, src_size};
                            src_remplit = true;
                        }
                        break;
                    case cterreBBG:
                        if (est_planete2) {
                            src = (SDL_FRect){0, 240, src_size, src_size};
                            src_remplit = true;
                        }
                        break;
                    case cterreHHD:
                        if (est_planete2) {
                            src = (SDL_FRect){16, 224, src_size, src_size};
                            src_remplit = true;
                        }
                        break;
                    case cterrBHD:
                        if (est_planete2) {
                            src = (SDL_FRect){16, 240, src_size, src_size};
                            src_remplit = true;
                        }
                        break;
                    case eau:
                        SDL_SetRenderDrawColor(renderer, 13, 25, 24, 255);
                        SDL_RenderFillRect(renderer, &dest);
                        src_remplit = false;
                        break;
                    case feu:
                    case arbrecoupe:
                    case arbreEntier: src = (SDL_FRect){16, 16, src_size, src_size}; src_remplit = true; break;
                }
            } else {
                switch(map[x][y].type) {
                    case vide:
                    case cterreHBG:
                    case cterreBBG:
                    case cterreHHD:
                    case cterrBHD:
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
                    case arbreEntier:
                        if (x > 0 && y > 0 && map[x-1][y].type != arbreEntier && map[x][y-1].type != arbreEntier) {
                            src = (SDL_FRect){768,0,128,192};
                            dest2 = (SDL_FRect){x*DISPLAY_TILE_SIZE + perso.x, y*DISPLAY_TILE_SIZE+perso.y,180,270};
                            src_remplit = true;
                        }
                        break;
                }
            }
            if(!est_planete3 && !est_planete2 && map[x][y].type != vide && src_remplit != false){
                SDL_FRect src2 = (SDL_FRect){125, 265, SOURCE_TILE_SIZE, SOURCE_TILE_SIZE};
                SDL_RenderTexture(renderer, tileset, &src2, &dest);
            }
            bool pierre_vers_terre = (x + 1 < W_MAP && map[x][y].type == pierre && map[x+1][y].type == terreP);
            if(!est_planete3 && !est_planete2 && (pierre_vers_terre || map[x][y].type == feu || map[x][y].type == arbrecoupe)){
                SDL_RenderTexture(renderer, tileset, &src3, &dest);
                SDL_RenderTexture(renderer, tileset, &src,&dest);
            }
            else if(!est_planete3 && !est_planete2 && map[x][y].type == arbreEntier && src_remplit != false){
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
            } else if((est_planete3 || est_planete2) && map[x][y].type == eau) {
                src = est_planete3 ? (SDL_FRect){0, 128, src_size, src_size} : (SDL_FRect){48, 304, src_size, src_size};
                SDL_RenderTexture(renderer, tileset, &src, &dest);
            } else if(src_remplit != false){
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
        if (marteau) { ajouter_item_inventaire(caisse_outils, CAISSE_OUTILS_SIZE, marteau); free(marteau); }
        if (soin) { ajouter_item_inventaire(caisse_outils, CAISSE_OUTILS_SIZE, soin); free(soin); }
        if (piece) { ajouter_item_inventaire(caisse_outils, CAISSE_OUTILS_SIZE, piece); free(piece); }
        if (briquet) { ajouter_item_inventaire(caisse_outils, CAISSE_OUTILS_SIZE, briquet); free(briquet); }
        if (bois) { ajouter_item_inventaire(caisse_outils, CAISSE_OUTILS_SIZE, bois); free(bois); }
        if (engrenage) { ajouter_item_inventaire(caisse_outils, CAISSE_OUTILS_SIZE, engrenage); free(engrenage); }
    }
}

/** @brief Etat de cuisson d'un item dans la hotbar. */
typedef struct {
    int actif;
    int slot_hotbar;
    Uint32 debut_cuisson;
    SDL_Renderer *renderer;
} t_Cuisson;

static t_Cuisson cuisson = {0, -1, 0, NULL};

/** @brief Etat de la console de commandes de debug en jeu. */
typedef struct {
    int ouvert;
    char saisie[128];
    char message[160];
    Uint32 message_expire;
    TTF_Font *font;
} t_console_cmd;

static t_console_cmd console_cmd = {0};
static int console_god_mode = 0;

static void console_set_message(const char *msg) {
    SDL_snprintf(console_cmd.message, sizeof(console_cmd.message), "%s", msg ? msg : "");
    console_cmd.message_expire = SDL_GetTicks() + 4000;
}

static int console_ajouter_item(typeItem type, int quantite, SDL_Renderer *renderer) {
    int ajoutes = 0;
    for (int i = 0; i < quantite; i++) {
        t_Item *item = init_item(type, renderer, 0.0f, 0.0f);
        if (!item) {
            break;
        }
        ajouter_item_hotbar(hotbar, item, renderer);
        free(item);
        ajoutes++;
    }
    return ajoutes;
}

static void console_executer_commande(const char *commande, SDL_Renderer *renderer) {
    if (!commande || commande[0] == '\0') {
        return;
    }

    while (*commande == ' ' || *commande == '\t') {
        commande++;
    }

    while (*commande == '/') {
        commande++;
    }

    if (*commande == '\0') {
        console_set_message("Commande vide. Tape /help");
        return;
    }

    if (SDL_strcasecmp(commande, "help") == 0) {
        console_set_message("/give engrenage|piece [q], /heal, /tp x y, /god on|off, /repare");
        return;
    }

    if (SDL_strcasecmp(commande, "repare") == 0 || SDL_strcasecmp(commande, "repair") == 0) {
        vaisseau_repare = true;
    engrenages_poses = nb_engrenages_requis;
        console_set_message("Vaisseau repare");
        return;
    }

    if (SDL_strcasecmp(commande, "heal") == 0) {
        perso.vie = perso.vie_max;
        perso.faim = perso.faim_max;
        console_set_message("Soin applique: vie et faim remplies");
        return;
    }

    int tx = 0;
    int ty = 0;
    if (sscanf(commande, "tp %d %d", &tx, &ty) == 2) {
        if (tx < 0) tx = 0;
        if (ty < 0) ty = 0;
        if (tx >= W_MAP) tx = W_MAP - 1;
        if (ty >= H_MAP) ty = H_MAP - 1;

        update_screen_metrics(renderer);
        perso.x = -(tx * DISPLAY_TILE_SIZE) + screen_center_x();
        perso.y = -(ty * DISPLAY_TILE_SIZE) + screen_center_y();

        char msg[160];
        SDL_snprintf(msg, sizeof(msg), "Teleportation: tuile (%d, %d)", tx, ty);
        console_set_message(msg);
        return;
    }

    char etat[16] = {0};
    if (sscanf(commande, "god %15s", etat) == 1) {
        if (SDL_strcasecmp(etat, "on") == 0 || SDL_strcasecmp(etat, "1") == 0) {
            console_god_mode = 1;
            perso.vie = perso.vie_max;
            perso.faim = perso.faim_max;
            console_set_message("God mode active");
            return;
        }
        if (SDL_strcasecmp(etat, "off") == 0 || SDL_strcasecmp(etat, "0") == 0) {
            console_god_mode = 0;
            console_set_message("God mode desactive");
            return;
        }
        console_set_message("Usage: /god on|off");
        return;
    }

    char objet[32] = {0};
    int quantite = 1;
    int lus = sscanf(commande, "give %31s %d", objet, &quantite);
    if (lus >= 1) {
        if (quantite < 1) {
            quantite = 1;
        }

        if (SDL_strcasecmp(objet, "engrenage") == 0 || SDL_strcasecmp(objet, "gear") == 0) {
            int ajoutes = console_ajouter_item(ENGRENAGE, quantite, renderer);
            if (ajoutes > 0) {
                char msg[160];
                SDL_snprintf(msg, sizeof(msg), "Ajout: %d engrenage(s)", ajoutes);
                console_set_message(msg);
            } else {
                console_set_message("Echec: impossible d'ajouter l'item");
            }
            return;
        }

        if (SDL_strcasecmp(objet, "piece") == 0 || SDL_strcasecmp(objet, "pieces") == 0) {
            int ajoutes = console_ajouter_item(PIECE, quantite, renderer);
            if (ajoutes > 0) {
                char msg[160];
                SDL_snprintf(msg, sizeof(msg), "Ajout: %d piece(s)", ajoutes);
                console_set_message(msg);
            } else {
                console_set_message("Echec: impossible d'ajouter l'item");
            }
            return;
        }

        console_set_message("Item inconnu. Exemple: /give engrenage 2");
        return;
    }

    console_set_message("Commande inconnue. Tape /help");
}

static void console_afficher(SDL_Renderer *renderer) {
    if (!console_cmd.font) {
        return;
    }

    const float panel_w = screen_widthf() * 0.8f;
    const float panel_h = 122.0f;
    const float panel_x = (screen_widthf() - panel_w) * 0.5f;
    const float panel_y = screen_heightf() - panel_h - 20.0f;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 12, 18, 30, 220);
    SDL_FRect panel = {panel_x, panel_y, panel_w, panel_h};
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 120, 180, 230, 220);
    SDL_RenderRect(renderer, &panel);

    SDL_Color col = {230, 230, 230, 255};
    const char *hint = "Terminal (F1 pour fermer) - /help pour commandes";
    SDL_Surface *s_hint = TTF_RenderText_Solid(console_cmd.font, hint, strlen(hint), col);
    if (s_hint) {
        SDL_Texture *t_hint = SDL_CreateTextureFromSurface(renderer, s_hint);
        SDL_FRect r_hint = {panel_x + 10.0f, panel_y + 8.0f, (float)s_hint->w, (float)s_hint->h};
        SDL_RenderTexture(renderer, t_hint, NULL, &r_hint);
        SDL_DestroyTexture(t_hint);
        SDL_DestroySurface(s_hint);
    }

    char ligne[160];
    SDL_snprintf(ligne, sizeof(ligne), "> %s", console_cmd.saisie);
    SDL_Surface *s_cmd = TTF_RenderText_Solid(console_cmd.font, ligne, strlen(ligne), col);
    if (s_cmd) {
        SDL_Texture *t_cmd = SDL_CreateTextureFromSurface(renderer, s_cmd);
        SDL_FRect r_cmd = {panel_x + 10.0f, panel_y + 34.0f, (float)s_cmd->w, (float)s_cmd->h};
        SDL_RenderTexture(renderer, t_cmd, NULL, &r_cmd);
        SDL_DestroyTexture(t_cmd);
        SDL_DestroySurface(s_cmd);
    }

    if (console_cmd.message[0] != '\0') {
        SDL_Color out_col = {170, 220, 255, 255};
        SDL_Surface *s_out = TTF_RenderText_Solid(console_cmd.font, console_cmd.message, strlen(console_cmd.message), out_col);
        if (s_out) {
            SDL_Texture *t_out = SDL_CreateTextureFromSurface(renderer, s_out);
            SDL_FRect r_out = {panel_x + 10.0f, panel_y + 62.0f, (float)s_out->w, (float)s_out->h};
            SDL_RenderTexture(renderer, t_out, NULL, &r_out);
            SDL_DestroyTexture(t_out);
            SDL_DestroySurface(s_out);
        }
    }

    /* Afficher le message de console au centre de l'ecran si non expire */
    if (console_cmd.message[0] != '\0' && console_cmd.message_expire > SDL_GetTicks()) {
        SDL_Color center_col = {255, 255, 255, 255};
        SDL_Surface *s_center = TTF_RenderText_Solid(console_cmd.font, console_cmd.message, strlen(console_cmd.message), center_col);
        if (s_center) {
            SDL_Texture *t_center = SDL_CreateTextureFromSurface(renderer, s_center);
            float cx = screen_center_x();
            float cy = screen_center_y();
            float w = (float)s_center->w;
            float h = (float)s_center->h;
            /* Assombrir l'arriere-plan pour faire ressortir l'ecran de pause */
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 10, 10, 30, 200);
            SDL_FRect bg = {cx - w * 0.5f - 10.0f, cy - h * 0.5f - 6.0f, w + 20.0f, h + 12.0f};
            SDL_RenderFillRect(renderer, &bg);
            SDL_SetRenderDrawColor(renderer, 120, 180, 230, 220);
            SDL_RenderRect(renderer, &bg);

            SDL_FRect dst = {cx - w * 0.5f, cy - h * 0.5f, w, h};
            SDL_RenderTexture(renderer, t_center, NULL, &dst);
            SDL_DestroyTexture(t_center);
            SDL_DestroySurface(s_center);
        }
    }
}

static void afficher_message_interaction(SDL_Renderer *renderer, const char *message, float x, float y) {
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 20);
    if (!font || !message || message[0] == '\0') {
        if (font) TTF_CloseFont(font);
        return;
    }

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, message, strlen(message), blanc);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_FRect rect = {
                x - (float)surface->w / 2.0f,
                y,
                (float)surface->w,
                (float)surface->h
            };
            SDL_RenderTexture(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }

    TTF_CloseFont(font);
}

/* Compte le nombre de clés dans la hotbar */
static int compter_cles(void) {
    int nombre_cles = 0;
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        if (hotbar[i] != NULL && hotbar[i]->item != NULL && hotbar[i]->item->type == CLE) {
            nombre_cles += hotbar[i]->quantiter;
        }
    }
    return nombre_cles;
}

int jeu_principal(SDL_Renderer *renderer, int planete, MIX_Track *track_global, bool reprendre_partie) {

    if (font_objectifs) {
        TTF_CloseFont(font_objectifs);
        font_objectifs = NULL;
    }
    detruire_tout_item(items);
    for (int i = 0; i < MAX_ITEMS; i++) items[i] = NULL;
    index_item = 0;
    detruire_mobs(mobs);
    reset_mob_respawn_queue();

    /* Initialiser le nombre d'engrenages requis selon la planete */
    if (planete == 1) nb_engrenages_requis = 3;
    else nb_engrenages_requis = 1;

    /* Consommer 3 engrenages uniquement si on découvre la planète pour la
       première fois (et si on ne reprend pas une partie). */
    if (planete >= 3 && planete <= 4) {
        if (!planete_debloquee[planete]) {
            /* retirer exactement 3 engrenages (priorité hotbar->inventaire->caisse) */
            extern int retirer_n_engrenages_joueur(int n);
            (void)retirer_n_engrenages_joueur(3);
            planete_debloquee[planete] = true;
        } else {
            /* reprise : marquer comme débloquée */
            planete_debloquee[planete] = true;
        }
    }

    init_boss(renderer, &boss1, TYPE_BOSS_DEMON_DE_FEU, 2550.0f, -100.0f);
    /*
     * Boss3 (minotaure) sera re-initialise a son vrai point de spawn
     * au moment ou il apparait sur la planete 3.
     */
    init_boss(renderer, &boss3, TYPE_BOSS_MINOTAURE, -10000.0f, -10000.0f);



    if (!reprendre_partie) {
        for (int i = 0; i < HOTBAR_SIZE; i++) {
            if (hotbar[i]) {
                if (hotbar[i]->item) free(hotbar[i]->item);
                free(hotbar[i]);
                hotbar[i] = NULL;
            }
        }

        for (int i = 0; i < INVENTAIRE_SIZE; i++) {
            if (inventaire[i]) {
                if (inventaire[i]->item) free(inventaire[i]->item);
                free(inventaire[i]);
                inventaire[i] = NULL;
            }
        }

        for (int i = 0; i < CAISSE_OUTILS_SIZE; i++) {
            if (caisse_outils[i]) {
                if (caisse_outils[i]->item) free(caisse_outils[i]->item);
                free(caisse_outils[i]);
                caisse_outils[i] = NULL;
            }
        }

        argent = 0;
        vitesse_bonus = 0.0f;
        inventaire_ouvert = false;
        caisse_outils_ouvert = false;
        init_caisse_outils(renderer);

        perso = (Perso){screen_center_x() - 1080.0f, screen_center_y() - 900.0f, NULL, 0, 9, 10, 9, 10, SDL_GetTicks()};
        srand(time(NULL));
    g_planete3_engrenage_recupere = false;
    g_planete3_boss_spawned = false;
        planete2_mastermind_engrenage_donne = false;
        planete2_simon_termine = false;
        planete1_engrenage_objectifs_donne = false;
        planete2_engrenage_objectifs_donne = false;
        planete3_engrenage_objectifs_donne = false;
        planete2_barriere_4_ouverte = false;

        if (planete == 3) {
            boss3.est_battu = 0;
            boss3.est_agro = 0;
            retirer_item_type_depuis_caisse(caisse_outils, CAISSE_OUTILS_SIZE, ENGRENAGE);
        }
        if (planete == 1) {
            int code_cinematique = jouer_cinematique_crash(renderer);
            if (code_cinematique == 1) {
                return 1;
            }
        }
    }


    const char *tileset_path = (planete == 2 || planete == 3)
        ? "assets/map/tileset_P2_P3.png"
        : "assets/tileset/V2/Tilemap_color1.png";

    SDL_Texture *tileset = IMG_LoadTexture(renderer, tileset_path);
    if (!tileset){
        SDL_Log("Erreur chargement tileset : %s", SDL_GetError());
        SDL_Delay(2000);
        return 1;
    }
    SDL_SetTextureScaleMode(tileset, SDL_SCALEMODE_NEAREST);


    SDL_Texture *texture_alien_planete2 = NULL;
    SDL_Texture *texture_rocher = NULL;
    const float alien_planete2_x_1 = 760.2f;
    const float alien_planete2_y_1 = 1935.4f;
    const float alien_planete2_x_2 = alien_planete2_x_1 + (13.0f * DISPLAY_TILE_SIZE);
    const float alien_planete2_y_2 = alien_planete2_y_1;
    const float alien_planete2_w = DISPLAY_TILE_SIZE * 0.85f;
    const float alien_planete2_h = DISPLAY_TILE_SIZE * 0.85f;
    // Première barrière : 1er epreuve
    const float rock_center_x_base = alien_planete2_x_1; 
    const float rock_center_y_base = alien_planete2_y_1 - (4.0f * DISPLAY_TILE_SIZE);
    // seconde barrière : 1er epreuve
    const float rock2_center_x_base = alien_planete2_x_1 + (5.0f * DISPLAY_TILE_SIZE);
    const float rock2_center_y_base = alien_planete2_y_1;
    // troisième barrière : 2eme epreuve (bloque le second alien) - disparaît quand simon_termine
    const float rock3_center_x_base = alien_planete2_x_2 + (4.0f * DISPLAY_TILE_SIZE);
    const float rock3_center_y_base = alien_planete2_y_2;
    // quatrième barrière : permanente jusqu'à avoir 2 clés (décalée de 3 tuiles à droite)
    const float rock4_center_x_base = rock3_center_x_base + (5.0f * DISPLAY_TILE_SIZE);
    const float rock4_center_y_base = alien_planete2_y_2;
    // variable pour tracker si la barrière 4 est ouverte (si joueur a 2 clés et presse E)



    if (planete == 2) {
        texture_alien_planete2 = IMG_LoadTexture(renderer, "assets/tileset/V2/alien/PNG/alien_red/red__0000_idle_1.png");

        if (!texture_alien_planete2) {
            SDL_Log("Erreur chargement alien planete 2 : %s", SDL_GetError());
        } else {
            SDL_SetTextureScaleMode(texture_alien_planete2, SDL_SCALEMODE_NEAREST);
        }

        /* charger texture rocher pour barrière (apparaît tant que la 1ere épreuve n'est pas réussie)
           essayer la casse correcte V2 puis v2 en fallback (Linux sensible à la casse)
        */
        texture_rocher = IMG_LoadTexture(renderer, "assets/tileset/V2/rocher.png");
        if (!texture_rocher) {
            texture_rocher = IMG_LoadTexture(renderer, "assets/tileset/v2/rocher.png");
        }
        if (texture_rocher) {
            SDL_SetTextureScaleMode(texture_rocher, SDL_SCALEMODE_NEAREST);
        } else {
            SDL_Log("Erreur chargement texture rocher (assets/tileset/V2|v2/rocher.png) : %s", SDL_GetError());
        }
        
    }


    t_tile map[W_MAP][H_MAP];
    int foam[W_MAP][H_MAP];
    int engrenage_case_x = -1;
    int engrenage_case_y = -1;
    switch (planete){
    case 1:
        remplir_tileset(map,"map.txt");
        /* Placer un engrenage dans la map de la planete 1 (nouvelle partie seulement) */
        if (!reprendre_partie && index_item < MAX_ITEMS) {
            /* Collecter toutes les tuiles praticables, puis en choisir une au hasard */
            int coords_x[W_MAP * H_MAP];
            int coords_y[W_MAP * H_MAP];
            int found = 0;
            for (int y = 0; y < H_MAP; ++y) {
                for (int x = 0; x < W_MAP; ++x) {
                    type_t tt = map[x][y].type;
                    if (tt != eau && tt != pierre && tt != arbreEntier) {
                        coords_x[found] = x;
                        coords_y[found] = y;
                        found++;
                    }
                }
            }
            if (found > 0) {
                int pick = rand() % found;
                int fx = coords_x[pick];
                int fy = coords_y[pick];
                float ix = fx * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE - 32.0f) * 0.5f;
                float iy = fy * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE - 32.0f) * 0.5f;
                t_Item *engrenage_map = init_item(ENGRENAGE, renderer, ix, iy);
                if (engrenage_map != NULL) {
                    items[index_item++] = engrenage_map;
                }
            }
        }
        break;
    case 2: remplir_tileset(map,"map2.txt");break;
    case 3:
        remplir_tileset(map,"map3.txt");
        generer_labyrinthe_planete3(map, &engrenage_case_x, &engrenage_case_y);

        if (engrenage_case_x < 0 || engrenage_case_x >= W_MAP ||
            engrenage_case_y < 0 || engrenage_case_y >= H_MAP) {
            engrenage_case_x = W_MAP - 8;
            engrenage_case_y = H_MAP - 8;
        }

        if (engrenage_case_x >= 0 && engrenage_case_y >= 0) {
            float spawn_world_x = (engrenage_case_x * DISPLAY_TILE_SIZE) + (DISPLAY_TILE_SIZE * 0.5f);
            float spawn_world_y = (engrenage_case_y * DISPLAY_TILE_SIZE) + (DISPLAY_TILE_SIZE * 0.5f);
            g_planete3_spawn_engrenage_x = screen_center_x() - spawn_world_x;
            g_planete3_spawn_engrenage_y = screen_center_y() - spawn_world_y;
            g_planete3_spawn_engrenage_defini = true;
        } else {
            g_planete3_spawn_engrenage_defini = false;
            g_planete3_spawn_engrenage_x = 0.0f;
            g_planete3_spawn_engrenage_y = 0.0f;
        }

        if (sortie_vaisseau && reprendre_partie && index_item < MAX_ITEMS &&
            engrenage_case_x >= 0 && engrenage_case_y >= 0) {
            float ix = engrenage_case_x * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE - 32.0f) * 0.5f;
            float iy = engrenage_case_y * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE - 32.0f) * 0.5f;
            t_Item *engrenage_sol = init_item(ENGRENAGE, renderer, ix, iy);
            if (engrenage_sol != NULL) {
                items[index_item++] = engrenage_sol;
            }
        }
        break;
    case 4:
        remplir_tileset(map,"map2.txt");
        break;
    default: break;
    }

    boss_set_navigation_map(map);

    boss_t *boss_actif = (planete == 3) ? &boss3 : (planete == 2 ? &boss1 : NULL);

    /* Ajouter les items pré-placés sur la map (clés et engrenage) */
    if (planete == 2) {
        // Engrenage à position spécifiée
        if (index_item < MAX_ITEMS) {
            t_Item *engrenage_map = init_item(ENGRENAGE, renderer, 1798.564941f, 3187.787109f);
            if (engrenage_map != NULL) {
                items[index_item++] = engrenage_map;
            }
        }
        // Clé 1 à position spécifiée
        if (index_item < MAX_ITEMS) {
            t_Item *cle1_map = init_item(CLE, renderer, 3326.389160f, 3278.864258f);
            if (cle1_map != NULL) {
                items[index_item++] = cle1_map;
            }
        }
        // Clé 2 à position spécifiée
        if (index_item < MAX_ITEMS) {
            t_Item *cle2_map = init_item(CLE, renderer, 780.847900f, 419.043213f);
            if (cle2_map != NULL) {
                items[index_item++] = cle2_map;
            }
        }
    }

    for (int x = 0; x < W_MAP; x++) {
        for (int y = 0; y < H_MAP; y++) {
            if((map[x][y].type <= 10) && (map[x][y].type >= 3 )) foam[x][y] = rand() % 16 ;
            else foam[x][y] = 100 ;
        }
    }


    if (!reprendre_partie) {
        init_mobs(renderer,mobs,map,100,100);
        engrenages_poses = 0;
        vaisseau_repare = false;
    }

    sauvegarde_appliquer_si_disponible(renderer);

    if (reprendre_partie && planete == 3 && g_planete3_boss_spawned) {
        float spawn_boss_x = 488.0f + (VAISSEAU_WIDTH * 0.8f);
        float spawn_boss_y = 481.0f;
        boss3.x = spawn_boss_x;
        boss3.y = spawn_boss_y;
        boss3.spawn_x = spawn_boss_x;
        boss3.spawn_y = spawn_boss_y;
    }

    if (reprendre_partie) {
        init_mobs(renderer, mobs, map, 100, 100);
        if (planete == 2 && sortie_vaisseau) {
            perso.x = 180.0f ;
            perso.y = -(28 * DISPLAY_TILE_SIZE) ;
            perso.direction = 0;
        } else if (planete == 3) {
            if (g_planete3_engrenage_recupere && g_planete3_spawn_engrenage_defini) {
                /* Si l'engrenage est deja recupere, reprendre directement dans la salle engrenage. */
                perso.x = g_planete3_spawn_engrenage_x;
                perso.y = g_planete3_spawn_engrenage_y;
            } else {
                /* Sinon, arriver au vaisseau pour eviter les incoherences du labyrinthe aleatoire. */
                perso.x = 160.0f;
                perso.y = -250.0f;
            }
            perso.direction = 0;
        } else if (planete == 1 && sortie_vaisseau) {
            perso.x = -100.0f;
            perso.y = -350.0f;
            perso.direction = 0;
        }
        
    }

    if (!reprendre_partie && planete == 1) {
        objectifs_reinitialiser_progression();
    }

    objectifs_init(&objectifs_jeu, planete);
    objectifs_restaurer_etat(&objectifs_jeu, planete);
    font_objectifs = TTF_OpenFont("assets/police.ttf", 14);

    /* Message d'introduction sur ce qu'il faut faire en arrivant sur la planete */
        if (!reprendre_partie) {
            /* pas de message centre a l'arrivee */
        }

    bool running = true;
    SDL_Event event;
    console_cmd.ouvert = 0;
    console_cmd.saisie[0] = '\0';
    console_cmd.message[0] = '\0';
    console_cmd.message_expire = 0;
    console_cmd.font = TTF_OpenFont("assets/police.ttf", 18);

    int code_sortie = 0;
    SDL_Texture * exterieure = IMG_LoadTexture(renderer,
        vaisseau_repare
            ? "assets/tileset/V2/EXT_vaisseau/vaisseau_non_casser.png"
            : "assets/tileset/V2/EXT_vaisseau/vaisseau_casser.png");
    int exterieure_reparee = vaisseau_repare ? 1 : 0;

    SDL_Texture *texture_caisse_outils = IMG_LoadTexture(renderer, "assets/UI/caisse_outils.png");

    float  vaisseau_world_x = VAISSEAU_WORLD_X;
    float vaisseau_world_y = VAISSEAU_WORLD_Y;
    if (planete == 2) {
        vaisseau_world_y = 2750.0f;
        vaisseau_world_x = 500.0f;
    } else if (planete == 3) {
        vaisseau_world_x = 488.0f;
        vaisseau_world_y = 481.0f;
    }

    float vaisseau_scale_x = (VAISSEAU_WIDTH / VAISSEAU_SPRITE_SRC_W);
    float vaisseau_scale_y = (VAISSEAU_HEIGHT / VAISSEAU_SPRITE_SRC_H);
    float vaisseau_collision_x = vaisseau_world_x + (VAISSEAU_SOLID_SRC_X * vaisseau_scale_x);
    float vaisseau_collision_y = vaisseau_world_y + (VAISSEAU_SOLID_SRC_Y * vaisseau_scale_y);
    float vaisseau_collision_w = (VAISSEAU_SOLID_SRC_W * vaisseau_scale_x);
    float vaisseau_collision_h = (VAISSEAU_SOLID_SRC_H * vaisseau_scale_y);



    Uint32 CYCLE_MS = 120000;
    Uint32 cycle_debut = SDL_GetTicks();

    Uint32 faim_timer = SDL_GetTicks();
    faim_regen_timer = SDL_GetTicks();


    bool reset_delta = false;
    while (running){
        update_screen_metrics(renderer);
        float cx = screen_center_x();
        float cy = screen_center_y();
        float sw = screen_widthf();
        float sh = screen_heightf();

        int vie_avant = perso.vie;
        char message_interaction[128] = "";
        bool afficher_message = false;

        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_MOUSE_MOTION ||
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                SDL_ConvertEventToRenderCoordinates(renderer, &event);
            }

            if (console_cmd.ouvert && event.type == SDL_EVENT_TEXT_INPUT) {
                size_t len = strlen(console_cmd.saisie);
                size_t remain = sizeof(console_cmd.saisie) - len - 1;
                if (remain > 0) {
                    strncat(console_cmd.saisie, event.text.text, remain);
                }
                continue;
            }

            if (event.type == SDL_EVENT_QUIT){
                running = false;
                code_sortie = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Window *window = SDL_GetRenderWindow(renderer);

                if (event.key.key == SDLK_F1) {
                    console_cmd.ouvert = !console_cmd.ouvert;
                    if (window) {
                        if (console_cmd.ouvert) SDL_StartTextInput(window);
                        else SDL_StopTextInput(window);
                    }
                    continue;
                }

                if (!console_cmd.ouvert && event.key.key == SDLK_SLASH) {
                    console_cmd.ouvert = 1;
                    SDL_snprintf(console_cmd.saisie, sizeof(console_cmd.saisie), "/");
                    if (window) SDL_StartTextInput(window);
                    continue;
                }

                if (console_cmd.ouvert) {
                    if (event.key.key == SDLK_ESCAPE) {
                        console_cmd.ouvert = 0;
                        if (window) SDL_StopTextInput(window);
                        continue;
                    }

                    if (event.key.key == SDLK_BACKSPACE) {
                        size_t len = strlen(console_cmd.saisie);
                        if (len > 0) {
                            console_cmd.saisie[len - 1] = '\0';
                        }
                        continue;
                    }

                    if (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER) {
                        console_executer_commande(console_cmd.saisie, renderer);
                        console_cmd.saisie[0] = '\0';
                        continue;
                    }

                    continue;
                }

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
                if(event.key.key == SDLK_I){
                    inventaire_ouvert = !inventaire_ouvert;
                }
                if(event.key.key == SDLK_E) {
                    if (planete == 2 && texture_alien_planete2 && !planete2_simon_termine) {
                        float alien_planete2_x = planete2_mastermind_engrenage_donne ? alien_planete2_x_2 : alien_planete2_x_1;
                        float alien_planete2_y = planete2_mastermind_engrenage_donne ? alien_planete2_y_2 : alien_planete2_y_1;
                        SDL_FRect rect_perso_alien = {cx - perso.x, cy - perso.y, 40.0f, 60.0f};
                        SDL_FRect rect_alien = {alien_planete2_x, alien_planete2_y, alien_planete2_w, alien_planete2_h};
                        if (SDL_HasRectIntersectionFloat(&rect_perso_alien, &rect_alien)) {
                            if (!planete2_mastermind_engrenage_donne) {
                                mastermind(renderer);
                                if (g_mastermind_reussi) {
                                    planete2_mastermind_engrenage_donne = true;
                                }
                            } else {
                                simon(renderer);
                                if (g_simon_reussi) {
                                    planete2_simon_termine = true;
                                }
                            }
                            continue;
                        }
                    }

                    SDL_FRect rect_perso_vaisseau = {cx - perso.x, cy - perso.y, 40.0f, 60.0f};
                    SDL_FRect rect_vaisseau = {vaisseau_collision_x, vaisseau_collision_y, vaisseau_collision_w, vaisseau_collision_h};
                    if (vaisseau_repare && SDL_HasRectIntersectionFloat(&rect_perso_vaisseau, &rect_vaisseau)) {
                        running = false;
                        code_sortie = 4;
                    }

                    /* Gestion barrière 4 : ouvrir si joueur a 2+ clés */
                    if (planete == 2 && !planete2_barriere_4_ouverte) {
                        float rock4_center_x = rock4_center_x_base;
                        float rock4_center_y = rock4_center_y_base;
                        // Convertir les coordonnées écran du joueur en coordonnées monde
                        float perso_world_x = cx - perso.x;
                        float perso_world_y = cy - perso.y;
                        SDL_FRect rect_perso_barrier = {perso_world_x - 20.0f, perso_world_y - 20.0f, 80.0f, 100.0f};
                        // Barrière 4 : centre + dessous (2 tuiles)
                        SDL_FRect rect_barrier4 = {rock4_center_x - 45.0f, rock4_center_y - 45.0f, DISPLAY_TILE_SIZE * 3.0f, DISPLAY_TILE_SIZE * 3.0f};
                        if (SDL_HasRectIntersectionFloat(&rect_perso_barrier, &rect_barrier4)) {
                            int cles = compter_cles();
                            if (cles >= 2) {
                                planete2_barriere_4_ouverte = true;
                                // Retirer 2 clés de l'inventaire
                                int cles_a_retirer = 2;
                                for (int i = 0; i < HOTBAR_SIZE && cles_a_retirer > 0; i++) {
                                    if (hotbar[i] != NULL && hotbar[i]->item != NULL && hotbar[i]->item->type == CLE) {
                                        if (hotbar[i]->quantiter >= cles_a_retirer) {
                                            hotbar[i]->quantiter -= cles_a_retirer;
                                            cles_a_retirer = 0;
                                        } else {
                                            cles_a_retirer -= hotbar[i]->quantiter;
                                            hotbar[i]->quantiter = 0;
                                            free(hotbar[i]->item);
                                            free(hotbar[i]);
                                            hotbar[i] = NULL;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    SDL_Rect rect_perso_caisse = {(int)cx, (int)cy, 40, 60};
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
                                        drop->x = -perso.x + cx;
                                        drop->y = -perso.y + cy;
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
                                SDL_FRect rect_perso_vaiseau = {cx - perso.x, cy - perso.y, 40.0f, 60.0f};
                                SDL_FRect rect_vaisseau = {vaisseau_collision_x, vaisseau_collision_y, vaisseau_collision_w, vaisseau_collision_h};
                                if (!vaisseau_repare && SDL_HasRectIntersectionFloat(&rect_perso_vaiseau, &rect_vaisseau)) {
                                    // Chercher 1 engrenage dans la hotbar
                                    int engrenage_slot = -1;
                                    for (int i = 0; i < HOTBAR_SIZE; i++) {
                                        if (hotbar[i] != NULL && hotbar[i]->item != NULL && hotbar[i]->item->type == ENGRENAGE) {
                                            engrenage_slot = i;
                                            break;
                                        }
                                    }
                                    if (engrenage_slot >= 0) {
                                        // Consommer 1 engrenage
                                        hotbar[engrenage_slot]->quantiter--;
                                        if (hotbar[engrenage_slot]->quantiter <= 0) {
                                            free(hotbar[engrenage_slot]->item);
                                            free(hotbar[engrenage_slot]);
                                            hotbar[engrenage_slot] = NULL;
                                        }
                                        engrenages_poses++;
                                        if (engrenages_poses >= nb_engrenages_requis) {
                                            vaisseau_repare = true;
                                            SDL_DestroyTexture(exterieure);
                                            exterieure = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/vaisseau_non_casser.png");
                                            exterieure_reparee = 1;
                                        }
                                    }
                                }
                            } else if (outil->type == PIECE) {
                                argent += 1;
                                hotbar[k]->quantiter--;
                                if (hotbar[k]->quantiter <= 0) { free(hotbar[k]->item); free(hotbar[k]); hotbar[k] = NULL; }

                            }else if(outil->type == BRIQUET){
                                float perso_monde_x = -perso.x + cx;
                                float perso_monde_y = -perso.y + cy;
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
                                float perso_monde_x = -perso.x + cx;
                                float perso_monde_y = -perso.y + cy;
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

        if (!console_cmd.ouvert && !caisse_outils_ouvert && !inventaire_ouvert) {
            deplacer_perso(delta);
        }

        SDL_Rect hitbox = {
            .x = (int)(cx - perso.x) + 30,
            .y = (int)(cy - perso.y) + 60,
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

        /* Trois rochers (planète 2) : centre au-dessus du premier alien + un à gauche et un à droite
           Tous bloquent tant que la première épreuve (mastermind) n'est pas réussie
        */
        if (!collision_trouve && planete == 2 && !planete2_mastermind_engrenage_donne && texture_rocher) {
            float rock_center_x = rock_center_x_base;
            float rock_center_y = rock_center_y_base;
            float rock_left_x = rock_center_x - (1.0f * DISPLAY_TILE_SIZE);
            float rock_right_x = rock_center_x + (1.0f * DISPLAY_TILE_SIZE);

            /* Collision en coordonnées monde (comme rect_alien_collision) : ne pas ajouter perso.x/perso.y */
            SDL_Rect rock_center_i = {(int)rock_center_x, (int)rock_center_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            SDL_Rect rock_left_i = {(int)rock_left_x, (int)rock_center_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            SDL_Rect rock_right_i = {(int)rock_right_x, (int)rock_center_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            if (SDL_HasRectIntersection(&hitbox, &rock_center_i) || SDL_HasRectIntersection(&hitbox, &rock_left_i) || SDL_HasRectIntersection(&hitbox, &rock_right_i)) {
                collision_trouve = true;
            }

            /* seconde barrière : centre + haut + bas */
            float rock2_center_x = rock2_center_x_base;
            float rock2_center_y = rock2_center_y_base;
            float rock2_up_y = rock2_center_y - (1.0f * DISPLAY_TILE_SIZE);
            float rock2_down_y = rock2_center_y + (1.0f * DISPLAY_TILE_SIZE);
            float rock2_down2_y = rock2_center_y + (2.0f * DISPLAY_TILE_SIZE);
            SDL_Rect rock2_center_i = {(int)rock2_center_x, (int)rock2_center_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            SDL_Rect rock2_up_i = {(int)rock2_center_x, (int)rock2_up_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            SDL_Rect rock2_down_i = {(int)rock2_center_x, (int)rock2_down_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            SDL_Rect rock2_down2_i = {(int)rock2_center_x, (int)rock2_down2_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            if (SDL_HasRectIntersection(&hitbox, &rock2_center_i) || SDL_HasRectIntersection(&hitbox, &rock2_up_i) || SDL_HasRectIntersection(&hitbox, &rock2_down_i) || SDL_HasRectIntersection(&hitbox, &rock2_down2_i)) {
                collision_trouve = true;
            }
        }

        /* troisième barrière : bloque le second alien tant que simon n'est pas terminé */
        if (!collision_trouve && planete == 2 && !planete2_simon_termine && texture_rocher) {
            float rock3_center_x = rock3_center_x_base;
            float rock3_center_y = rock3_center_y_base;
            float rock3_below_y = rock3_center_y + (1.0f * DISPLAY_TILE_SIZE);
            SDL_Rect rock3_center_i = {(int)rock3_center_x, (int)rock3_center_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            SDL_Rect rock3_below_i = {(int)rock3_center_x, (int)rock3_below_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            if (SDL_HasRectIntersection(&hitbox, &rock3_center_i) || SDL_HasRectIntersection(&hitbox, &rock3_below_i)) {
                collision_trouve = true;
            }
        }

        /* quatrième barrière : permanente, bloque jusqu'à avoir 2 clés et ouvrir */
        if (!collision_trouve && planete == 2 && !planete2_barriere_4_ouverte && texture_rocher) {
            float rock4_center_x = rock4_center_x_base;
            float rock4_center_y = rock4_center_y_base;
            float rock4_below_y = rock4_center_y + (1.0f * DISPLAY_TILE_SIZE);
            SDL_Rect rock4_center_i = {(int)rock4_center_x, (int)rock4_center_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            SDL_Rect rock4_below_i = {(int)rock4_center_x, (int)rock4_below_y, (int)DISPLAY_TILE_SIZE, (int)DISPLAY_TILE_SIZE};
            if (SDL_HasRectIntersection(&hitbox, &rock4_center_i) || SDL_HasRectIntersection(&hitbox, &rock4_below_i)) {
                collision_trouve = true;
            }
        }

        if (collision_trouve) {
            perso.x = old_x;
            perso.y = old_y;
        }

        SDL_FRect rect_vaisseau_collision = {
            vaisseau_collision_x,
            vaisseau_collision_y,
            vaisseau_collision_w,
            vaisseau_collision_h
        };
        SDL_FRect hitbox_float = {(float)hitbox.x, (float)hitbox.y, (float)hitbox.w, (float)hitbox.h};
        if (SDL_HasRectIntersectionFloat(&hitbox_float, &rect_vaisseau_collision)) {
            perso.x = old_x;
            perso.y = old_y;
        }

        if (planete == 2 && texture_alien_planete2 && !planete2_simon_termine) {
            float alien_planete2_x = planete2_mastermind_engrenage_donne ? alien_planete2_x_2 : alien_planete2_x_1;
            float alien_planete2_y = planete2_mastermind_engrenage_donne ? alien_planete2_y_2 : alien_planete2_y_1;
            SDL_FRect rect_alien_collision = {
                alien_planete2_x + 4.0f,
                alien_planete2_y + 4.0f,
                alien_planete2_w - 8.0f,
                alien_planete2_h - 8.0f
            };
            if (SDL_HasRectIntersectionFloat(&hitbox_float, &rect_alien_collision)) {
                perso.x = old_x;
                perso.y = old_y;
            }
        }

        SDL_SetRenderDrawColor(renderer, 71, 171, 169, 255);
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



        charger_tilemap(renderer, tileset, map, foam, planete);

        /* Dessiner la barrière de rochers (si active) */
        if (planete == 2 && texture_rocher && !planete2_mastermind_engrenage_donne) {
            float rock_center_x = rock_center_x_base;
            float rock_center_y = rock_center_y_base;
            float rock_left_x = rock_center_x - (1.0f * DISPLAY_TILE_SIZE);
            float rock_right_x = rock_center_x + (1.0f * DISPLAY_TILE_SIZE);

            SDL_FRect dest_center = { rock_center_x + perso.x, rock_center_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_FRect dest_left = { rock_left_x + perso.x, rock_center_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_FRect dest_right = { rock_right_x + perso.x, rock_center_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };

            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest_left);
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest_center);
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest_right);
            /* dessiner seconde barrière : centre + haut + bas */
            float rock2_center_x = rock2_center_x_base;
            float rock2_center_y = rock2_center_y_base;
            float rock2_up_y = rock2_center_y - (1.0f * DISPLAY_TILE_SIZE);
            float rock2_down_y = rock2_center_y + (1.0f * DISPLAY_TILE_SIZE);
            float rock2_down2_y = rock2_center_y + (2.0f * DISPLAY_TILE_SIZE);
            SDL_FRect dest2_center = { rock2_center_x + perso.x, rock2_center_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_FRect dest2_up = { rock2_center_x + perso.x, rock2_up_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_FRect dest2_down = { rock2_center_x + perso.x, rock2_down_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_FRect dest2_down2 = { rock2_center_x + perso.x, rock2_down2_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest2_up);
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest2_center);
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest2_down);
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest2_down2);
        }

        /* Dessiner la troisième barrière pour le second alien (disparaît quand simon est terminé) */
        if (planete == 2 && texture_rocher && !planete2_simon_termine) {
            float rock3_center_x = rock3_center_x_base;
            float rock3_center_y = rock3_center_y_base;
            float rock3_below_y = rock3_center_y + (1.0f * DISPLAY_TILE_SIZE);
            SDL_FRect dest3_center = { rock3_center_x + perso.x, rock3_center_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_FRect dest3_below = { rock3_center_x + perso.x, rock3_below_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest3_center);
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest3_below);
        }

        /* Dessiner la quatrième barrière (permanente jusqu'à ouvrir avec 2 clés) */
        if (planete == 2 && texture_rocher && !planete2_barriere_4_ouverte) {
            float rock4_center_x = rock4_center_x_base;
            float rock4_center_y = rock4_center_y_base;
            float rock4_below_y = rock4_center_y + (1.0f * DISPLAY_TILE_SIZE);
            SDL_FRect dest4_center = { rock4_center_x + perso.x, rock4_center_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_FRect dest4_below = { rock4_center_x + perso.x, rock4_below_y + perso.y, (float)DISPLAY_TILE_SIZE, (float)DISPLAY_TILE_SIZE };
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest4_center);
            SDL_RenderTexture(renderer, texture_rocher, NULL, &dest4_below);

            /* Afficher le message seulement si le joueur est proche (distance < 150 pixels) */
            float dx = (cx - perso.x) - rock4_center_x;
            float dy = (cy - perso.y) - rock4_center_y;
            float distance = SDL_sqrtf(dx * dx + dy * dy);
            if (distance < 150.0f) {
                int cles = compter_cles();
                const char *msg_barrier4 = cles >= 2 ? "Appuyer sur E pour ouvrir le passage" : "Vous avez besoin de deux cles pour ouvrir";
                TTF_Font *font_barrier = TTF_OpenFont("assets/police.ttf", 18);
                if (font_barrier) {
                    SDL_Color couleur_texte = {255, 255, 255, 255};
                    SDL_Surface *surf = TTF_RenderText_Solid(font_barrier, msg_barrier4, strlen(msg_barrier4), couleur_texte);
                    if (surf) {
                        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                        SDL_FRect rect_msg = {
                            rock4_center_x + perso.x - (float)surf->w / 2.0f,
                            rock4_center_y + perso.y - 40.0f,
                            (float)surf->w,
                            (float)surf->h
                        };
                        SDL_RenderTexture(renderer, tex, NULL, &rect_msg);
                        SDL_DestroyTexture(tex);
                        SDL_DestroySurface(surf);
                    }
                    TTF_CloseFont(font_barrier);
                }
            }
        }

        if (planete == 2 && texture_alien_planete2 && !planete2_simon_termine) {
            float alien_planete2_x = planete2_mastermind_engrenage_donne ? alien_planete2_x_2 : alien_planete2_x_1;
            float alien_planete2_y = planete2_mastermind_engrenage_donne ? alien_planete2_y_2 : alien_planete2_y_1;
            SDL_FRect dest_alien = {
                alien_planete2_x + perso.x,
                alien_planete2_y + perso.y,
                DISPLAY_TILE_SIZE * 0.85f,
                DISPLAY_TILE_SIZE * 0.85f
            };
            SDL_RenderTexture(renderer, texture_alien_planete2, NULL, &dest_alien);
        }
       

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


        update_combat(map, mobs, renderer, items);
        update_mobs(map, mobs);

        if (vaisseau_repare && !exterieure_reparee) {
            SDL_DestroyTexture(exterieure);
            exterieure = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/vaisseau_non_casser.png");
            exterieure_reparee = 1;
        }

        if (planete == 3 && g_planete3_engrenage_recupere && !g_planete3_boss_spawned) {
            boss_set_navigation_map(map);
            /* Spawn legerement en dessous du vaisseau pour eviter d'etre coince dans sa hitbox */
            float spawn_boss_x = 488 + (VAISSEAU_WIDTH * 0.8f) ;
            float spawn_boss_y = 481 ;
            init_boss(renderer, &boss3, TYPE_BOSS_MINOTAURE,
                      spawn_boss_x, spawn_boss_y);
            g_planete3_boss_spawned = true;
            hud_set_message("Un boss est apparu pres du vaisseau !", 2500);
        }

        if (boss_actif && !(planete == 3 && !g_planete3_boss_spawned) && !(planete == 2 && boss1.est_battu)) {
            mettre_a_jour_boss(renderer, boss_actif);
        }
        possible_ramasser_item(items, renderer, hotbar);



        SDL_FRect src_caisse_outils = {0, 0, 64, 64};
        SDL_FRect dest_caisse_outils = {10*DISPLAY_TILE_SIZE + perso.x, 11*DISPLAY_TILE_SIZE + perso.y, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE};
        if(planete == 1)SDL_RenderTexture(renderer, texture_caisse_outils, &src_caisse_outils, &dest_caisse_outils);

        afficher_item(items, renderer);

        SDL_FRect src_vaiseaux = {0, 0, VAISSEAU_WIDTH, VAISSEAU_HEIGHT};
        SDL_FRect dest_vaisseau = {vaisseau_world_x + perso.x, vaisseau_world_y + perso.y, VAISSEAU_WIDTH, VAISSEAU_HEIGHT};
        SDL_RenderTexture(renderer, exterieure, &src_vaiseaux, &dest_vaisseau);

        if (combat_en_cours == false) afficher_perso(renderer);


        afficher_mob(renderer, mobs);

        if (planete == 3) {
            appliquer_vision_reduite_planete3(renderer, cx, cy, sw, sh);
        }

        if (boss_actif && !(planete == 3 && !g_planete3_boss_spawned) && !(planete == 2 && boss1.est_battu)) {
            afficher_boss(renderer, boss_actif);
        }

        /* Affichage d'un message HUD temporaire (2-3 secondes) */
        if (g_message_hud[0] != '\0' && SDL_GetTicks() < g_message_hud_expire) {
            TTF_Font *font_msg = TTF_OpenFont("assets/police.ttf", 26);
            if (font_msg) {
                SDL_Color jaune = {255, 230, 80, 255};
                const char *txt = g_message_hud;
                SDL_Surface *surf = TTF_RenderText_Solid(font_msg, txt, (int)SDL_strlen(txt), jaune);
                if (surf) {
                    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                    if (tex) {
                        SDL_FRect rect = {
                            screen_center_x() - ((float)surf->w * 0.5f),
                            screen_center_y() - 120.0f,
                            (float)surf->w,
                            (float)surf->h
                        };
                        SDL_RenderTexture(renderer, tex, NULL, &rect);
                        SDL_DestroyTexture(tex);
                    }
                    SDL_DestroySurface(surf);
                }
                TTF_CloseFont(font_msg);
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
                        (sw - surf->w) / 2.0f,
                        400.0f,
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


        // Compteur d'engrenages au-dessus du vaisseau
        TTF_Font *font_eng = TTF_OpenFont("assets/police.ttf", 20);
        if (font_eng) {
            char texte_eng[64];
            SDL_Color couleur_eng;
            if (vaisseau_repare) {
                SDL_snprintf(texte_eng, sizeof(texte_eng), "Vaisseau repare !");
                couleur_eng = (SDL_Color){0, 255, 100, 255};
            } else {
                SDL_snprintf(texte_eng, sizeof(texte_eng), "Vaisseau : %d/%d engrenages", engrenages_poses, nb_engrenages_requis);
                couleur_eng = (SDL_Color){255, 200, 50, 255};
            }
            SDL_Surface *surf_eng = TTF_RenderText_Solid(font_eng, texte_eng, strlen(texte_eng), couleur_eng);
            if (surf_eng) {
                SDL_Texture *tex_eng = SDL_CreateTextureFromSurface(renderer, surf_eng);
                float vx = vaisseau_world_x + perso.x + (VAISSEAU_WIDTH - surf_eng->w) / 2.0f;
                float vy = vaisseau_world_y + perso.y - 30.0f;
                SDL_FRect rect_eng = { vx, vy, (float)surf_eng->w, (float)surf_eng->h };
                SDL_RenderTexture(renderer, tex_eng, NULL, &rect_eng);
                SDL_DestroyTexture(tex_eng);
                SDL_DestroySurface(surf_eng);
            }
            TTF_CloseFont(font_eng);
        }

        // Message quand le joueur est proche du vaisseau
        {
            SDL_FRect rect_perso_vaisseau = {cx - perso.x, cy - perso.y, 40.0f, 60.0f};
            SDL_FRect rect_vaisseau = {vaisseau_collision_x, vaisseau_collision_y, vaisseau_collision_w, vaisseau_collision_h};
            if (SDL_HasRectIntersectionFloat(&rect_perso_vaisseau, &rect_vaisseau)) {
                TTF_Font *font_hint = TTF_OpenFont("assets/police.ttf", 20);
                if (font_hint) {
                    char hint[128];
                    SDL_Color col_hint;

                    if (vaisseau_repare) {
                        SDL_snprintf(hint, sizeof(hint), "Appuyez sur E pour entrer dans le vaisseau");
                        col_hint = (SDL_Color){0, 255, 100, 255};
                    } else {
                        bool un_marteau = false, un_engrenage = false;
                        for (int si = 0; si < HOTBAR_SIZE; si++) {
                            if (hotbar[si] && hotbar[si]->item) {
                                if (hotbar[si]->item->type == MARTEAU)   un_marteau   = true;
                                if (hotbar[si]->item->type == ENGRENAGE) un_engrenage = true;
                            }
                        }

                        if (un_marteau && un_engrenage) {
                            SDL_snprintf(hint, sizeof(hint), "Appuyez sur la touche du marteau pour poser un engrenage");
                            col_hint = (SDL_Color){255, 255, 100, 255};
                        } else if (un_marteau) {
                            SDL_snprintf(hint, sizeof(hint), "Il vous faut des engrenages (%d/%d poses)", engrenages_poses, nb_engrenages_requis);
                            col_hint = (SDL_Color){255, 100, 100, 255};
                        } else {
                            SDL_snprintf(hint, sizeof(hint), "Equipez un marteau pour reparer le vaisseau");
                            col_hint = (SDL_Color){200, 200, 200, 255};
                        }
                    }

                    SDL_Surface *surf_hint = TTF_RenderText_Solid(font_hint, hint, strlen(hint), col_hint);
                    if (surf_hint) {
                        SDL_Texture *tex_hint = SDL_CreateTextureFromSurface(renderer, surf_hint);
                        SDL_FRect rect_hint = {
                            (sw - surf_hint->w) / 2.0f, sh - 130.0f,
                            (float)surf_hint->w, (float)surf_hint->h
                        };
                        SDL_RenderTexture(renderer, tex_hint, NULL, &rect_hint);
                        SDL_DestroyTexture(tex_hint);
                        SDL_DestroySurface(surf_hint);
                    }
                    TTF_CloseFont(font_hint);
                }
            }
        }

        afficher_combat(renderer);

        Uint32 cycle_etat = (SDL_GetTicks() - cycle_debut) % CYCLE_MS;
        float phase = (float)cycle_etat / (float)CYCLE_MS;
        float mod = (phase < 0.5f) ? (phase * 2.0f) : ((1.0f - phase) * 2.0f);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(mod * 140.0f));
        SDL_RenderFillRect(renderer, NULL);

        afficher_hotbar(hotbar, renderer);

        if (!console_god_mode && (maintenant - faim_timer) > 20000) {
            if (perso.faim > 0) perso.faim--;
            faim_timer = maintenant;
        }

        if(!console_god_mode && perso.faim == 0 && (maintenant-faim_degat_timer)>3000){
            if((rand()%100<20)){
                jouer_son("assets/audio/dammage.wav", 0.2f);
                perso.vie--;
            }
            faim_degat_timer = maintenant;
        }

        if (!console_god_mode && perso.faim >= perso.faim_max && perso.vie < perso.vie_max) {
            if ((maintenant - faim_regen_timer) > 5000) {
                perso.vie++;
                faim_regen_timer = maintenant;
            }
        } else {
            faim_regen_timer = maintenant;
        }

        afficher_stat(renderer);
            objectifs_afficher(&objectifs_jeu,renderer,font_objectifs);
            objectifs_sauvegarder_etat(&objectifs_jeu, planete);
            /* Donner un engrenage si tous les objectifs de la planete 1 sont completes (une seule fois) */
            if (planete == 1 && !planete1_engrenage_objectifs_donne) {
                int valides = 0;
                for (int i = 0; i < objectifs_jeu.nb; ++i) if (objectifs_jeu.objectifs[i].valide) ++valides;
                if (valides >= objectifs_jeu.nb) {
                    if (index_item < MAX_ITEMS) {
                        float give_x = -perso.x + screen_center_x();
                        float give_y = -perso.y + screen_center_y();
                        t_Item *engrenage_obj = init_item(ENGRENAGE, renderer, give_x, give_y);
                        if (engrenage_obj != NULL) {
                            items[index_item++] = engrenage_obj;
                            /* recompense eng: pas de message centre */
                        }
                    }
                    planete1_engrenage_objectifs_donne = true;
                }
            }

            /* Meme logique pour la planete 2: 1 engrenage quand tous les objectifs sont completes. */
            if (planete == 2 && !planete2_engrenage_objectifs_donne) {
                int valides = 0;
                for (int i = 0; i < objectifs_jeu.nb; ++i) if (objectifs_jeu.objectifs[i].valide) ++valides;
                if (valides >= objectifs_jeu.nb) {
                    if (index_item < MAX_ITEMS) {
                        float give_x = -perso.x + screen_center_x();
                        float give_y = -perso.y + screen_center_y();
                        t_Item *engrenage_obj = init_item(ENGRENAGE, renderer, give_x, give_y);
                        if (engrenage_obj != NULL) {
                            items[index_item++] = engrenage_obj;
                        }
                    }
                    planete2_engrenage_objectifs_donne = true;
                }
            }

            /* Meme logique pour la planete 3: 1 engrenage quand tous les objectifs sont completes. */
            if (planete == 3 && !planete3_engrenage_objectifs_donne) {
                int valides = 0;
                for (int i = 0; i < objectifs_jeu.nb; ++i) if (objectifs_jeu.objectifs[i].valide) ++valides;
                if (valides >= objectifs_jeu.nb) {
                    if (index_item < MAX_ITEMS) {
                        float give_x = -perso.x + screen_center_x();
                        float give_y = -perso.y + screen_center_y();
                        t_Item *engrenage_obj = init_item(ENGRENAGE, renderer, give_x, give_y);
                        if (engrenage_obj != NULL) {
                            items[index_item++] = engrenage_obj;
                        }
                    }
                    planete3_engrenage_objectifs_donne = true;
                }
            }

        if (planete == 2 && texture_alien_planete2 && !planete2_simon_termine && !console_cmd.ouvert) {
            float alien_planete2_x = planete2_mastermind_engrenage_donne ? alien_planete2_x_2 : alien_planete2_x_1;
            float alien_planete2_y = planete2_mastermind_engrenage_donne ? alien_planete2_y_2 : alien_planete2_y_1;
            SDL_FRect rect_perso_alien = {cx - perso.x, cy - perso.y, 40.0f, 60.0f};
            SDL_FRect rect_alien_zone = {
                alien_planete2_x - 24.0f,
                alien_planete2_y - 24.0f,
                alien_planete2_w + 48.0f,
                alien_planete2_h + 48.0f
            };
            if (SDL_HasRectIntersectionFloat(&rect_perso_alien, &rect_alien_zone)) {
                if (!planete2_mastermind_engrenage_donne) {
                    SDL_snprintf(message_interaction, sizeof(message_interaction), "Appuyez sur E pour lancer la premiere epreuve");
                } else {
                    SDL_snprintf(message_interaction, sizeof(message_interaction), "Appuyez sur E pour lancer la derniere epreuve");
                }
                afficher_message = true;
            }
        }

        if (afficher_message) {
            float alien_planete2_x = planete2_mastermind_engrenage_donne ? alien_planete2_x_2 : alien_planete2_x_1;
            float alien_planete2_y = planete2_mastermind_engrenage_donne ? alien_planete2_y_2 : alien_planete2_y_1;
            afficher_message_interaction(renderer,
                                         message_interaction,
                                         alien_planete2_x + perso.x + (alien_planete2_w * 0.5f),
                                         alien_planete2_y + perso.y - 28.0f);
        }

        if (console_cmd.ouvert) {
            console_afficher(renderer);
        } else if (console_cmd.message[0] != '\0' && SDL_GetTicks() < console_cmd.message_expire && console_cmd.font) {
            SDL_Color info_col = {220, 240, 255, 255};
            SDL_Surface *s_info = TTF_RenderText_Solid(console_cmd.font, console_cmd.message, strlen(console_cmd.message), info_col);
            if (s_info) {
                SDL_Texture *t_info = SDL_CreateTextureFromSurface(renderer, s_info);
                SDL_FRect r_info = {20.0f, sh - 38.0f, (float)s_info->w, (float)s_info->h};
                SDL_RenderTexture(renderer, t_info, NULL, &r_info);
                SDL_DestroyTexture(t_info);
                SDL_DestroySurface(s_info);
            }
        }

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
                        sw - 220.0f,
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

        if (console_god_mode) {
            perso.vie = perso.vie_max;
            perso.faim = perso.faim_max;
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
            SDL_FRect dest = {0.0f, 0.0f, sw, sh};
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
        if(perso.vie == 0) {jouer_son("assets/audio/game_over.mp3", 0.2f); running = game_over(renderer, planete);}
    }

    if (code_sortie != 4 && font_objectifs) { TTF_CloseFont(font_objectifs); font_objectifs = NULL; }
    if (console_cmd.ouvert) {
        SDL_Window *window = SDL_GetRenderWindow(renderer);
        if (window) SDL_StopTextInput(window);
        console_cmd.ouvert = 0;
    }
    if (console_cmd.font) {
        TTF_CloseFont(console_cmd.font);
        console_cmd.font = NULL;
    }
    SDL_DestroyTexture(exterieure);
    SDL_DestroyTexture(texture_caisse_outils);
    if (texture_alien_planete2) SDL_DestroyTexture(texture_alien_planete2);
    if (texture_rocher) SDL_DestroyTexture(texture_rocher);
    if (code_sortie != 4) {
        detruire_mobs(mobs);
        detruire_boss(boss_actif);
    }
    SDL_DestroyTexture(tileset);
    return code_sortie;
}
 
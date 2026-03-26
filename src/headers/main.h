#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

#define FRAME_DUREE 50

#define W_MAP 40
#define H_MAP 40
#define SOURCE_TILE_SIZE 64
#define DISPLAY_TILE_SIZE 90

#define MAX_MOB 1000
#define MAX_ITEMS 200

#define HOTBAR_SIZE 5
#define INVENTAIRE_COLS 5
#define INVENTAIRE_ROWS 4
#define INVENTAIRE_SIZE (INVENTAIRE_COLS * INVENTAIRE_ROWS)
#define CAISSE_OUTILS_SIZE 6

typedef enum {vide, terreP, eau, terreCHG, terreCHD, terreCBG, terreCBD, terreH, terreB, terreG, terreD, feu, pierre, arbrecoupe, arbreEntier} type_t;

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

typedef struct {
    float x;
    float y;
    SDL_Texture *texture;
    int direction; // 0=south,1=north,2=west,3=east
    int vie;
    int vie_max;
    int faim;
    int faim_max;
    Uint32 invincibiliter_timer;
} Perso;

typedef enum {PIECE,VIANDE,MARTEAU,SOIN,BRIQUET,BOIS,FEUDECAMP,VIANDECUITE,ENGRENAGE} typeItem;

typedef struct item_ {
    typeItem type;
    SDL_Texture * texture;
    float x;
    float y;
}t_Item;

typedef struct {
    float x;
    float y;
    int direction;
    float vx;
    float vy;
    float target_vx;
    float target_vy;
    float speed;
    float largeur,hauteur;
    Uint32 time_change_dir;
    SDL_Texture * texture;
    int vie;
    int id; //1 = mouton, 2 = pawns
    int drop_chance;
}Mob;

typedef struct {
    float x;
    float y;
    int est_battu;
    int vie;
    int vie_max;
    int attaque;
    Uint32 cooldown_attaque;
    int animation_frame_idle;
    int animation_frame_attack;
    int animation_frame_death;
    Uint32 animation_timer;
    int phase;
    SDL_Texture * texture;
    int animation_state; // 0: idle, 1: attack, 2: death
}boss_t;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    t_Item * item;
    int stackable;
    int quantiter;
}t_case;


#include "menu.h"
#include "game_over.h"
#include "jeux.h"
#include "bouton.h"
#include "option.h"
#include "perso.h"
#include "son.h"
#include "boss.h"
#include "mob.h"
#include "vaisseau.h"
#include "map.h"
#include "item.h"
#include "inventaire.h"
#include "sauvegarde.h"


extern Perso perso;
extern int animation_frame;   
extern Uint32 animation_timer; 
extern Uint32 bouge_timer;
extern bool perso_bouge;
extern bool combat_en_cours;
extern int combat_frame;
extern Uint32 combat_timer;

extern MIX_Track *track_global;

extern int Planete_actuelle;

extern int index_item;

extern t_case *inventaire[INVENTAIRE_SIZE];
extern bool inventaire_ouvert;
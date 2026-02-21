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
    Uint32 invincibiliter_timer;
} Perso;



#include "menu.h"
#include "jeux.h"
#include "bouton.h"
#include "option.h"
#include "perso.h"
#include "son.h"
#include "mob.h"
#include "vaisseau.h"

extern Perso perso;
extern int animation_frame;   
extern Uint32 animation_timer; 
extern Uint32 bouge_timer;
extern bool perso_bouge;
extern bool combat_en_cours;
extern int combat_frame;
extern Uint32 combat_timer;

extern MIX_Track *track_global;


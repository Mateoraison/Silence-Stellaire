#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "menu.h"
#include "jeux.h"
#include "bouton.h"
#include "option.h"
#include "perso.h"

#define FRAME_DUREE 50

typedef struct {
    float x;
    float y;
    SDL_Texture *texture;
    int direction; // 0=south,1=north,2=west,3=east
} Perso;


extern Perso perso;
extern int animation_frame;   
extern Uint32 animation_timer; 
extern Uint32 bouge_timer;
extern bool perso_bouge;



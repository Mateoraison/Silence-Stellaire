#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

void init_mobs(SDL_Renderer * renderer, Mob * mobs[MAX_MOB]);
void update_mobs(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB]);
void afficher_mob(SDL_Renderer * renderer, Mob * mobs[MAX_MOB]);
void detruire_mobs(Mob * mobs[MAX_MOB]);
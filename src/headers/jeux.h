#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

int jeu_principal(SDL_Renderer *renderer, int planete, MIX_Track *track_global, bool reprendre_partie);
void init_caisse_outils(SDL_Renderer *renderer);

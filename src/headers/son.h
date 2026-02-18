#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>



MIX_Track * jouer_son(const char* chemin);

void pause_son(MIX_Track *track);

void reprendre_son(MIX_Track *track);
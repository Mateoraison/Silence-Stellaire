#ifndef PAUSE_H
#define PAUSE_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

// Codes de retour de afficher_pause()
#define PAUSE_REPRENDRE   0
#define PAUSE_MENU        1
#define PAUSE_OPTIONS     2
#define PAUSE_SAUVEGARDE  3 

int afficher_pause(SDL_Renderer *renderer, MIX_Track *track_global);

#endif

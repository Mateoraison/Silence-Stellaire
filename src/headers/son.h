#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>



MIX_Track * jouer_son(const char* chemin, float volume);

void pause_son(MIX_Track *track);

void reprendre_son(MIX_Track *track);

int son_est_actif(void);

void son_definir_actif(int actif);

int son_ambiance_est_active(void);

void son_definir_ambiance_active(int actif);

int son_sfx_est_actif(void);

void son_definir_sfx_active(int actif);

void son_precharger_sfx(void);

void son_pause_tout(void);
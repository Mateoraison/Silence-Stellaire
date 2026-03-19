#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "headers/main.h"


MIX_Track *jouer_son(const char* chemin) {
    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if(!mixer){
        SDL_Log("Erreur creation mixer : %s", SDL_GetError());
        return NULL;
    }

    MIX_Audio *son = MIX_LoadAudio(mixer, chemin, true);
    if(!son){
        SDL_Log("Erreur chargement son : %s", SDL_GetError());
        return NULL;
    }

    MIX_Track *track = MIX_CreateTrack(mixer);
    if(!track){
        SDL_Log("Erreur creation track : %s", SDL_GetError());
        return NULL;
    }

    MIX_SetTrackAudio(track, son);
    MIX_SetTrackGain(track, 0.3f);
    MIX_PlayTrack(track, -1);
    return track;
}

void pause_son(MIX_Track *track) {
    MIX_PauseTrack(track);
}

void reprendre_son(MIX_Track *track) {
    MIX_ResumeTrack(track);
}
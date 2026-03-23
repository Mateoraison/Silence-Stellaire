#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "headers/main.h"
#include <string.h>

#define MAX_SONS_CACHE 16

typedef struct {
    char chemin[256];
    MIX_Audio *audio;
    MIX_Track *track;
} SonCache;

static MIX_Mixer *g_mixer = NULL;
static SonCache g_sons[MAX_SONS_CACHE];
static int g_nb_sons = 0;

static MIX_Mixer *recuperer_mixer(void) {
    if (!g_mixer) {
        g_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
        if (!g_mixer) {
            SDL_Log("Erreur creation mixer : %s", SDL_GetError());
        }
    }
    return g_mixer;
}

static SonCache *recuperer_son(const char *chemin) {
    MIX_Mixer *mixer = recuperer_mixer();
    if (!mixer || !chemin) {
        return NULL;
    }

    for (int i = 0; i < g_nb_sons; i++) {
        if (strcmp(g_sons[i].chemin, chemin) == 0) {
            return &g_sons[i];
        }
    }

    if (g_nb_sons >= MAX_SONS_CACHE) {
        SDL_Log("Cache audio plein, impossible de charger: %s", chemin);
        return NULL;
    }

    MIX_Audio *audio = MIX_LoadAudio(mixer, chemin, true);
    if (!audio) {
        SDL_Log("Erreur chargement son : %s", SDL_GetError());
        return NULL;
    }

    MIX_Track *track = MIX_CreateTrack(mixer);
    if (!track) {
        SDL_Log("Erreur creation track : %s", SDL_GetError());
        MIX_DestroyAudio(audio);
        return NULL;
    }

    if (!MIX_SetTrackAudio(track, audio)) {
        SDL_Log("Erreur assignation track audio : %s", SDL_GetError());
        MIX_DestroyTrack(track);
        MIX_DestroyAudio(audio);
        return NULL;
    }

    SonCache *slot = &g_sons[g_nb_sons++];
    SDL_strlcpy(slot->chemin, chemin, sizeof(slot->chemin));
    slot->audio = audio;
    slot->track = track;
    return slot;
}


MIX_Track *jouer_son(const char* chemin, float volume) {
    SonCache *son = recuperer_son(chemin);
    if (!son) {
        return NULL;
    }

    MIX_SetTrackGain(son->track, volume);

    int loops = 0;
    if (strcmp(chemin, "assets/audio/ambiance.wav") == 0) {
        loops = -1;
    }

    if (!MIX_PlayTrack(son->track, loops)) {
        SDL_Log("Erreur lecture track : %s", SDL_GetError());
        return NULL;
    }

    return son->track;
}

void pause_son(MIX_Track *track) {
    if (track) MIX_PauseTrack(track);
}

void reprendre_son(MIX_Track *track) {
    if (track) MIX_ResumeTrack(track);
}

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
static int g_son_actif = 1;
static MIX_Track *g_ambiance_track = NULL;
// piste spécifique pour le son du vaisseau (ne doit pas écraser la piste d'ambiance globale)
static MIX_Track *g_vaisseau_track = NULL;
static int g_ambiance_active = 1;
static int g_sfx_active = 1;

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

    if (!g_son_actif) {
        return son->track;
    }

    // Vérifier si c'est une ambiance (musique) ou un SFX
    int est_ambiance = (strstr(chemin, "ambiance") != NULL || strstr(chemin, "vaisseau") != NULL);
    if (est_ambiance && !g_ambiance_active) {
        return son->track;
    }
    if (!est_ambiance && !g_sfx_active) {
        return son->track;
    }

    int loops = 0;
    if (strcmp(chemin, "assets/audio/ambiance.wav") == 0) {
        loops = -1;
        g_ambiance_track = son->track;
    } else if (strcmp(chemin, "assets/audio/vaisseau.wav") == 0) {
        // vaisseau est joué en boucle mais ne doit pas écraser la piste d'ambiance principale
        loops = -1;
        g_vaisseau_track = son->track;
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
    if (!track || !g_son_actif) return;

    // Si c'est une piste d'ambiance (ambiance principale ou vaisseau) et que les ambiances sont désactivées,
    // ne pas la reprendre.
    if ((track == g_ambiance_track || track == g_vaisseau_track) && !g_ambiance_active) {
        return;
    }

    MIX_ResumeTrack(track);
}

int son_est_actif(void) {
    return g_son_actif;
}

void son_definir_actif(int actif) {
    g_son_actif = actif ? 1 : 0;

    for (int i = 0; i < g_nb_sons; i++) {
        if (!g_sons[i].track) {
            continue;
        }

        if (g_son_actif) {
            MIX_ResumeTrack(g_sons[i].track);
        } else {
            MIX_PauseTrack(g_sons[i].track);
        }
    }
}

int son_ambiance_est_active(void) {
    return g_ambiance_active;
}

void son_definir_ambiance_active(int actif) {
    g_ambiance_active = actif ? 1 : 0;
    
    if (g_ambiance_track) {
        if (g_ambiance_active && g_son_actif) {
            MIX_ResumeTrack(g_ambiance_track);
        } else {
            MIX_PauseTrack(g_ambiance_track);
        }
    }
}

int son_sfx_est_actif(void) {
    return g_sfx_active;
}

void son_definir_sfx_active(int actif) {
    g_sfx_active = actif ? 1 : 0;
}

void son_precharger_sfx(void) {
    recuperer_son("assets/audio/click.mp3");
    SDL_Log("SFX prechargé: click.mp3");

    recuperer_son("assets/audio/dammage.wav");
    SDL_Log("SFX prechargé: dammage.wav");

    recuperer_son("assets/audio/crash.mp3");
    SDL_Log("SFX prechargé: crash.mp3");

    recuperer_son("assets/audio/kick.mp3");
    SDL_Log("SFX prechargé: kick.mp3");

    recuperer_son("assets/audio/credit.mp3");
    SDL_Log("SFX prechargé: credit.mp3");

    recuperer_son("assets/audio/game_over.mp3");
    SDL_Log("SFX prechargé: game_over.mp3");

}

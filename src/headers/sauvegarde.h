#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define NB_SLOTS_SAUVEGARDE 3

int sauvegarde_slot_existe(int slot);
int sauvegarde_choisir_slot(SDL_Renderer *renderer, const char *titre, bool mode_chargement);

int sauvegarder_partie_slot(int slot, int planete);
int charger_partie_slot(int slot, int *planete_out);

int sauvegarde_get_slot_actif(void);
void sauvegarde_set_slot_actif(int slot);

/* Applique une sauvegarde chargee en attente sur l'etat global du jeu. */
int sauvegarde_appliquer_si_disponible(SDL_Renderer *renderer);

#endif

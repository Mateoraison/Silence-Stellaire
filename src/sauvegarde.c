/**
 * @file sauvegarde.c
 * @brief Gestion des sauvegardes, des chargements et des emplacements.
 */

#include "headers/main.h"
#include "headers/sauvegarde.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

extern int engrenages_poses;
extern bool vaisseau_repare;

/** @brief Format binaire de donnees persistees pour un slot de sauvegarde. */
typedef struct {
	int magic;
	int version;
	int planete;
	float perso_x;
	float perso_y;
	int direction;
	int vie;
	int vie_max;
	int faim;
	int faim_max;
	int argent;
	float vitesse_bonus;
	int engrenages_poses;
	int vaisseau_repare;
	int hotbar_type[HOTBAR_SIZE];
	int hotbar_qte[HOTBAR_SIZE];
	int inventaire_type[INVENTAIRE_SIZE];
	int inventaire_qte[INVENTAIRE_SIZE];
	int caisse_type[CAISSE_OUTILS_SIZE];
	int caisse_qte[CAISSE_OUTILS_SIZE];
	int boss1_est_battu;
	int boss1_est_agro;
	int boss1_vie;
	int boss1_phase;
	int boss3_est_battu;
	int boss3_est_agro;
	int boss3_vie;
	int boss3_phase;
	int planete2_mastermind_engrenage_donne;
	int planete2_simon_termine;
	int planete1_engrenage_objectifs_donne;
	int planete2_engrenage_objectifs_donne;
	int planete3_engrenage_objectifs_donne;
	int planete2_barriere4_ouverte;
	int planete3_engrenage_recupere;
	int planete3_boss_spawned;
	int mini_mastermind_reussi;
	int mini_simon_reussi;
	int planete1_objectifs_valides[MAX_OBJECTIFS];
	int planete2_objectifs_valides[MAX_OBJECTIFS];
	int planete3_objectifs_valides[MAX_OBJECTIFS];
	uint32_t checksum;
} SaveData;

#define SAVE_MAGIC 0x5353544C /* SSTL */
#define SAVE_VERSION 3

static int slot_actif = 1;
static int chargement_en_attente = 0;
static SaveData sauvegarde_attente;

extern t_case *hotbar[HOTBAR_SIZE];
extern t_case *inventaire[INVENTAIRE_SIZE];
extern t_case *caisse_outils[CAISSE_OUTILS_SIZE];

static uint32_t calculer_checksum_save_v3(const SaveData *data) {
	SaveData tmp = *data;
	tmp.checksum = 0;

	const unsigned char *bytes = (const unsigned char *)&tmp;
	uint32_t hash = 2166136261u;
	for (size_t i = 0; i < sizeof(SaveData); i++) {
		hash ^= (uint32_t)bytes[i];
		hash *= 16777619u;
	}
	return hash;
}


static void save_set_checksum(SaveData *data) {
	data->checksum = 0;
	data->checksum = calculer_checksum_save_v3(data);
}

static int save_checksum_ok_v3(const SaveData *data) {
	return data->checksum == calculer_checksum_save_v3(data);
}

static int save_data_valide_v3(const SaveData *data) {
	if (data->magic != SAVE_MAGIC) return 0;
	if (data->version != SAVE_VERSION) return 0;
	if (data->planete < 1 || data->planete > 3) return 0;
	if (data->vie_max <= 0 || data->faim_max <= 0) return 0;
	if (data->vie < 0 || data->vie > data->vie_max) return 0;
	if (data->faim < 0 || data->faim > data->faim_max) return 0;
	if (data->direction < 0 || data->direction > 3) return 0;

	for (int i = 0; i < HOTBAR_SIZE; i++) {
		if (data->hotbar_qte[i] < 0) return 0;
	}
	for (int i = 0; i < INVENTAIRE_SIZE; i++) {
		if (data->inventaire_qte[i] < 0) return 0;
	}
	for (int i = 0; i < CAISSE_OUTILS_SIZE; i++) {
		if (data->caisse_qte[i] < 0) return 0;
	}

	if (!save_checksum_ok_v3(data)) return 0;
	return 1;
}


static void liberer_case_array(t_case **cases, int taille) {
	for (int i = 0; i < taille; i++) {
		if (cases[i]) {
			if (cases[i]->item) {
				free(cases[i]->item);
			}
			free(cases[i]);
			cases[i] = NULL;
		}
	}
}

static void reconstruire_case_array(t_case **cases, int taille, const int *types, const int *qtes, SDL_Renderer *renderer) {
	for (int i = 0; i < taille; i++) {
		if (types[i] < 0 || qtes[i] <= 0) continue;

		t_Item *base = init_item((typeItem)types[i], renderer, 0.0f, 0.0f);
		if (!base) continue;

		cases[i] = malloc(sizeof(t_case));
		if (!cases[i]) {
			free(base);
			continue;
		}

		cases[i]->item = malloc(sizeof(t_Item));
		if (!cases[i]->item) {
			free(cases[i]);
			cases[i] = NULL;
			free(base);
			continue;
		}

		cases[i]->item->type = (typeItem)types[i];
		cases[i]->item->texture = base->texture;
		cases[i]->item->x = 0;
		cases[i]->item->y = 0;
		cases[i]->quantiter = qtes[i];
		cases[i]->stackable = 1;
		cases[i]->x = 0;
		cases[i]->y = 0;
		cases[i]->w = 0;
		cases[i]->h = 0;

		free(base);
	}
}

static void assurer_dossier_saves(void) {
#ifdef _WIN32
	_mkdir("saves");
#else
	mkdir("saves", 0777);
#endif
}

static void chemin_slot(int slot, char out[64]) {
	SDL_snprintf(out, 64, "saves/slot%d.sav", slot);
}

static SDL_Texture *creer_texte(SDL_Renderer *renderer, TTF_Font *font, const char *txt, SDL_Color c) {
	SDL_Surface *s = TTF_RenderText_Solid(font, txt, SDL_strlen(txt), c);
	if (!s) return NULL;
	SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
	SDL_DestroySurface(s);
	return t;
}

int sauvegarde_slot_existe(int slot) {
	if (slot < 1 || slot > NB_SLOTS_SAUVEGARDE) return 0;
	char path[64];
	chemin_slot(slot, path);
	FILE *f = fopen(path, "rb");
	if (!f) return 0;
	fclose(f);
	return 1;
}

int sauvegarde_get_slot_actif(void) {
	return slot_actif;
}

void sauvegarde_set_slot_actif(int slot) {
	if (slot >= 1 && slot <= NB_SLOTS_SAUVEGARDE) {
		slot_actif = slot;
	}
}

int sauvegarder_partie_slot(int slot, int planete) {
	if (slot < 1 || slot > NB_SLOTS_SAUVEGARDE) return -1;

	SaveData data;
	SDL_memset(&data, 0, sizeof(data));
	data.magic = SAVE_MAGIC;
	data.version = SAVE_VERSION;
	data.planete = planete;
	data.perso_x = perso.x;
	data.perso_y = perso.y;
	data.direction = perso.direction;
	data.vie = perso.vie;
	data.vie_max = perso.vie_max;
	data.faim = perso.faim;
	data.faim_max = perso.faim_max;
	data.argent = argent;
	data.vitesse_bonus = vitesse_bonus;
	data.engrenages_poses = engrenages_poses;
	data.vaisseau_repare = vaisseau_repare ? 1 : 0;
	data.boss1_est_battu = boss1.est_battu;
	data.boss1_est_agro = boss1.est_agro;
	data.boss1_vie = boss1.vie;
	data.boss1_phase = boss1.phase;
	data.boss3_est_battu = boss3.est_battu;
	data.boss3_est_agro = boss3.est_agro;
	data.boss3_vie = boss3.vie;
	data.boss3_phase = boss3.phase;

	for (int i = 0; i < HOTBAR_SIZE; i++) {
		data.hotbar_type[i] = -1;
		data.hotbar_qte[i] = 0;
		if (hotbar[i] && hotbar[i]->item) {
			data.hotbar_type[i] = hotbar[i]->item->type;
			data.hotbar_qte[i] = hotbar[i]->quantiter;
		}
	}

	for (int i = 0; i < INVENTAIRE_SIZE; i++) {
		data.inventaire_type[i] = -1;
		data.inventaire_qte[i] = 0;
		if (inventaire[i] && inventaire[i]->item) {
			data.inventaire_type[i] = inventaire[i]->item->type;
			data.inventaire_qte[i] = inventaire[i]->quantiter;
		}
	}

	for (int i = 0; i < CAISSE_OUTILS_SIZE; i++) {
		data.caisse_type[i] = -1;
		data.caisse_qte[i] = 0;
		if (caisse_outils[i] && caisse_outils[i]->item) {
			data.caisse_type[i] = caisse_outils[i]->item->type;
			data.caisse_qte[i] = caisse_outils[i]->quantiter;
		}
	}

	progression_jeu_t progression;
	jeu_get_progression(&progression);
	data.planete2_mastermind_engrenage_donne = progression.planete2_mastermind_engrenage_donne;
	data.planete2_simon_termine = progression.planete2_simon_termine;
	data.planete1_engrenage_objectifs_donne = progression.planete1_engrenage_objectifs_donne;
	data.planete2_engrenage_objectifs_donne = progression.planete2_engrenage_objectifs_donne;
	data.planete3_engrenage_objectifs_donne = progression.planete3_engrenage_objectifs_donne;
	data.planete2_barriere4_ouverte = progression.planete2_barriere4_ouverte;
	data.planete3_engrenage_recupere = progression.planete3_engrenage_recupere;
	data.planete3_boss_spawned = progression.planete3_boss_spawned;
	data.mini_mastermind_reussi = progression.mastermind_reussi;
	data.mini_simon_reussi = progression.simon_reussi;
	for (int i = 0; i < MAX_OBJECTIFS; i++) {
		data.planete1_objectifs_valides[i] = progression.planete1_objectifs_valides[i];
		data.planete2_objectifs_valides[i] = progression.planete2_objectifs_valides[i];
		data.planete3_objectifs_valides[i] = progression.planete3_objectifs_valides[i];
	}

	save_set_checksum(&data);

	assurer_dossier_saves();
	char path[64];
	chemin_slot(slot, path);

	FILE *f = fopen(path, "wb");
	if (!f) return -1;
	size_t n = fwrite(&data, sizeof(SaveData), 1, f);
	fclose(f);
	if (n != 1) return -1;

	slot_actif = slot;
	return 0;
}

int charger_partie_slot(int slot, int *planete_out) {
	if (slot < 1 || slot > NB_SLOTS_SAUVEGARDE) return -1;

	char path[64];
	chemin_slot(slot, path);
	FILE *f = fopen(path, "rb");
	if (!f) return -1;

	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	if (file_size < 0) {
		fclose(f);
		return -1;
	}
	rewind(f);

	SaveData data;
	SDL_memset(&data, 0, sizeof(data));

	if ((size_t)file_size == sizeof(SaveData)) {
		size_t n = fread(&data, sizeof(SaveData), 1, f);
		fclose(f);
		if (n != 1) return -1;
		if (!save_data_valide_v3(&data)) return -1;
	} else {
		fclose(f);
		return -1;
	}

	sauvegarde_attente = data;
	chargement_en_attente = 1;
	slot_actif = slot;
	if (planete_out) *planete_out = data.planete;
	return 0;
}

int sauvegarde_appliquer_si_disponible(SDL_Renderer *renderer) {
	if (!chargement_en_attente) return 0;

	perso.x = sauvegarde_attente.perso_x;
	perso.y = sauvegarde_attente.perso_y;
	perso.direction = sauvegarde_attente.direction;
	perso.vie = sauvegarde_attente.vie;
	perso.vie_max = sauvegarde_attente.vie_max;
	perso.faim = sauvegarde_attente.faim;
	perso.faim_max = sauvegarde_attente.faim_max;
	argent = sauvegarde_attente.argent;
	vitesse_bonus = sauvegarde_attente.vitesse_bonus;
	engrenages_poses = sauvegarde_attente.engrenages_poses;
	vaisseau_repare = (sauvegarde_attente.vaisseau_repare != 0);
	boss1.est_battu = sauvegarde_attente.boss1_est_battu;
	boss1.est_agro = sauvegarde_attente.boss1_est_agro;
	boss1.vie = sauvegarde_attente.boss1_vie;
	boss1.phase = sauvegarde_attente.boss1_phase;
	if (boss1.est_battu) {
		boss1.est_agro = 0;
		boss1.frame_anim_mort = 15;
	}
	boss3.est_battu = sauvegarde_attente.boss3_est_battu;
	boss3.est_agro = sauvegarde_attente.boss3_est_agro;
	boss3.vie = sauvegarde_attente.boss3_vie;
	boss3.phase = sauvegarde_attente.boss3_phase;
	if (boss3.est_battu) {
		boss3.est_agro = 0;
		boss3.frame_anim_mort = 15;
	}

	progression_jeu_t progression = {
		.planete2_mastermind_engrenage_donne = sauvegarde_attente.planete2_mastermind_engrenage_donne,
		.planete2_simon_termine = sauvegarde_attente.planete2_simon_termine,
		.planete1_engrenage_objectifs_donne = sauvegarde_attente.planete1_engrenage_objectifs_donne,
		.planete2_engrenage_objectifs_donne = sauvegarde_attente.planete2_engrenage_objectifs_donne,
		.planete3_engrenage_objectifs_donne = sauvegarde_attente.planete3_engrenage_objectifs_donne,
		.planete2_barriere4_ouverte = sauvegarde_attente.planete2_barriere4_ouverte,
		.planete3_engrenage_recupere = sauvegarde_attente.planete3_engrenage_recupere,
		.planete3_boss_spawned = sauvegarde_attente.planete3_boss_spawned,
		.mastermind_reussi = sauvegarde_attente.mini_mastermind_reussi,
		.simon_reussi = sauvegarde_attente.mini_simon_reussi,
		.planete1_objectifs_valides = {0},
		.planete2_objectifs_valides = {0},
		.planete3_objectifs_valides = {0}
	};
	for (int i = 0; i < MAX_OBJECTIFS; i++) {
		progression.planete1_objectifs_valides[i] = sauvegarde_attente.planete1_objectifs_valides[i];
		progression.planete2_objectifs_valides[i] = sauvegarde_attente.planete2_objectifs_valides[i];
		progression.planete3_objectifs_valides[i] = sauvegarde_attente.planete3_objectifs_valides[i];
	}
	jeu_set_progression(&progression);

	liberer_case_array(hotbar, HOTBAR_SIZE);
	liberer_case_array(inventaire, INVENTAIRE_SIZE);
	liberer_case_array(caisse_outils, CAISSE_OUTILS_SIZE);
	reconstruire_case_array(hotbar, HOTBAR_SIZE, sauvegarde_attente.hotbar_type, sauvegarde_attente.hotbar_qte, renderer);
	reconstruire_case_array(inventaire, INVENTAIRE_SIZE, sauvegarde_attente.inventaire_type, sauvegarde_attente.inventaire_qte, renderer);
	reconstruire_case_array(caisse_outils, CAISSE_OUTILS_SIZE, sauvegarde_attente.caisse_type, sauvegarde_attente.caisse_qte, renderer);

	chargement_en_attente = 0;
	return 1;
}

int sauvegarde_choisir_slot(SDL_Renderer *renderer, const char *titre, bool mode_chargement) {
	TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
	if (!font) return 0;

	SDL_Texture *fond = IMG_LoadTexture(renderer, "assets/menu/menu.png");
	SDL_Texture *btn_tex = IMG_LoadTexture(renderer, "assets/menu/bouton.png");
	if (!btn_tex) {
		if (fond) SDL_DestroyTexture(fond);
		TTF_CloseFont(font);
		return 0;
	}

	Bouton slots[NB_SLOTS_SAUVEGARDE];
	float base_x = screen_center_x() - 170.0f;
	float base_y = screen_center_y() - ((NB_SLOTS_SAUVEGARDE + 1) * 95.0f) * 0.5f;
	for (int i = 0; i < NB_SLOTS_SAUVEGARDE; i++) {
		Bouton_Init(&slots[i], base_x, base_y + i * 95.0f, 340.0f, 72.0f, btn_tex);
	}
	Bouton btn_retour;
	Bouton_Init(&btn_retour, base_x, base_y + NB_SLOTS_SAUVEGARDE * 95.0f, 340.0f, 72.0f, btn_tex);

	SDL_Color blanc = {255, 255, 255, 255};
	SDL_Color gris = {180, 180, 180, 255};
	SDL_Color jaune = {255, 220, 80, 255};

	SDL_Texture *tex_titre = creer_texte(renderer, font, titre, blanc);

	SDL_Texture *tex_slots[NB_SLOTS_SAUVEGARDE] = {NULL};
	for (int i = 0; i < NB_SLOTS_SAUVEGARDE; i++) {
		char txt[64];
		int occupe = sauvegarde_slot_existe(i + 1);
		SDL_snprintf(txt, sizeof(txt), "Slot %d - %s", i + 1, occupe ? "Occupe" : "Vide");
		tex_slots[i] = creer_texte(renderer, font, txt, occupe ? blanc : gris);
	}
	SDL_Texture *tex_retour = creer_texte(renderer, font, "Retour", blanc);

	char notif[96] = "";
	Uint32 notif_debut = 0;

	int slot_choisi = 0;
	bool running = true;
	while (running) {
		update_screen_metrics(renderer);
		base_x = screen_center_x() - 170.0f;
		base_y = screen_center_y() - ((NB_SLOTS_SAUVEGARDE + 1) * 95.0f) * 0.5f;
		for (int i = 0; i < NB_SLOTS_SAUVEGARDE; i++) {
			slots[i].rect.x = base_x;
			slots[i].rect.y = base_y + i * 95.0f;
		}
		btn_retour.rect.x = base_x;
		btn_retour.rect.y = base_y + NB_SLOTS_SAUVEGARDE * 95.0f;

		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_MOUSE_MOTION ||
			    ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
			    ev.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				SDL_ConvertEventToRenderCoordinates(renderer, &ev);
			}

			if (ev.type == SDL_EVENT_QUIT) { running = false; break; }
			if (ev.type == SDL_EVENT_KEY_DOWN && ev.key.key == SDLK_ESCAPE) { running = false; break; }

			if (Bouton_GererEvenement(&btn_retour, &ev)) {
				running = false;
				break;
			}

			for (int i = 0; i < NB_SLOTS_SAUVEGARDE; i++) {
				if (Bouton_GererEvenement(&slots[i], &ev)) {
					int slot = i + 1;
					if (mode_chargement && !sauvegarde_slot_existe(slot)) {
						SDL_snprintf(notif, sizeof(notif), "Le slot %d est vide", slot);
						notif_debut = SDL_GetTicks();
					} else {
						slot_choisi = slot;
						running = false;
					}
					break;
				}
			}
		}

		SDL_RenderClear(renderer);
		if (fond) SDL_RenderTexture(renderer, fond, NULL, NULL);

		if (tex_titre) {
			SDL_FRect r = {screen_center_x() - 170.0f, base_y - 70.0f, 340.0f, 40.0f};
			SDL_RenderTexture(renderer, tex_titre, NULL, &r);
		}

		for (int i = 0; i < NB_SLOTS_SAUVEGARDE; i++) {
			Bouton_Afficher(&slots[i], renderer);
			if (tex_slots[i]) afficher_texte_centre(renderer, tex_slots[i], &slots[i].rect);
		}
		Bouton_Afficher(&btn_retour, renderer);
		if (tex_retour) afficher_texte_centre(renderer, tex_retour, &btn_retour.rect);

		if (notif[0] != '\0' && SDL_GetTicks() - notif_debut < 1800) {
			SDL_Texture *tex_notif = creer_texte(renderer, font, notif, jaune);
			if (tex_notif) {
				SDL_FRect r = {screen_center_x() - 140.0f, btn_retour.rect.y + btn_retour.rect.h + 14.0f, 280.0f, 30.0f};
				SDL_RenderTexture(renderer, tex_notif, NULL, &r);
				SDL_DestroyTexture(tex_notif);
			}
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

	if (tex_titre) SDL_DestroyTexture(tex_titre);
	for (int i = 0; i < NB_SLOTS_SAUVEGARDE; i++) {
		if (tex_slots[i]) SDL_DestroyTexture(tex_slots[i]);
	}
	if (tex_retour) SDL_DestroyTexture(tex_retour);
	if (fond) SDL_DestroyTexture(fond);
	SDL_DestroyTexture(btn_tex);
	TTF_CloseFont(font);

	return slot_choisi;
}

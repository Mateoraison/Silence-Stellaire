#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"

/*
 * Definitions required by extern declarations in src/headers/main.h.
 * Only the values used by src/perso.c tests are initialized here.
 */
Perso perso = {0};
int animation_frame = 0;
Uint32 animation_timer = 0;
Uint32 bouge_timer = 0;
bool perso_bouge = false;
bool combat_en_cours = false;
int combat_frame = 0;
Uint32 combat_timer = 0;
MIX_Track *track_global = NULL;
int Planete_actuelle = 0;
int g_screen_w = 1920;
int g_screen_h = 1080;
int index_item = 0;
int argent = 0;
float vitesse_bonus = 0.0f;
t_case *hotbar[HOTBAR_SIZE] = {0};
t_case *caisse_outils[CAISSE_OUTILS_SIZE] = {0};
t_Item *items[MAX_ITEMS] = {0};
Mob *mobs[MAX_MOB] = {0};
boss_t boss1 = {0};
boss_t boss3 = {0};
t_case *inventaire[INVENTAIRE_SIZE] = {0};
bool inventaire_ouvert = false;
bool g_planete3_engrenage_recupere = false;
bool g_planete3_boss_spawned = false;

/* Stubs for project-level dependencies referenced by src/perso.c. */
void schedule_respawn(int type_mob, Uint32 delai_ms) {
	(void)type_mob;
	(void)delai_ms;
}

void detruire_un_mob(Mob *mob) {
	(void)mob;
}

t_Item *init_item(typeItem type, SDL_Renderer *renderer, float x, float y) {
	(void)type;
	(void)renderer;
	(void)x;
	(void)y;
	return NULL;
}

void ajouter_item_hotbar(t_case *hotbar_slots[HOTBAR_SIZE], t_Item *item, SDL_Renderer *renderer) {
	(void)hotbar_slots;
	(void)item;
	(void)renderer;
}

static int g_total_checks = 0;
static int g_failed_checks = 0;

static void assert_int_eq(const char *name, int expected, int actual) {
	g_total_checks++;
	if (expected != actual) {
		g_failed_checks++;
		printf("[ECHEC] %s: attendu=%d, obtenu=%d\n", name, expected, actual);
	} else {
		printf("[OK] %s\n", name);
	}
}

static void assert_true(const char *name, int condition) {
	g_total_checks++;
	if (!condition) {
		g_failed_checks++;
		printf("[ECHEC] %s\n", name);
	} else {
		printf("[OK] %s\n", name);
	}
}

static void init_map(t_tile map[W_MAP][H_MAP], type_t type) {
	for (int x = 0; x < W_MAP; x++) {
		for (int y = 0; y < H_MAP; y++) {
			map[x][y].type = type;
			map[x][y].width = DISPLAY_TILE_SIZE;
			map[x][y].height = DISPLAY_TILE_SIZE;
		}
	}
}

int main(void) {
	t_tile map[W_MAP][H_MAP];
	init_map(map, terreP);

	/* 1) vide: collision pleine tile */
	map[10][10].type = vide;
	SDL_Rect hit_vide = {10 * DISPLAY_TILE_SIZE + 1, 10 * DISPLAY_TILE_SIZE + 1, 10, 10};
	assert_int_eq("vide avec intersection", 1, test_collision(10, 10, map, 0, hit_vide));

	SDL_Rect miss_vide = {0, 0, 10, 10};
	assert_int_eq("vide sans intersection", 0, test_collision(10, 10, map, 0, miss_vide));

	/* 2) eau: meme comportement de collision que vide */
	map[11][10].type = eau;
	SDL_Rect hit_eau = {11 * DISPLAY_TILE_SIZE + 30, 10 * DISPLAY_TILE_SIZE + 30, 5, 5};
	assert_int_eq("eau avec intersection", 1, test_collision(11, 10, map, 0, hit_eau));

	/* 3) pierre: hitbox restreinte */
	map[12][10].type = pierre;
	SDL_Rect hit_pierre = {12 * DISPLAY_TILE_SIZE + 8, 10 * DISPLAY_TILE_SIZE + 4, 20, 20};
	SDL_Rect miss_pierre = {12 * DISPLAY_TILE_SIZE, 10 * DISPLAY_TILE_SIZE, 5, 5};
	assert_int_eq("pierre dans hitbox", 1, test_collision(12, 10, map, 0, hit_pierre));
	assert_int_eq("pierre hors hitbox", 0, test_collision(12, 10, map, 0, miss_pierre));

	/* 4) arbrecoupe: hitbox partielle */
	map[13][10].type = arbrecoupe;
	SDL_Rect hit_arbrecoupe = {13 * DISPLAY_TILE_SIZE + 13, 10 * DISPLAY_TILE_SIZE + 8, 20, 20};
	assert_int_eq("arbrecoupe dans hitbox", 1, test_collision(13, 10, map, 0, hit_arbrecoupe));

	/* 5) arbreEntier: collision multi-rectangles */
	map[15][15].type = arbreEntier;
	SDL_Rect hit_arbre_entier = {
		15 * DISPLAY_TILE_SIZE + (int)(59 * 1.4f),
		15 * DISPLAY_TILE_SIZE + (int)(38 * 1.4f),
		6,
		6
	};
	assert_int_eq("arbreEntier dans hitbox composee", 1, test_collision(15, 15, map, 0, hit_arbre_entier));

	/* 6) feu: degats au joueur si non-mob et pas invincible */
	map[20][20].type = feu;
	perso.vie = 3;
	perso.invincibiliter_timer = 0;
	SDL_Delay(2);
	assert_int_eq("feu retourne 0", 0, test_collision(20, 20, map, 0, (SDL_Rect){0, 0, 1, 1}));
	assert_int_eq("feu retire 1 pv", 2, perso.vie);

	/* 7) feu: pas de degats sur mob */
	perso.vie = 5;
	perso.invincibiliter_timer = 0;
	assert_int_eq("feu sur mob ne blesse pas", 0, test_collision(20, 20, map, 1, (SDL_Rect){0, 0, 1, 1}));
	assert_int_eq("vie intacte pour mob", 5, perso.vie);

	/* 8) feu: pas de degats si deja invincible */
	perso.vie = 4;
	perso.invincibiliter_timer = 1234;
	assert_int_eq("feu invincible retourne 0", 0, test_collision(20, 20, map, 0, (SDL_Rect){0, 0, 1, 1}));
	assert_int_eq("feu invincible ne retire pas pv", 4, perso.vie);

	printf("\nResultat: %d checks, %d echecs\n", g_total_checks, g_failed_checks);
	return (g_failed_checks == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/**
 * @file main.h
 * @brief Declarations globales et utilitaires partages entre les modules du jeu.
 */

#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

#define FRAME_DUREE 50

#define W_MAP 40
#define H_MAP 40
#define SOURCE_TILE_SIZE 64
#define DISPLAY_TILE_SIZE 90

#define PLANETE3_TILE_SIZE 16
#define PLANETE3_ROOM_W 10
#define PLANETE3_ROOM_H 7
#define PLANETE3_END_W 5
#define PLANETE3_END_H 5

#define VAISSEAU_WORLD_X 750.0f
#define VAISSEAU_WORLD_Y 550.0f
#define VAISSEAU_WIDTH 644.0f
#define VAISSEAU_HEIGHT 388.0f

#define VAISSEAU_SPRITE_SRC_W 192.0f
#define VAISSEAU_SPRITE_SRC_H 128.0f
#define VAISSEAU_SOLID_SRC_X 28.0f
#define VAISSEAU_SOLID_SRC_Y 22.0f
#define VAISSEAU_SOLID_SRC_W 148.0f
#define VAISSEAU_SOLID_SRC_H 94.0f

#define VAISSEAU_COLLISION_X (VAISSEAU_WORLD_X + (VAISSEAU_SOLID_SRC_X * (VAISSEAU_WIDTH / VAISSEAU_SPRITE_SRC_W)))
#define VAISSEAU_COLLISION_Y (VAISSEAU_WORLD_Y + (VAISSEAU_SOLID_SRC_Y * (VAISSEAU_HEIGHT / VAISSEAU_SPRITE_SRC_H)))
#define VAISSEAU_COLLISION_W (VAISSEAU_SOLID_SRC_W * (VAISSEAU_WIDTH / VAISSEAU_SPRITE_SRC_W))
#define VAISSEAU_COLLISION_H (VAISSEAU_SOLID_SRC_H * (VAISSEAU_HEIGHT / VAISSEAU_SPRITE_SRC_H))

#define MAX_MOB 1000
#define MAX_ITEMS 200

#define HOTBAR_SIZE 5
#define INVENTAIRE_COLS 5
#define INVENTAIRE_ROWS 4
#define INVENTAIRE_SIZE (INVENTAIRE_COLS * INVENTAIRE_ROWS)
#define CAISSE_OUTILS_SIZE 6
#define MAX_OBJECTIFS 8

/**
 * @enum type_t
 * @brief Types de tuiles utilises par les cartes de planetes et le systeme de collision.
 */
typedef enum {vide, terreP, eau, terreCHG, terreCHD, terreCBG, terreCBD, terreH, terreB, terreG, terreD, feu, pierre, arbrecoupe, arbreEntier, cterreHBG, cterreBBG, cterreHHD, cterrBHD} type_t;

/**
 * @struct t_tile
 * @brief Representation d'une tuile logique de carte (type + dimensions de rendu).
 */
typedef struct tile_{
    type_t type;
    int width;
    int height;
}t_tile;

/**
 * @struct t_tileset
 * @brief Grille complete de tuiles pour une carte de jeu.
 */
typedef struct tileset_{
    int width;
    int height;
    t_tile tab[W_MAP][H_MAP];
}t_tileset;

/**
 * @struct Perso
 * @brief Etat runtime du joueur (position, direction, stats vitales et invincibilite temporaire).
 */
typedef struct {
    float x;
    float y;
    SDL_Texture *texture;
    int direction; // 0=south,1=north,2=west,3=east
    int vie;
    int vie_max;
    int faim;
    int faim_max;
    Uint32 invincibiliter_timer;
} Perso;

/**
 * @enum typeItem
 * @brief Types d'objets manipulables dans l'inventaire, la hotbar et les interactions.
 */
typedef enum {PIECE,VIANDE,MARTEAU,SOIN,BRIQUET,BOIS,FEUDECAMP,VIANDECUITE,ENGRENAGE, CLE} typeItem;

/**
 * @struct t_Item
 * @brief Instance d'objet present au sol ou dans un conteneur.
 */
typedef struct item_ {
    typeItem type;
    SDL_Texture * texture;
    float x;
    float y;
}t_Item;

/**
 * @struct Mob
 * @brief Entite ennemie/neutre active avec son etat de deplacement, vie et drop.
 */
typedef struct {
    float x;
    float y;
    int direction;
    float vx;
    float vy;
    float target_vx;
    float target_vy;
    float speed;
    float largeur,hauteur;
    Uint32 time_change_dir;
    SDL_Texture * texture;
    int vie;
    int id; //1 = mouton, 2 = pawns
    int type_boss_proprietaire; // -1 pour les mobs neutres, sinon type de boss proprietaire
    int drop_chance;
}Mob;

/**
 * @enum type_boss_t
 * @brief Identifiants des variantes de boss disponibles dans le jeu.
 */
typedef enum {
    TYPE_BOSS_DEMON_DE_FEU = 0,
    TYPE_BOSS_SENTINELLE = 1,
    TYPE_BOSS_MINOTAURE = 2
} type_boss_t;

/**
 * @struct boss_config_t
 * @brief Parametres statiques d'un boss (ressources, stats et cooldowns de capacites).
 */
typedef struct {
    type_boss_t type;
    const char *nom_affiche;
    const char *chemin_sprite;
    const char *chemin_projectile;
    const char *chemin_minion_course;
    int vie_max;
    int attaque_contact;
    Uint32 cooldown_projectile_ms;
    Uint32 cooldown_zone_ms;
    Uint32 cooldown_invocation_ms;
    int active_projectile;
    int active_zone;
    int active_invocation;
} boss_config_t;

/**
 * @struct boss_t
 * @brief Etat dynamique d'un boss en combat (position, phases, animations, cooldowns et IA).
 */
typedef struct {
    float x;
    float y;
    type_boss_t type;
    const boss_config_t *config_boss;
    int est_battu;
    int vie;
    int vie_max;
    int attaque;
    Uint32 cooldown_attaque;
    Uint32 cooldown_zone;
    Uint32 cooldown_invocation;
    Uint32 fin_animation_attaque;
    int frame_anim_repos;
    int frame_anim_attaque;
    int frame_anim_mort;
    Uint32 animation_timer;
    int phase;
    SDL_Texture * texture;
    int etat_anim; // 0: repos, 1: attaque, 2: mort
    int drop_effectue;
    int est_agro;
    float rayon_detection;
    float spawn_x;
    float spawn_y;
    float patrouille_cible_x;
    float patrouille_cible_y;
    int a_cible_patrouille;
    int degats_melee_appliques;
    int minotaure_regarde_droite;
    Uint32 stun_jusqua;
    Uint32 stun_recharge_jusqua;
}boss_t;

/**
 * @struct t_case
 * @brief Slot de stockage UI (hotbar/inventaire/caisse) contenant un item et sa quantite.
 */
typedef struct {
    int x;
    int y;
    int w;
    int h;
    t_Item * item;
    int stackable;
    int quantiter;
}t_case;


#include "menu.h"
#include "game_over.h"
#include "jeux.h"
#include "bouton.h"
#include "option.h"
#include "perso.h"
#include "son.h"
#include "boss.h"
#include "mob.h"
#include "vaisseau.h"
#include "map.h"
#include "item.h"
#include "inventaire.h"
#include "sauvegarde.h"
#include "objectifs.h"
#include "cinematique.h"
#include "shop.h"


extern Perso perso;
extern int animation_frame;
extern Uint32 animation_timer;
extern Uint32 bouge_timer;
extern bool perso_bouge;
extern bool combat_en_cours;
extern int combat_frame;
extern Uint32 combat_timer;

extern MIX_Track *track_global;

extern int Planete_actuelle;

extern int g_screen_w;
extern int g_screen_h;

/**
 * @fn void update_screen_metrics(SDL_Renderer *renderer)
 * @brief Met a jour screen metrics en fonction de l'etat courant du jeu.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void update_screen_metrics(SDL_Renderer *renderer);

/* Retire les engrenages du joueur lors d'un changement de planete */
/**
 * @fn void retirer_engrenages_joueur(void)
 * @brief Retire les engrenages requis du stock joueur pour valider une action.
 */
void retirer_engrenages_joueur(void);

static inline float screen_widthf(void) {
    return (float)g_screen_w;
}

static inline float screen_heightf(void) {
    return (float)g_screen_h;
}

static inline float screen_center_x(void) {
    return screen_widthf() * 0.5f;
}

static inline float screen_center_y(void) {
    return screen_heightf() * 0.5f;
}

extern int index_item;
extern int argent;
extern float vitesse_bonus;
extern bool sortie_vaisseau;

extern t_case *hotbar[HOTBAR_SIZE];
extern t_case *caisse_outils[CAISSE_OUTILS_SIZE];
extern t_Item * items[MAX_ITEMS];
extern Mob * mobs[MAX_MOB];
extern boss_t boss1;
extern boss_t boss3;
extern bool g_mastermind_reussi;
extern bool g_simon_reussi;

extern t_case *inventaire[INVENTAIRE_SIZE];
extern bool inventaire_ouvert;

extern bool g_planete3_engrenage_recupere;
extern bool g_planete3_boss_spawned;
extern bool g_planete3_spawn_engrenage_defini;
extern float g_planete3_spawn_engrenage_x;
extern float g_planete3_spawn_engrenage_y;

#endif

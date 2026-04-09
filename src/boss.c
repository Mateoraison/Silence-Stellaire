#include "headers/main.h"
#include <SDL3_ttf/SDL_ttf.h>

#define BOSS_PROJECTILE_MAX 32
#define BOSS_PROJECTILE_SPEED 300.0f
#define BOSS_PROJECTILE_LIFETIME_MS 3000
#define BOSS_PROJECTILE_COOLDOWN_MS 1200
#define BOSS_PROJECTILE_FRAME_COUNT 13
#define BOSS_PROJECTILE_FRAME_MS 70
#define BOSS_ZONE_COOLDOWN_MS 3200
#define BOSS_SUMMON_COOLDOWN_MS 5200
#define BOSS_ZONE_RADIUS 260.0f
#define BOSS_ZONE_WARNING_MS 900
#define BOSS_REFLECTED_PROJECTILE_SPEED 420.0f
#define BOSS_REFLECTED_PROJECTILE_DAMAGE 8
#define BOSS_HEALTHBAR_Y 62.0f
#define BOSS_MINION_MAX 6
#define BOSS_AGGRO_RADIUS 800.0f

#define BOSS_MINOTAURE_AGGRO_RADIUS 620.0f
#define BOSS_MINOTAURE_MOVE_SPEED 155.0f
#define BOSS_MINOTAURE_MELEE_RANGE 118.0f
#define BOSS_MINOTAURE_PATROL_RADIUS 260.0f
#define BOSS_MINOTAURE_STUN_MS 700
#define BOSS_MINOTAURE_STUN_RECHARGE_MS 2200

#define BOSS_MINOTAURE_IDLE_FRAMES 5
#define BOSS_MINOTAURE_WALK_FRAMES 8
#define BOSS_MINOTAURE_ATTACK_FRAMES 9
#define BOSS_MINOTAURE_DEATH_FRAMES 9
#define BOSS_MINOTAURE_IDLE_Y 960.0f
#define BOSS_MINOTAURE_WALK_Y 1072.0f
#define BOSS_MINOTAURE_ATTACK_Y 1248.0f
#define BOSS_MINOTAURE_DEATH_Y 1840.0f
#define BOSS_MINOTAURE_FRAME_W 96.0f
#define BOSS_MINOTAURE_FRAME_H 96.0f

typedef void (*fonction_attaque_boss_t)(SDL_Renderer *renderer, boss_t *boss_ref);

typedef struct {
    int actif;
    int renvoye;
    float x;
    float y;
    float vx;
    float vy;
    Uint32 spawn_tick;
} boss_projectile_t;

static boss_projectile_t g_projectiles[BOSS_PROJECTILE_MAX];
static Uint32 g_zone_warning_start = 0;
static int g_zone_warning_actif = 0;
static int g_zone_impact_pending = 0;
static Uint32 g_zone_impact_tick = 0;
static float g_zone_center_x = 0.0f;
static float g_zone_center_y = 0.0f;
static int g_last_combat_en_cours = 0;
static SDL_Texture *g_texture_pawn_invoke = NULL;
static TTF_Font *g_boss_name_font = NULL;
static SDL_Texture *g_texture_projectile = NULL;
static type_boss_t g_type_boss_projectile_charge = (type_boss_t)-1;
static type_boss_t g_type_boss_minion_charge = (type_boss_t)-1;
static t_tile (*g_map_navigation)[H_MAP] = NULL;
static float g_boss_dt = 0.016f;

static const boss_config_t g_configs_boss[] = {
    {
        TYPE_BOSS_DEMON_DE_FEU,
        "FIREDEMON",
        "assets/personnage/boss1/boss.png",
        "assets/personnage/boss1/attaque_ligne.png",
        "assets/personnage/boss1/minions/Warrior_Run.png",
        100,
        10,
        1200,
        3200,
        5200,
        1,
        1,
        1
    },
    {
        TYPE_BOSS_SENTINELLE,
        "SENTINELLE",
        "assets/personnage/boss1/boss.png",
        "assets/personnage/boss1/attaque_ligne.png",
        "assets/personnage/boss1/minions/Warrior_Run.png",
        140,
        12,
        900,
        2400,
        0,
        1,
        1,
        0
    },
    {
        TYPE_BOSS_MINOTAURE,
        "MINOTAURE",
        "assets/personnage/boss3/boss2.png",
        "",
        "",
        170,
        2,
        900,
        0,
        0,
        0,
        0,
        0
    }
};

void boss_set_navigation_map(t_tile map[W_MAP][H_MAP]) {
    g_map_navigation = map;
}

static const boss_config_t *obtenir_config_boss(type_boss_t type) {
    for (int i = 0; i < (int)(sizeof(g_configs_boss) / sizeof(g_configs_boss[0])); i++) {
        if (g_configs_boss[i].type == type) {
            return &g_configs_boss[i];
        }
    }
    return &g_configs_boss[0];
}

static void get_player_world_center(float *x, float *y) {
    // Match gameplay hitbox center: player hitbox is 40x60 at (screen_center_x, screen_center_y).
    if (x) *x = -perso.x + screen_center_x() + 20.0f;
    if (y) *y = -perso.y + screen_center_y() + 30.0f;
}

static int compter_minions_boss(const boss_t *boss_ref) {
    int count = 0;
    for (int i = 0; i < MAX_MOB && mobs[i] != NULL; i++) {
        if (mobs[i]->id == 3 && mobs[i]->type_boss_proprietaire == (int)boss_ref->type) {
            count++;
        }
    }
    return count;
}

static void supprimer_minions_boss(const boss_t *boss_ref) {
    int i = 0;
    while (i < MAX_MOB && mobs[i] != NULL) {
        if (mobs[i]->id == 3 && mobs[i]->type_boss_proprietaire == (int)boss_ref->type) {
            detruire_un_mob(mobs[i]);
            int j = i;
            while (j + 1 < MAX_MOB && mobs[j + 1] != NULL) {
                mobs[j] = mobs[j + 1];
                j++;
            }
            mobs[j] = NULL;
            continue;
        }
        i++;
    }
}

static void get_boss_centre_monde(const boss_t *boss_ref, float *cx, float *cy) {
    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
    /* Centre base sur la hitbox minotaure reduite (voir get_boss_hitbox_ecran) */
    *cx = boss_ref->x + 155.0f;
    *cy = boss_ref->y + 192.5f;
        return;
    }

    *cx = boss_ref->x + 290.0f;
    *cy = boss_ref->y + 165.0f;
}

static int joueur_dans_zone_agro(const boss_t *boss_ref) {
    float boss_cx = 0.0f;
    float boss_cy = 0.0f;
    float joueur_x = 0.0f;
    float joueur_y = 0.0f;

    get_boss_centre_monde(boss_ref, &boss_cx, &boss_cy);
    get_player_world_center(&joueur_x, &joueur_y);

    float dx = joueur_x - boss_cx;
    float dy = joueur_y - boss_cy;
    float dist2 = dx * dx + dy * dy;
    float radius = boss_ref->rayon_detection;
    return dist2 <= (radius * radius);
}

static void clear_projectiles(void) {
    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++) {
        g_projectiles[i].actif = 0;
        g_projectiles[i].renvoye = 0;
    }
}

static void charger_texture_projectile(SDL_Renderer *renderer, const boss_t *boss_ref) {
    if (renderer == NULL || boss_ref == NULL || boss_ref->config_boss == NULL) {
        return;
    }

    if (g_texture_projectile != NULL && g_type_boss_projectile_charge != boss_ref->type) {
        SDL_DestroyTexture(g_texture_projectile);
        g_texture_projectile = NULL;
    }

    if (g_texture_projectile == NULL) {
        g_texture_projectile = IMG_LoadTexture(renderer, boss_ref->config_boss->chemin_projectile);
        if (g_texture_projectile == NULL) {
            SDL_Log("Erreur chargement projectile boss : %s", SDL_GetError());
        } else {
            g_type_boss_projectile_charge = boss_ref->type;
        }
    }
}

static void get_zone_attaque_joueur(SDL_FRect *zone) {
    const float char_screen_x = screen_center_x();
    const float char_screen_y = screen_center_y();
    const float char_w = DISPLAY_TILE_SIZE;
    const float char_h = DISPLAY_TILE_SIZE;
    float zone_attaque_w = DISPLAY_TILE_SIZE * 0.6f;
    float zone_attaque_h = DISPLAY_TILE_SIZE * 0.5f;
    float zone_attaque_x = char_screen_x + (char_w - zone_attaque_w) / 2.0f;
    float zone_attaque_y = char_screen_y + (char_h - zone_attaque_h) / 2.0f;

    switch (perso.direction) {
        case 0:
            zone_attaque_y = char_screen_y + char_h;
            break;
        case 1:
            zone_attaque_y = char_screen_y - zone_attaque_h;
            break;
        case 2:
            zone_attaque_x = char_screen_x - zone_attaque_w;
            break;
        case 3:
            zone_attaque_x = char_screen_x + char_w;
            break;
        default:
            break;
    }

    zone->x = zone_attaque_x;
    zone->y = zone_attaque_y;
    zone->w = zone_attaque_w;
    zone->h = zone_attaque_h;
}

static SDL_FRect get_boss_hitbox_ecran(const boss_t *boss_ref) {
    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        SDL_FRect rect_boss = {
            /* Hitbox reduite pour naviguer dans le labyrinthe */
            boss_ref->x + perso.x + 120.0f,
            boss_ref->y + perso.y + 145.0f,
            70.0f,
            95.0f
        };
        return rect_boss;
    }

    SDL_FRect rect_boss = {
        boss_ref->x + perso.x + 95.0f,
        boss_ref->y + perso.y + 55.0f,
        390.0f,
        220.0f
    };
    return rect_boss;
}

static SDL_Texture *get_texture_pawn_existante(SDL_Renderer *renderer, const boss_t *boss_ref) {
    for (int i = 0; i < MAX_MOB && mobs[i] != NULL; i++) {
        if (mobs[i]->id == 3 && mobs[i]->type_boss_proprietaire == (int)boss_ref->type && mobs[i]->texture != NULL) {
            return mobs[i]->texture;
        }
    }

    if (g_texture_pawn_invoke != NULL && g_type_boss_minion_charge != boss_ref->type) {
        SDL_DestroyTexture(g_texture_pawn_invoke);
        g_texture_pawn_invoke = NULL;
    }

    if (g_texture_pawn_invoke == NULL && renderer != NULL && boss_ref->config_boss != NULL) {
        g_texture_pawn_invoke = IMG_LoadTexture(renderer, boss_ref->config_boss->chemin_minion_course);
        if (g_texture_pawn_invoke != NULL) {
            g_type_boss_minion_charge = boss_ref->type;
        }
    }
    return g_texture_pawn_invoke;
}

static void invoquer_minions_autour_boss(SDL_Renderer *renderer, const boss_t *boss_ref, int nombre) {
    SDL_Texture *tex_pawn = get_texture_pawn_existante(renderer, boss_ref);
    if (tex_pawn == NULL) {
        return;
    }

    int deja_presents = compter_minions_boss(boss_ref);
    if (deja_presents >= BOSS_MINION_MAX) {
        return;
    }

    int a_spawn = nombre;
    if (deja_presents + a_spawn > BOSS_MINION_MAX) {
        a_spawn = BOSS_MINION_MAX - deja_presents;
    }

    float centre_x = 0.0f;
    float centre_y = 0.0f;
    get_boss_centre_monde(boss_ref, &centre_x, &centre_y);

    float joueur_x = 0.0f;
    float joueur_y = 0.0f;
    get_player_world_center(&joueur_x, &joueur_y);
    float dir_x = joueur_x - centre_x;
    float dir_y = joueur_y - centre_y;
    float dir_len = sqrtf(dir_x * dir_x + dir_y * dir_y);
    if (dir_len < 0.001f) {
        dir_len = 1.0f;
    }
    float base_angle = atan2f(dir_y / dir_len, dir_x / dir_len);

    for (int n = 0; n < a_spawn; n++) {
        int idx = 0;
        while (idx < MAX_MOB && mobs[idx] != NULL) {
            idx++;
        }
        if (idx >= MAX_MOB) {
            break;
        }

        Mob *m = malloc(sizeof(Mob));
        if (m == NULL) {
            break;
        }

        float cone_offset = ((float)(rand() % 111) - 55.0f) * (3.14159265f / 180.0f);
        float spawn_angle = base_angle + cone_offset;
        float rayon = 130.0f + (float)(rand() % 90);

        m->largeur = 1.0f;
        m->hauteur = 1.0f;
        m->x = centre_x + cosf(spawn_angle) * rayon - (DISPLAY_TILE_SIZE / 2.0f);
        m->y = centre_y + sinf(spawn_angle) * rayon - (DISPLAY_TILE_SIZE / 2.0f);
        m->direction = 0;
        m->vx = 0.0f;
        m->vy = 0.0f;
        m->target_vx = 0.0f;
        m->target_vy = 0.0f;
        m->speed = 110.0f;
        m->time_change_dir = 0;
        m->texture = tex_pawn;
        m->vie = 3;
        m->id = 3;
        m->type_boss_proprietaire = (int)boss_ref->type;
        m->drop_chance = 0;

        mobs[idx] = m;
    }
}

static void appliquer_degats_joueur(int degats) {
    if (degats <= 0) {
        return;
    }

    if (perso.vie > 0 && perso.invincibiliter_timer == 0) {
        jouer_son("assets/audio/dammage.wav", 0.5f);
        perso.vie -= degats;
        if (perso.vie < 0) {
            perso.vie = 0;
        }
        perso.invincibiliter_timer = SDL_GetTicks();
    }
}

static void spawn_projectile_vers_joueur(boss_t *boss_ref) {
    float boss_cx = 0.0f;
    float boss_cy = 0.0f;
    get_boss_centre_monde(boss_ref, &boss_cx, &boss_cy);
    float joueur_cx = 0.0f;
    float joueur_cy = 0.0f;
    get_player_world_center(&joueur_cx, &joueur_cy);

    float dx = joueur_cx - boss_cx;
    float dy = joueur_cy - boss_cy;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < 0.001f) {
        dist = 1.0f;
    }

    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++) {
        if (!g_projectiles[i].actif) {
            g_projectiles[i].actif = 1;
            g_projectiles[i].renvoye = 0;
            g_projectiles[i].x = boss_cx;
            g_projectiles[i].y = boss_cy;
            g_projectiles[i].vx = (dx / dist) * BOSS_PROJECTILE_SPEED;
            g_projectiles[i].vy = (dy / dist) * BOSS_PROJECTILE_SPEED;
            g_projectiles[i].spawn_tick = SDL_GetTicks();
            break;
        }
    }
}

static void update_projectiles(float dt, boss_t *boss_ref) {
    Uint32 now = SDL_GetTicks();
    SDL_FRect hitbox_perso = {screen_center_x(), screen_center_y(), 40.0f, 60.0f};
    SDL_FRect boss_hitbox = get_boss_hitbox_ecran(boss_ref);

    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++) {
        if (!g_projectiles[i].actif) {
            continue;
        }

        g_projectiles[i].x += g_projectiles[i].vx * dt;
        g_projectiles[i].y += g_projectiles[i].vy * dt;

        if (now - g_projectiles[i].spawn_tick > BOSS_PROJECTILE_LIFETIME_MS) {
            g_projectiles[i].actif = 0;
            continue;
        }

        SDL_FRect rect_proj = {
            g_projectiles[i].x + perso.x - 16.0f,
            g_projectiles[i].y + perso.y - 16.0f,
            32.0f,
            32.0f
        };

        if (!g_projectiles[i].renvoye && SDL_HasRectIntersectionFloat(&rect_proj, &hitbox_perso)) {
            appliquer_degats_joueur(1);
            g_projectiles[i].actif = 0;
            continue;
        }

        if (g_projectiles[i].renvoye && SDL_HasRectIntersectionFloat(&rect_proj, &boss_hitbox)) {
            if (!boss_ref->est_battu) {
                boss_ref->vie -= BOSS_REFLECTED_PROJECTILE_DAMAGE;
                if (boss_ref->vie < 0) {
                    boss_ref->vie = 0;
                }
            }
            g_projectiles[i].actif = 0;
        }
    }
}

static void renvoyer_projectiles_touches(boss_t *boss_ref) {
    if (boss_ref->est_battu) {
        return;
    }

    SDL_FRect zone_attaque;
    get_zone_attaque_joueur(&zone_attaque);
    float boss_cx = 0.0f;
    float boss_cy = 0.0f;
    get_boss_centre_monde(boss_ref, &boss_cx, &boss_cy);

    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++) {
        if (!g_projectiles[i].actif || g_projectiles[i].renvoye) {
            continue;
        }

        SDL_FRect rect_proj = {
            g_projectiles[i].x + perso.x - 16.0f,
            g_projectiles[i].y + perso.y - 16.0f,
            32.0f,
            32.0f
        };

        if (SDL_HasRectIntersectionFloat(&rect_proj, &zone_attaque)) {
            float dx = boss_cx - g_projectiles[i].x;
            float dy = boss_cy - g_projectiles[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < 0.001f) {
                dist = 1.0f;
            }

            g_projectiles[i].renvoye = 1;
            g_projectiles[i].vx = (dx / dist) * BOSS_REFLECTED_PROJECTILE_SPEED;
            g_projectiles[i].vy = (dy / dist) * BOSS_REFLECTED_PROJECTILE_SPEED;
            g_projectiles[i].spawn_tick = SDL_GetTicks();
        }
    }
}

static void afficher_projectiles(SDL_Renderer *renderer, const boss_t *boss_ref) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    charger_texture_projectile(renderer, boss_ref);

    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++) {
        if (!g_projectiles[i].actif) {
            continue;
        }

        SDL_FRect rect_proj = {
            g_projectiles[i].x + perso.x - 16.0f,
            g_projectiles[i].y + perso.y - 16.0f,
            32.0f,
            32.0f
        };

        if (g_texture_projectile != NULL) {
            float tw = 0.0f;
            float th = 0.0f;
            SDL_GetTextureSize(g_texture_projectile, &tw, &th);
            if (tw > 0.0f && th > 0.0f) {
                Uint32 age = SDL_GetTicks() - g_projectiles[i].spawn_tick;
                int frame = (int)((age / BOSS_PROJECTILE_FRAME_MS) % BOSS_PROJECTILE_FRAME_COUNT);
                float frame_w = tw / (float)BOSS_PROJECTILE_FRAME_COUNT;
                double angle_deg = atan2((double)g_projectiles[i].vy, (double)g_projectiles[i].vx) * (180.0 / 3.141592653589793);

                SDL_FRect src_proj = {
                    frame_w * (float)frame,
                    0.0f,
                    frame_w,
                    th
                };
                SDL_FPoint center = {rect_proj.w * 0.5f, rect_proj.h * 0.5f};
                SDL_RenderTextureRotated(renderer, g_texture_projectile, &src_proj, &rect_proj, angle_deg, &center, SDL_FLIP_NONE);
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 120, 50, 255);
            SDL_RenderFillRect(renderer, &rect_proj);
        }

        if (g_projectiles[i].renvoye) {
            SDL_SetRenderDrawColor(renderer, 110, 220, 255, 255);
            SDL_RenderRect(renderer, &rect_proj);
        }
    }
}

static void afficher_zone_warning(SDL_Renderer *renderer, boss_t *boss_ref) {
    (void)boss_ref;
    if (!g_zone_warning_actif) {
        return;
    }

    Uint32 now = SDL_GetTicks();
    if (!g_zone_impact_pending || now >= g_zone_impact_tick) {
        g_zone_warning_actif = 0;
        return;
    }

    float progress = (float)(now - g_zone_warning_start) / (float)BOSS_ZONE_WARNING_MS;
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, (Uint8)(70 + progress * 110.0f));
    SDL_FRect zone = {
        g_zone_center_x + perso.x - BOSS_ZONE_RADIUS,
        g_zone_center_y + perso.y - BOSS_ZONE_RADIUS,
        BOSS_ZONE_RADIUS * 2.0f,
        BOSS_ZONE_RADIUS * 2.0f
    };
    SDL_RenderFillRect(renderer, &zone);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, (Uint8)(80 + progress * 150.0f));
    SDL_RenderRect(renderer, &zone);
}

static void afficher_barre_vie_boss(SDL_Renderer *renderer, const boss_t *boss_ref) {
    if (boss_ref->est_battu || boss_ref->vie_max <= 0 || !boss_ref->est_agro) {
        return;
    }

    float ratio = (float)boss_ref->vie / (float)boss_ref->vie_max;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    SDL_FRect fond = {screen_center_x() - 250.0f, BOSS_HEALTHBAR_Y, 500.0f, 24.0f};
    SDL_FRect vie = {fond.x + 2.0f, BOSS_HEALTHBAR_Y + 2.0f, (fond.w - 4.0f) * ratio, 20.0f};
    SDL_Color color_nom = {235, 35, 40, 255};
    SDL_Color color_ombre = {35, 0, 0, 220};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        SDL_SetRenderDrawColor(renderer, 60, 42, 28, 220);
    } else {
        SDL_SetRenderDrawColor(renderer, 35, 35, 35, 210);
    }
    SDL_RenderFillRect(renderer, &fond);

    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        SDL_SetRenderDrawColor(renderer, 141, 90, 52, 240);
        color_nom = (SDL_Color){173, 117, 72, 255};
        color_ombre = (SDL_Color){45, 24, 10, 220};
    } else {
        SDL_SetRenderDrawColor(renderer, 210, 50, 45, 230);
    }
    SDL_RenderFillRect(renderer, &vie);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 210);
    SDL_RenderRect(renderer, &fond);

    if (g_boss_name_font == NULL) {
        g_boss_name_font = TTF_OpenFont("assets/police.ttf", 32);
    }

    if (g_boss_name_font != NULL && boss_ref->config_boss != NULL) {
        const char *nom_boss = boss_ref->config_boss->nom_affiche;
        SDL_Surface *surf_shadow = TTF_RenderText_Blended(g_boss_name_font, nom_boss, SDL_strlen(nom_boss), color_ombre);
        SDL_Surface *surf_main = TTF_RenderText_Blended(g_boss_name_font, nom_boss, SDL_strlen(nom_boss), color_nom);

        if (surf_shadow != NULL && surf_main != NULL) {
            SDL_Texture *tex_shadow = SDL_CreateTextureFromSurface(renderer, surf_shadow);
            SDL_Texture *tex_main = SDL_CreateTextureFromSurface(renderer, surf_main);

            if (tex_shadow != NULL && tex_main != NULL) {
                float centre_x = fond.x + fond.w * 0.5f;
                SDL_FRect rect_shadow = {
                    centre_x - (float)surf_shadow->w * 0.5f + 2.0f,
                    fond.y - 34.0f,
                    (float)surf_shadow->w,
                    (float)surf_shadow->h
                };
                SDL_FRect rect_main = {
                    centre_x - (float)surf_main->w * 0.5f,
                    fond.y - 36.0f,
                    (float)surf_main->w,
                    (float)surf_main->h
                };

                SDL_RenderTexture(renderer, tex_shadow, NULL, &rect_shadow);
                SDL_RenderTexture(renderer, tex_main, NULL, &rect_main);
            }

            if (tex_shadow != NULL) SDL_DestroyTexture(tex_shadow);
            if (tex_main != NULL) SDL_DestroyTexture(tex_main);
        }

        if (surf_shadow != NULL) SDL_DestroySurface(surf_shadow);
        if (surf_main != NULL) SDL_DestroySurface(surf_main);
    }
}

static void appliquer_degats_contact(boss_t *boss_ref) {
    if (boss_ref->est_battu || !boss_ref->est_agro) {
        return;
    }

    SDL_FRect rect_perso = {screen_center_x(), screen_center_y(), 40.0f, 60.0f};
    SDL_FRect rect_boss = get_boss_hitbox_ecran(boss_ref);

    if (SDL_HasRectIntersectionFloat(&rect_perso, &rect_boss)) {
        appliquer_degats_joueur(1);
    }
}

static void appliquer_degats_joueur_sur_boss(boss_t *boss_ref) {
    if (boss_ref->est_battu || combat_en_cours || !g_last_combat_en_cours) {
        return;
    }

    SDL_FRect zone;
    get_zone_attaque_joueur(&zone);
    SDL_FRect boss_hitbox = get_boss_hitbox_ecran(boss_ref);

    if (SDL_HasRectIntersectionFloat(&zone, &boss_hitbox)) {
        boss_ref->vie -= 3;
        if (boss_ref->vie <= 0) {
            boss_ref->vie = 0;
        }

        if (boss_ref->type == TYPE_BOSS_MINOTAURE && boss_ref->vie > 0) {
            Uint32 now = SDL_GetTicks();
            if (now >= boss_ref->stun_recharge_jusqua) {
                boss_ref->stun_jusqua = now + BOSS_MINOTAURE_STUN_MS;
                boss_ref->stun_recharge_jusqua = now + BOSS_MINOTAURE_STUN_RECHARGE_MS;
                boss_ref->etat_anim = 0;
                boss_ref->frame_anim_attaque = 0;
                boss_ref->a_cible_patrouille = 0;
            }
        }
    }

    renvoyer_projectiles_touches(boss_ref);
}

static void declencher_animation_attaque(boss_t *boss_ref) {
    if (boss_ref->etat_anim != 1) {
        boss_ref->etat_anim = 1;
        boss_ref->frame_anim_attaque = 0;
    }

    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        boss_ref->degats_melee_appliques = 0;
    }
}

static int minotaure_position_libre_centre(float centre_x, float centre_y) {
    if (g_map_navigation == NULL) {
        return 1;
    }

    int tx = (int)(centre_x / DISPLAY_TILE_SIZE);
    int ty = (int)(centre_y / DISPLAY_TILE_SIZE);
    if (tx < 0 || ty < 0 || tx >= W_MAP || ty >= H_MAP) {
        return 0;
    }

    return g_map_navigation[tx][ty].type == terreP;
}

static int minotaure_trouver_prochaine_case(const boss_t *boss_ref, float target_x, float target_y,
                                            float *next_x, float *next_y) {
    if (g_map_navigation == NULL || boss_ref == NULL) {
        return 0;
    }

    float boss_cx = 0.0f;
    float boss_cy = 0.0f;
    get_boss_centre_monde(boss_ref, &boss_cx, &boss_cy);

    int start_tx = (int)(boss_cx / DISPLAY_TILE_SIZE);
    int start_ty = (int)(boss_cy / DISPLAY_TILE_SIZE);
    int goal_tx = (int)(target_x / DISPLAY_TILE_SIZE);
    int goal_ty = (int)(target_y / DISPLAY_TILE_SIZE);

    if (start_tx < 0 || start_ty < 0 || start_tx >= W_MAP || start_ty >= H_MAP) {
        return 0;
    }
    if (goal_tx < 0 || goal_ty < 0 || goal_tx >= W_MAP || goal_ty >= H_MAP) {
        return 0;
    }

    if (g_map_navigation[start_tx][start_ty].type != terreP || g_map_navigation[goal_tx][goal_ty].type != terreP) {
        return 0;
    }

    int file_x[W_MAP * H_MAP];
    int file_y[W_MAP * H_MAP];
    int parent_x[W_MAP][H_MAP];
    int parent_y[W_MAP][H_MAP];
    unsigned char visited[W_MAP][H_MAP];

    for (int x = 0; x < W_MAP; x++) {
        for (int y = 0; y < H_MAP; y++) {
            parent_x[x][y] = -1;
            parent_y[x][y] = -1;
            visited[x][y] = 0;
        }
    }

    int front = 0;
    int rear = 0;
    file_x[rear] = start_tx;
    file_y[rear] = start_ty;
    rear++;
    visited[start_tx][start_ty] = 1;

    const int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    while (front < rear) {
        int cx = file_x[front];
        int cy = file_y[front];
        front++;

        if (cx == goal_tx && cy == goal_ty) {
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nx = cx + dirs[i][0];
            int ny = cy + dirs[i][1];
            if (nx < 0 || ny < 0 || nx >= W_MAP || ny >= H_MAP) {
                continue;
            }
            if (visited[nx][ny] || g_map_navigation[nx][ny].type != terreP) {
                continue;
            }

            visited[nx][ny] = 1;
            parent_x[nx][ny] = cx;
            parent_y[nx][ny] = cy;
            file_x[rear] = nx;
            file_y[rear] = ny;
            rear++;
        }
    }

    if (!visited[goal_tx][goal_ty]) {
        return 0;
    }

    int step_x = goal_tx;
    int step_y = goal_ty;
    while (parent_x[step_x][step_y] != start_tx || parent_y[step_x][step_y] != start_ty) {
        int px = parent_x[step_x][step_y];
        int py = parent_y[step_x][step_y];
        if (px < 0 || py < 0) {
            return 0;
        }
        step_x = px;
        step_y = py;
    }

    if (next_x) *next_x = (float)step_x * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE * 0.5f);
    if (next_y) *next_y = (float)step_y * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE * 0.5f);
    return 1;
}

static int minotaure_joueur_visible(const boss_t *boss_ref) {
    if (g_map_navigation == NULL) {
        return joueur_dans_zone_agro(boss_ref);
    }

    return joueur_dans_zone_agro(boss_ref);
}

static void deplacer_minotaure_vers(boss_t *boss_ref, float target_x, float target_y, float dt) {
    float boss_cx = 0.0f;
    float boss_cy = 0.0f;
    get_boss_centre_monde(boss_ref, &boss_cx, &boss_cy);

    float dx = target_x - boss_cx;
    float dy = target_y - boss_cy;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < 1.0f) {
        return;
    }

    if (fabsf(dx) > 3.0f) {
        boss_ref->minotaure_regarde_droite = (dx > 0.0f) ? 1 : 0;
    }

    float vx = (dx / dist) * BOSS_MINOTAURE_MOVE_SPEED * dt;
    float vy = (dy / dist) * BOSS_MINOTAURE_MOVE_SPEED * dt;

    /* Offsets inverse de get_boss_centre_monde() */
    const float off_x = 155.0f;
    const float off_y = 192.5f;

    float new_cx = boss_cx + vx;
    float new_cy = boss_cy;
    if (minotaure_position_libre_centre(new_cx, new_cy)) {
        boss_ref->x = new_cx - off_x;
        boss_ref->y = new_cy - off_y;
    }

    new_cx = boss_cx;
    new_cy = boss_cy + vy;
    if (minotaure_position_libre_centre(new_cx, new_cy)) {
        boss_ref->x = new_cx - off_x;
        boss_ref->y = new_cy - off_y;
    }
}

static void choisir_cible_patrouille_minotaure(boss_t *boss_ref) {
    for (int essais = 0; essais < 18; essais++) {
        float angle = ((float)(rand() % 360)) * (3.14159265f / 180.0f);
        float rayon = 70.0f + (float)(rand() % (int)BOSS_MINOTAURE_PATROL_RADIUS);
        float boss_cx = 0.0f;
        float boss_cy = 0.0f;
        get_boss_centre_monde(boss_ref, &boss_cx, &boss_cy);
        float cx = boss_cx + cosf(angle) * rayon;
        float cy = boss_cy + sinf(angle) * rayon;

        if (minotaure_position_libre_centre(cx, cy)) {
            boss_ref->patrouille_cible_x = cx;
            boss_ref->patrouille_cible_y = cy;
            boss_ref->a_cible_patrouille = 1;
            return;
        }
    }

    boss_ref->patrouille_cible_x = boss_ref->spawn_x + 150.0f;
    boss_ref->patrouille_cible_y = boss_ref->spawn_y + 175.0f;
    boss_ref->a_cible_patrouille = 1;
}

static void appliquer_degats_hache_minotaure(boss_t *boss_ref) {
    if (boss_ref->degats_melee_appliques || boss_ref->etat_anim != 1) {
        return;
    }

    if (boss_ref->frame_anim_attaque < 3 || boss_ref->frame_anim_attaque > 7) {
        return;
    }

    SDL_FRect rect_perso = {screen_center_x(), screen_center_y(), 40.0f, 60.0f};
    SDL_FRect zone_hache = {
        boss_ref->x + perso.x + 18.0f,
        boss_ref->y + perso.y + 126.0f,
        172.0f,
        132.0f
    };

    if (boss_ref->minotaure_regarde_droite) {
        zone_hache.x = boss_ref->x + perso.x + 110.0f;
    }

    if (SDL_HasRectIntersectionFloat(&rect_perso, &zone_hache)) {
        appliquer_degats_joueur(boss_ref->attaque);
    }

    boss_ref->degats_melee_appliques = 1;
}

static void attaque_minotaure(SDL_Renderer *renderer, boss_t *boss_ref) {
    (void)renderer;

    if (boss_ref->est_battu) {
        return;
    }

    if (SDL_GetTicks() < boss_ref->stun_jusqua) {
        boss_ref->etat_anim = 0;
        return;
    }

    if (boss_ref->etat_anim == 1) {
        return;
    }

    float joueur_x = 0.0f;
    float joueur_y = 0.0f;
    float boss_cx = 0.0f;
    float boss_cy = 0.0f;
    get_player_world_center(&joueur_x, &joueur_y);
    get_boss_centre_monde(boss_ref, &boss_cx, &boss_cy);
    boss_ref->minotaure_regarde_droite = (joueur_x > boss_cx) ? 1 : 0;

    int visible = minotaure_joueur_visible(boss_ref);
    boss_ref->est_agro = visible;

    if (visible && boss_ref->etat_anim != 1) {
        float dx = joueur_x - boss_cx;
        float dy = joueur_y - boss_cy;
        float dist = sqrtf(dx * dx + dy * dy);
        Uint32 now = SDL_GetTicks();

        if (dist <= BOSS_MINOTAURE_MELEE_RANGE && now - boss_ref->cooldown_attaque >= boss_ref->config_boss->cooldown_projectile_ms) {
            boss_ref->cooldown_attaque = now;
            declencher_animation_attaque(boss_ref);
            boss_ref->a_cible_patrouille = 0;
            return;
        }

        float prochaine_x = 0.0f;
        float prochaine_y = 0.0f;
        if (minotaure_trouver_prochaine_case(boss_ref, joueur_x, joueur_y, &prochaine_x, &prochaine_y)) {
            deplacer_minotaure_vers(boss_ref, prochaine_x, prochaine_y, g_boss_dt);
        } else {
            deplacer_minotaure_vers(boss_ref, joueur_x, joueur_y, g_boss_dt);
        }
        boss_ref->etat_anim = 3;
        boss_ref->a_cible_patrouille = 0;
        return;
    }

    if (!boss_ref->a_cible_patrouille) {
        choisir_cible_patrouille_minotaure(boss_ref);
    }

    float pdx = boss_ref->patrouille_cible_x - boss_cx;
    float pdy = boss_ref->patrouille_cible_y - boss_cy;
    float pdist = sqrtf(pdx * pdx + pdy * pdy);
    if (pdist < 14.0f) {
        boss_ref->a_cible_patrouille = 0;
        boss_ref->etat_anim = 0;
    } else {
        deplacer_minotaure_vers(boss_ref, boss_ref->patrouille_cible_x, boss_ref->patrouille_cible_y, g_boss_dt * 0.65f);
        boss_ref->etat_anim = 3;
    }
}

static void attaque_demon_de_feu(SDL_Renderer *renderer, boss_t *boss_ref) {
    Uint32 now = SDL_GetTicks();

    if (boss_ref->config_boss->active_projectile && now - boss_ref->cooldown_attaque >= boss_ref->config_boss->cooldown_projectile_ms) {
        spawn_projectile_vers_joueur(boss_ref);
        boss_ref->cooldown_attaque = now;
        declencher_animation_attaque(boss_ref);
    }

    if (boss_ref->config_boss->active_zone && boss_ref->phase >= 2 && now - boss_ref->cooldown_zone >= boss_ref->config_boss->cooldown_zone_ms) {
        boss_ref->cooldown_zone = now;
        g_zone_warning_actif = 1;
        g_zone_warning_start = now;
        g_zone_impact_pending = 1;
        g_zone_impact_tick = now + BOSS_ZONE_WARNING_MS;
        
        // Calculer la direction vers le joueur pour avancer la zone AOE devant le boss
        get_boss_centre_monde(boss_ref, &g_zone_center_x, &g_zone_center_y);
        float joueur_x = 0.0f, joueur_y = 0.0f;
        get_player_world_center(&joueur_x, &joueur_y);
        float dx = joueur_x - g_zone_center_x;
        float dy = joueur_y - g_zone_center_y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist > 0.001f) {
            dx /= dist;
            dy /= dist;
            // Decaler la zone environ 280 pixels devant le boss
            g_zone_center_x += dx * 280.0f;
            g_zone_center_y += dy * 280.0f;
        }
        
        declencher_animation_attaque(boss_ref);
    }

    if (boss_ref->config_boss->active_invocation && boss_ref->phase >= 3 &&
        now - boss_ref->cooldown_invocation >= boss_ref->config_boss->cooldown_invocation_ms) {
        boss_ref->cooldown_invocation = now;
        invoquer_minions_autour_boss(renderer, boss_ref, 3);
        declencher_animation_attaque(boss_ref);
    }
}

static void attaque_sentinelle(SDL_Renderer *renderer, boss_t *boss_ref) {
    Uint32 now = SDL_GetTicks();

    if (boss_ref->config_boss->active_projectile && now - boss_ref->cooldown_attaque >= boss_ref->config_boss->cooldown_projectile_ms) {
        spawn_projectile_vers_joueur(boss_ref);
        boss_ref->cooldown_attaque = now;
        declencher_animation_attaque(boss_ref);
    }

    if (boss_ref->config_boss->active_zone && now - boss_ref->cooldown_zone >= boss_ref->config_boss->cooldown_zone_ms) {
        boss_ref->cooldown_zone = now;
        g_zone_warning_actif = 1;
        g_zone_warning_start = now;
        g_zone_impact_pending = 1;
        g_zone_impact_tick = now + BOSS_ZONE_WARNING_MS;
        get_player_world_center(&g_zone_center_x, &g_zone_center_y);
        declencher_animation_attaque(boss_ref);
    }

    (void)renderer;
}

static fonction_attaque_boss_t obtenir_fonction_attaque_boss(const boss_t *boss_ref) {
    switch (boss_ref->type) {
        case TYPE_BOSS_MINOTAURE:
            return attaque_minotaure;
        case TYPE_BOSS_SENTINELLE:
            return attaque_sentinelle;
        case TYPE_BOSS_DEMON_DE_FEU:
        default:
            return attaque_demon_de_feu;
    }
}

static void mettre_a_jour_phase_boss(boss_t *boss_ref) {
    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        boss_ref->phase = 1;
        return;
    }

    if (boss_ref->type == TYPE_BOSS_SENTINELLE) {
        if (boss_ref->vie <= (int)((float)boss_ref->vie_max * 0.55f)) {
            boss_ref->phase = 3;
        } else if (boss_ref->vie <= (int)((float)boss_ref->vie_max * 0.85f) && boss_ref->phase < 2) {
            boss_ref->phase = 2;
        }
        return;
    }

    if (boss_ref->vie <= (int)((float)boss_ref->vie_max * 0.40f)) {
        boss_ref->phase = 3;
    } else if (boss_ref->vie <= (int)((float)boss_ref->vie_max * 0.75f) && boss_ref->phase < 2) {
        boss_ref->phase = 2;
    }
}

void init_boss(SDL_Renderer *renderer, boss_t *boss_ref, type_boss_t type, float x, float y) {
    const boss_config_t *config = obtenir_config_boss(type);

    boss_ref->x = x;
    boss_ref->y = y;
    boss_ref->type = config->type;
    boss_ref->config_boss = config;
    boss_ref->vie = config->vie_max;
    boss_ref->vie_max = config->vie_max;
    boss_ref->attaque = config->attaque_contact;
    boss_ref->cooldown_attaque = SDL_GetTicks();
    boss_ref->cooldown_zone = SDL_GetTicks();
    boss_ref->cooldown_invocation = SDL_GetTicks();
    boss_ref->fin_animation_attaque = 0;
    boss_ref->drop_effectue = 0;
    boss_ref->phase = 1;
    boss_ref->est_battu = 0;
    boss_ref->frame_anim_repos = 0;
    boss_ref->frame_anim_attaque = 0;
    boss_ref->frame_anim_mort = 0;
    boss_ref->etat_anim = 0;
    boss_ref->animation_timer = SDL_GetTicks();
    boss_ref->texture = IMG_LoadTexture(renderer, config->chemin_sprite);
    boss_ref->est_agro = 0;
    boss_ref->rayon_detection = (type == TYPE_BOSS_MINOTAURE) ? BOSS_MINOTAURE_AGGRO_RADIUS : BOSS_AGGRO_RADIUS;
    boss_ref->spawn_x = x;
    boss_ref->spawn_y = y;
    boss_ref->patrouille_cible_x = x;
    boss_ref->patrouille_cible_y = y;
    boss_ref->a_cible_patrouille = 0;
    boss_ref->degats_melee_appliques = 0;
    boss_ref->minotaure_regarde_droite = 0;
    boss_ref->stun_jusqua = 0;
    boss_ref->stun_recharge_jusqua = 0;

    if (config->active_projectile) {
        charger_texture_projectile(renderer, boss_ref);
    }
    clear_projectiles();
    g_zone_warning_actif = 0;
    g_zone_impact_pending = 0;
    g_zone_impact_tick = 0;
    g_zone_center_x = 0.0f;
    g_zone_center_y = 0.0f;
    g_last_combat_en_cours = 0;
}

void afficher_boss(SDL_Renderer *renderer, boss_t *boss_ref) {
    if (boss_ref->type != TYPE_BOSS_MINOTAURE && boss_ref->est_battu && boss_ref->frame_anim_mort >= 15) {
        return;
    }

    SDL_FRect dstrect = {(boss_ref->x) + perso.x, (boss_ref->y) + perso.y, 576.0f, 320.0f};
    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        dstrect.w = 300.0f;
        dstrect.h = 300.0f;
    }

    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        if (boss_ref->etat_anim == 2) {
            SDL_FRect srcrect = {boss_ref->frame_anim_mort * BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_DEATH_Y, BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_FRAME_H};
            SDL_FPoint centre = {dstrect.w * 0.5f, dstrect.h * 0.5f};
            SDL_FlipMode flip = boss_ref->minotaure_regarde_droite ?  SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderTextureRotated(renderer, boss_ref->texture, &srcrect, &dstrect, 0.0, &centre, flip);
            return;
        }

        if (boss_ref->etat_anim == 0) {
            SDL_FRect srcrect = {boss_ref->frame_anim_repos * BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_IDLE_Y, BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_FRAME_H};
            SDL_FPoint centre = {dstrect.w * 0.5f, dstrect.h * 0.5f};
            SDL_FlipMode flip = boss_ref->minotaure_regarde_droite ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderTextureRotated(renderer, boss_ref->texture, &srcrect, &dstrect, 0.0, &centre, flip);
        } else if (boss_ref->etat_anim == 1) {
            SDL_FRect srcrect = {boss_ref->frame_anim_attaque * BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_ATTACK_Y, BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_FRAME_H};
            SDL_FPoint centre = {dstrect.w * 0.5f, dstrect.h * 0.5f};
            SDL_FlipMode flip = boss_ref->minotaure_regarde_droite ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderTextureRotated(renderer, boss_ref->texture, &srcrect, &dstrect, 0.0, &centre, flip);
        } else if (boss_ref->etat_anim == 3) {
            SDL_FRect srcrect = {boss_ref->frame_anim_repos * BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_WALK_Y, BOSS_MINOTAURE_FRAME_W, BOSS_MINOTAURE_FRAME_H};
            SDL_FPoint centre = {dstrect.w * 0.5f, dstrect.h * 0.5f};
            SDL_FlipMode flip = boss_ref->minotaure_regarde_droite ?  SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderTextureRotated(renderer, boss_ref->texture, &srcrect, &dstrect, 0.0, &centre, flip);
        }

        afficher_barre_vie_boss(renderer, boss_ref);
        return;
    }

    if (boss_ref->etat_anim == 0) {
        SDL_FRect srcrect = {boss_ref->frame_anim_repos * 288.0f, 0, 288, 160};
        SDL_RenderTexture(renderer, boss_ref->texture, &srcrect, &dstrect);
    } else if (boss_ref->etat_anim == 1) {
        SDL_FRect srcrect = {boss_ref->frame_anim_attaque * 288.0f, 320, 288, 160};
        SDL_RenderTexture(renderer, boss_ref->texture, &srcrect, &dstrect);
    } else if (boss_ref->etat_anim == 2) {
        SDL_FRect srcrect = {boss_ref->frame_anim_mort * 288.0f, 640, 288, 160};
        SDL_RenderTexture(renderer, boss_ref->texture, &srcrect, &dstrect);
    }

    afficher_projectiles(renderer, boss_ref);
    afficher_zone_warning(renderer, boss_ref);
    afficher_barre_vie_boss(renderer, boss_ref);
}

void mettre_a_jour_animation_boss(boss_t *boss_ref) {
    Uint32 maintenant = SDL_GetTicks();
    if (maintenant - boss_ref->animation_timer <= 125) {
        return;
    }

    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        if (boss_ref->etat_anim == 0) {
            boss_ref->frame_anim_repos = (boss_ref->frame_anim_repos + 1) % BOSS_MINOTAURE_IDLE_FRAMES;
        } else if (boss_ref->etat_anim == 3) {
            boss_ref->frame_anim_repos = (boss_ref->frame_anim_repos + 1) % BOSS_MINOTAURE_WALK_FRAMES;
        } else if (boss_ref->etat_anim == 1) {
            if (boss_ref->frame_anim_attaque < (BOSS_MINOTAURE_ATTACK_FRAMES - 1)) {
                boss_ref->frame_anim_attaque++;
            } else {
                boss_ref->etat_anim = 0;
                boss_ref->frame_anim_attaque = 0;
                boss_ref->degats_melee_appliques = 0;
            }
        } else if (boss_ref->etat_anim == 2) {
            if (boss_ref->frame_anim_mort < (BOSS_MINOTAURE_DEATH_FRAMES - 1)) {
                boss_ref->frame_anim_mort++;
            }
        }

        boss_ref->animation_timer = maintenant;
        return;
    }

    if (boss_ref->etat_anim == 0) {
        boss_ref->frame_anim_repos = (boss_ref->frame_anim_repos + 1) % 6;
    } else if (boss_ref->etat_anim == 1) {
        if (boss_ref->frame_anim_attaque < 14) {
            boss_ref->frame_anim_attaque++;
        } else {
            boss_ref->etat_anim = 0;
            boss_ref->frame_anim_attaque = 0;
        }
    } else if (boss_ref->etat_anim == 2) {
        if (boss_ref->frame_anim_mort < 15) {
            boss_ref->frame_anim_mort++;
        }
    }

    boss_ref->animation_timer = maintenant;
}

void mettre_a_jour_boss(SDL_Renderer *renderer, boss_t *boss_ref) {
    static Uint32 last_tick = 0;
    Uint32 now = SDL_GetTicks();
    if (last_tick == 0) {
        last_tick = now;
    }
    float dt = (now - last_tick) / 1000.0f;
    if (dt <= 0.0f) {
        dt = 0.001f;
    }
    last_tick = now;
    g_boss_dt = dt;

    if (boss_ref->est_battu) {
        mettre_a_jour_animation_boss(boss_ref);
        boss_ref->est_agro = 0;
        g_last_combat_en_cours = combat_en_cours;
        return;
    }

    boss_ref->est_agro = joueur_dans_zone_agro(boss_ref);

    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        boss_ref->est_agro = minotaure_joueur_visible(boss_ref);
    }

    if (!boss_ref->est_agro) {
        boss_ref->etat_anim = 0;
        boss_ref->frame_anim_attaque = 0;
        g_zone_warning_actif = 0;
        g_zone_impact_pending = 0;
    }

    appliquer_degats_joueur_sur_boss(boss_ref);

    if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
        appliquer_degats_hache_minotaure(boss_ref);
    } else {
        update_projectiles(dt, boss_ref);
    }

    if (boss_ref->type != TYPE_BOSS_MINOTAURE) {
        appliquer_degats_contact(boss_ref);
    }

    if (boss_ref->type != TYPE_BOSS_MINOTAURE && g_zone_impact_pending && now >= g_zone_impact_tick) {
        float joueur_cx = 0.0f;
        float joueur_cy = 0.0f;
        get_player_world_center(&joueur_cx, &joueur_cy);
        float dx = joueur_cx - g_zone_center_x;
        float dy = joueur_cy - g_zone_center_y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist <= BOSS_ZONE_RADIUS) {
            appliquer_degats_joueur(2);
        }
        g_zone_impact_pending = 0;
        g_zone_warning_actif = 0;
    }

    mettre_a_jour_phase_boss(boss_ref);

    if (boss_ref->type == TYPE_BOSS_MINOTAURE && now < boss_ref->stun_jusqua) {
        boss_ref->etat_anim = 0;
    } else {
        boss_attaque(renderer, boss_ref);
    }

    if (boss_ref->vie <= 0 && !boss_ref->est_battu) {
        boss_ref->est_battu = 1;
        boss_ref->etat_anim = 2;
        if (boss_ref->type == TYPE_BOSS_MINOTAURE) {
            boss_ref->frame_anim_mort = 0;
            boss_ref->fin_animation_attaque = now;
        }
        boss_ref->frame_anim_attaque = 0;
        clear_projectiles();
        supprimer_minions_boss(boss_ref);
        g_zone_warning_actif = 0;
        g_zone_impact_pending = 0;

        if (!boss_ref->drop_effectue && index_item < MAX_ITEMS) {
            t_Item *drop = init_item(ENGRENAGE, renderer, boss_ref->x + 260.0f, boss_ref->y + 140.0f);
            if (drop != NULL) {
                items[index_item++] = drop;
                boss_ref->drop_effectue = 1;
            }
        }
    }

    mettre_a_jour_animation_boss(boss_ref);
    g_last_combat_en_cours = combat_en_cours;
}

void reset_boss_for_retry(boss_t *boss_ref) {
    /**
     * Réinitialise le boss après un retry du joueur après sa mort.
     * - Supprime les minions du boss
     * - Réinitialise la vie et l'état du boss
     */
    supprimer_minions_boss(boss_ref);
    
    boss_ref->vie = boss_ref->vie_max;
    boss_ref->est_battu = 0;
    boss_ref->est_agro = 0;
    boss_ref->phase = 1;
    boss_ref->drop_effectue = 0;
    
    Uint32 now = SDL_GetTicks();
    boss_ref->cooldown_attaque = now;
    boss_ref->cooldown_zone = now;
    boss_ref->cooldown_invocation = now;
    
    boss_ref->etat_anim = 0;
    boss_ref->frame_anim_repos = 0;
    boss_ref->frame_anim_attaque = 0;
    boss_ref->frame_anim_mort = 0;
    
    clear_projectiles();
    g_zone_warning_actif = 0;
    g_zone_impact_pending = 0;
}

void boss_attaque(SDL_Renderer *renderer, boss_t *boss_ref) {
    if (boss_ref->est_battu) {
        return;
    }

    if (!boss_ref->est_agro && boss_ref->type != TYPE_BOSS_MINOTAURE) {
        return;
    }

    fonction_attaque_boss_t attack_fn = obtenir_fonction_attaque_boss(boss_ref);
    if (attack_fn != NULL) {
        attack_fn(renderer, boss_ref);
    }
}

void detruire_boss(boss_t *boss_ref) {
    if (boss_ref != NULL && boss_ref->texture) {
        SDL_DestroyTexture(boss_ref->texture);
        boss_ref->texture = NULL;
    }
    clear_projectiles();
    if (boss_ref != NULL) supprimer_minions_boss(boss_ref);
    g_zone_warning_actif = 0;
    g_zone_impact_pending = 0;
    g_zone_impact_tick = 0;
    g_zone_center_x = 0.0f;
    g_zone_center_y = 0.0f;
    g_last_combat_en_cours = 0;

    if (g_texture_pawn_invoke != NULL) {
        SDL_DestroyTexture(g_texture_pawn_invoke);
        g_texture_pawn_invoke = NULL;
        g_type_boss_minion_charge = (type_boss_t)-1;
    }
    if (g_texture_projectile != NULL) {
        SDL_DestroyTexture(g_texture_projectile);
        g_texture_projectile = NULL;
        g_type_boss_projectile_charge = (type_boss_t)-1;
    }
    if (g_boss_name_font != NULL) {
        TTF_CloseFont(g_boss_name_font);
        g_boss_name_font = NULL;
    }
}



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
#define BOSS_ZONE_RADIUS 190.0f
#define BOSS_ZONE_WARNING_MS 900
#define BOSS_REFLECTED_PROJECTILE_SPEED 420.0f
#define BOSS_REFLECTED_PROJECTILE_DAMAGE 8
#define BOSS_HEALTHBAR_Y 62.0f
#define BOSS_NAME_TEXT "FIREDEMON"
#define BOSS_MINION_MAX 6

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

static int compter_minions_boss(void) {
    int count = 0;
    for (int i = 0; i < MAX_MOB && mobs[i] != NULL; i++) {
        if (mobs[i]->id == 3) {
            count++;
        }
    }
    return count;
}

static void supprimer_minions_boss(void) {
    int i = 0;
    while (i < MAX_MOB && mobs[i] != NULL) {
        if (mobs[i]->id == 3) {
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
    *cx = boss_ref->x + 290.0f;
    *cy = boss_ref->y + 165.0f;
}

static void clear_projectiles(void) {
    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++) {
        g_projectiles[i].actif = 0;
        g_projectiles[i].renvoye = 0;
    }
}

static void charger_texture_projectile(SDL_Renderer *renderer) {
    if (g_texture_projectile == NULL && renderer != NULL) {
        g_texture_projectile = IMG_LoadTexture(renderer, "assets/personnage/boss1/attaque_ligne.png");
        if (g_texture_projectile == NULL) {
            SDL_Log("Erreur chargement projectile boss : %s", SDL_GetError());
        }
    }
}

static void get_zone_attaque_joueur(SDL_FRect *zone) {
    const float char_screen_x = 500.0f;
    const float char_screen_y = 400.0f;
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
    SDL_FRect rect_boss = {
        boss_ref->x + perso.x + 95.0f,
        boss_ref->y + perso.y + 55.0f,
        390.0f,
        220.0f
    };
    return rect_boss;
}

static SDL_Texture *get_texture_pawn_existante(SDL_Renderer *renderer) {
    for (int i = 0; i < MAX_MOB && mobs[i] != NULL; i++) {
        if (mobs[i]->id == 3 && mobs[i]->texture != NULL) {
            return mobs[i]->texture;
        }
    }

    if (g_texture_pawn_invoke == NULL && renderer != NULL) {
        g_texture_pawn_invoke = IMG_LoadTexture(renderer, "assets/personnage/boss1/minions/Warrior_Run.png");
    }
    return g_texture_pawn_invoke;
}

static void invoquer_minions_autour_boss(SDL_Renderer *renderer, const boss_t *boss_ref, int nombre) {
    SDL_Texture *tex_pawn = get_texture_pawn_existante(renderer);
    if (tex_pawn == NULL) {
        return;
    }

    int deja_presents = compter_minions_boss();
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

    float joueur_x = -perso.x + 520.0f;
    float joueur_y = -perso.y + 430.0f;
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
        m->drop_chance = 0;

        mobs[idx] = m;
    }
}

static void appliquer_degats_joueur(int degats) {
    if (degats <= 0) {
        return;
    }

    if (perso.vie > 0 && perso.invincibiliter_timer == 0) {
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
    float joueur_cx = -perso.x + 520.0f;
    float joueur_cy = -perso.y + 430.0f;

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

static void update_projectiles(float dt) {
    Uint32 now = SDL_GetTicks();
    SDL_FRect hitbox_perso = {500.0f, 400.0f, 40.0f, 60.0f};
    SDL_FRect boss_hitbox = get_boss_hitbox_ecran(&boss1);

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
            g_projectiles[i].x + perso.x - 8.0f,
            g_projectiles[i].y + perso.y - 8.0f,
            16.0f,
            16.0f
        };

        if (!g_projectiles[i].renvoye && SDL_HasRectIntersectionFloat(&rect_proj, &hitbox_perso)) {
            appliquer_degats_joueur(1);
            g_projectiles[i].actif = 0;
            continue;
        }

        if (g_projectiles[i].renvoye && SDL_HasRectIntersectionFloat(&rect_proj, &boss_hitbox)) {
            if (!boss1.est_battu) {
                boss1.vie -= BOSS_REFLECTED_PROJECTILE_DAMAGE;
                if (boss1.vie < 0) {
                    boss1.vie = 0;
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
            g_projectiles[i].x + perso.x - 8.0f,
            g_projectiles[i].y + perso.y - 8.0f,
            16.0f,
            16.0f
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

static void afficher_projectiles(SDL_Renderer *renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    charger_texture_projectile(renderer);

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
    if (boss_ref->est_battu || boss_ref->vie_max <= 0) {
        return;
    }

    float ratio = (float)boss_ref->vie / (float)boss_ref->vie_max;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    SDL_FRect fond = {250.0f, BOSS_HEALTHBAR_Y, 500.0f, 24.0f};
    SDL_FRect vie = {252.0f, BOSS_HEALTHBAR_Y + 2.0f, 496.0f * ratio, 20.0f};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 210);
    SDL_RenderFillRect(renderer, &fond);
    SDL_SetRenderDrawColor(renderer, 210, 50, 45, 230);
    SDL_RenderFillRect(renderer, &vie);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 210);
    SDL_RenderRect(renderer, &fond);

    if (g_boss_name_font == NULL) {
        g_boss_name_font = TTF_OpenFont("assets/police.ttf", 32);
    }

    if (g_boss_name_font != NULL) {
        SDL_Color rouge = {235, 35, 40, 255};
        SDL_Color ombre = {35, 0, 0, 220};
        SDL_Surface *surf_shadow = TTF_RenderText_Blended(g_boss_name_font, BOSS_NAME_TEXT, SDL_strlen(BOSS_NAME_TEXT), ombre);
        SDL_Surface *surf_main = TTF_RenderText_Blended(g_boss_name_font, BOSS_NAME_TEXT, SDL_strlen(BOSS_NAME_TEXT), rouge);

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
    if (boss_ref->est_battu) {
        return;
    }

    SDL_FRect rect_perso = {500.0f, 400.0f, 40.0f, 60.0f};
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
    }

    renvoyer_projectiles_touches(boss_ref);
}

void init_boss(SDL_Renderer *renderer, boss_t *boss_ref, float x, float y, int vie_max, int attaque) {
    boss_ref->x = x;
    boss_ref->y = y;
    boss_ref->vie = vie_max;
    boss_ref->vie_max = vie_max;
    boss_ref->attaque = attaque;
    boss_ref->cooldown_attaque = SDL_GetTicks();
    boss_ref->cooldown_zone = SDL_GetTicks();
    boss_ref->cooldown_invocation = SDL_GetTicks();
    boss_ref->fin_animation_attaque = 0;
    boss_ref->drop_effectue = 0;
    boss_ref->phase = 1;
    boss_ref->est_battu = 0;
    boss_ref->animation_frame_idle = 0;
    boss_ref->animation_frame_attack = 0;
    boss_ref->animation_frame_death = 0;
    boss_ref->animation_state = 0;
    boss_ref->animation_timer = SDL_GetTicks();
    boss_ref->texture = IMG_LoadTexture(renderer, "assets/personnage/boss1/boss.png");
    charger_texture_projectile(renderer);
    clear_projectiles();
    g_zone_warning_actif = 0;
    g_zone_impact_pending = 0;
    g_zone_impact_tick = 0;
    g_zone_center_x = 0.0f;
    g_zone_center_y = 0.0f;
    g_last_combat_en_cours = 0;
}

void afficher_boss(SDL_Renderer *renderer, boss_t *boss_ref) {
    if (boss_ref->est_battu && boss_ref->animation_frame_death >= 15) {
        return;
    }

    SDL_FRect dstrect = {(boss_ref->x) + perso.x, (boss_ref->y) + perso.y, 576.0f, 320.0f};
    if (boss_ref->animation_state == 0) {
        SDL_FRect srcrect = {boss_ref->animation_frame_idle * 288.0f, 0, 288, 160};
        SDL_RenderTexture(renderer, boss_ref->texture, &srcrect, &dstrect);
    } else if (boss_ref->animation_state == 1) {
        SDL_FRect srcrect = {boss_ref->animation_frame_attack * 288.0f, 320, 288, 160};
        SDL_RenderTexture(renderer, boss_ref->texture, &srcrect, &dstrect);
    } else if (boss_ref->animation_state == 2) {
        SDL_FRect srcrect = {boss_ref->animation_frame_death * 288.0f, 640, 288, 160};
        SDL_RenderTexture(renderer, boss_ref->texture, &srcrect, &dstrect);
    }

    afficher_projectiles(renderer);
    afficher_zone_warning(renderer, boss_ref);
    afficher_barre_vie_boss(renderer, boss_ref);
}

void update_boss(SDL_Renderer *renderer, boss_t *boss_ref) {
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

    if (boss_ref->est_battu) {
        g_last_combat_en_cours = combat_en_cours;
        return;
    }

    appliquer_degats_joueur_sur_boss(boss_ref);
    update_projectiles(dt);
    appliquer_degats_contact(boss_ref);

    if (g_zone_impact_pending && now >= g_zone_impact_tick) {
        float joueur_cx = -perso.x + 520.0f;
        float joueur_cy = -perso.y + 430.0f;
        float dx = joueur_cx - g_zone_center_x;
        float dy = joueur_cy - g_zone_center_y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist <= BOSS_ZONE_RADIUS) {
            appliquer_degats_joueur(2);
        }
        g_zone_impact_pending = 0;
        g_zone_warning_actif = 0;
    }

    if (boss_ref->vie <= (int)((float)boss_ref->vie_max * 0.40f)) {
        boss_ref->phase = 3;
    } else if (boss_ref->vie <= (int)((float)boss_ref->vie_max * 0.75f)) {
        if (boss_ref->phase < 2) {
            boss_ref->phase = 2;
        }
    }

    boss_attaque(renderer, boss_ref);

    if (boss_ref->vie <= 0 && !boss_ref->est_battu) {
        boss_ref->est_battu = 1;
        boss_ref->animation_state = 2;
        boss_ref->animation_frame_attack = 0;
        clear_projectiles();
        supprimer_minions_boss();
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

    g_last_combat_en_cours = combat_en_cours;
}

void boss_attaque(SDL_Renderer *renderer, boss_t *boss_ref) {
    if (boss_ref->est_battu) {
        return;
    }

    Uint32 now = SDL_GetTicks();

    if (now - boss_ref->cooldown_attaque >= BOSS_PROJECTILE_COOLDOWN_MS) {
        spawn_projectile_vers_joueur(boss_ref);
        boss_ref->cooldown_attaque = now;
        if (boss_ref->animation_state != 1) {
            boss_ref->animation_state = 1;
            boss_ref->animation_frame_attack = 0;
        }
    }

    if (boss_ref->phase >= 2 && now - boss_ref->cooldown_zone >= BOSS_ZONE_COOLDOWN_MS) {
        boss_ref->cooldown_zone = now;
        g_zone_warning_actif = 1;
        g_zone_warning_start = now;
        g_zone_impact_pending = 1;
        g_zone_impact_tick = now + BOSS_ZONE_WARNING_MS;
        get_boss_centre_monde(boss_ref, &g_zone_center_x, &g_zone_center_y);
        if (boss_ref->animation_state != 1) {
            boss_ref->animation_state = 1;
            boss_ref->animation_frame_attack = 0;
        }
    }

    if (boss_ref->phase >= 3 && now - boss_ref->cooldown_invocation >= BOSS_SUMMON_COOLDOWN_MS) {
        boss_ref->cooldown_invocation = now;
        invoquer_minions_autour_boss(renderer, boss_ref, 3);
        if (boss_ref->animation_state != 1) {
            boss_ref->animation_state = 1;
            boss_ref->animation_frame_attack = 0;
        }
    }
}

void Destroy_boss(boss_t *boss_ref) {
    if (boss_ref->texture) {
        SDL_DestroyTexture(boss_ref->texture);
        boss_ref->texture = NULL;
    }
    clear_projectiles();
    supprimer_minions_boss();
    g_zone_warning_actif = 0;
    g_zone_impact_pending = 0;
    g_zone_impact_tick = 0;
    g_zone_center_x = 0.0f;
    g_zone_center_y = 0.0f;
    g_last_combat_en_cours = 0;

    if (g_texture_pawn_invoke != NULL) {
        SDL_DestroyTexture(g_texture_pawn_invoke);
        g_texture_pawn_invoke = NULL;
    }
    if (g_texture_projectile != NULL) {
        SDL_DestroyTexture(g_texture_projectile);
        g_texture_projectile = NULL;
    }
    if (g_boss_name_font != NULL) {
        TTF_CloseFont(g_boss_name_font);
        g_boss_name_font = NULL;
    }
}

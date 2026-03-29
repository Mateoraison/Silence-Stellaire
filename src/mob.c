#include "headers/main.h"

static SDL_Texture * texture_pawns = NULL;
static SDL_Texture * texture_mouton = NULL;
static SDL_Texture * texture_minion_run = NULL;
static SDL_Texture * texture_minion_attack = NULL;
static Uint32 minion_attack_timer = 0;

#define TAILLE_FILE_RESPAWN 256
typedef struct { int id; Uint32 quand; } EntreeRespawn;

static EntreeRespawn file_respawn[TAILLE_FILE_RESPAWN];
static int tete_file = 0;
static int queue_file = 0;

void schedule_respawn(int id, Uint32 delay_ms) {
    int suivant = (queue_file + 1) % TAILLE_FILE_RESPAWN;
    if (suivant == tete_file) return;
    file_respawn[queue_file].id = id;
    file_respawn[queue_file].quand = SDL_GetTicks() + delay_ms;
    queue_file = suivant;
}

static int pop_respawn_prevu(EntreeRespawn *out) {
    if (tete_file == queue_file) return -1;
    Uint32 maintenant = SDL_GetTicks();
    if (file_respawn[tete_file].quand <= maintenant) {
        if (out) *out = file_respawn[tete_file];
        tete_file = (tete_file + 1) % TAILLE_FILE_RESPAWN;
        return 0;
    }
    return -1;
}

void init_mobs(SDL_Renderer * renderer, Mob * mobs[MAX_MOB], t_tile map[W_MAP][H_MAP], int nb_pawns, int nb_moutons) {
    if (texture_pawns == NULL) {
        texture_pawns = IMG_LoadTexture(renderer, "assets/tileset/V2/Tiny_Swords/Units/BlackUnits/Pawn/Pawn.png");
        if (texture_pawns == NULL) {
            SDL_Log("Erreur chargement texture mob: %s", SDL_GetError());
        }
    }

    if (texture_mouton == NULL) {
        texture_mouton = IMG_LoadTexture(renderer, "assets/tileset/V2/Tiny_Swords/Terrain/Resources/Meat/Sheep/Sheep_Idle.png");
        if (texture_mouton == NULL) {
            SDL_Log("Erreur chargement texture mob: %s", SDL_GetError());
        }
    }

    if (texture_minion_run == NULL) {
        texture_minion_run = IMG_LoadTexture(renderer, "assets/personnage/boss1/minions/Warrior_Run.png");
    }
    if (texture_minion_attack == NULL) {
        texture_minion_attack = IMG_LoadTexture(renderer, "assets/personnage/boss1/minions/Warrior_Attack.png");
    }

    int index = 0;
    int rayon_app = 2;
    float joueur_monde_x = -perso.x + 500.0f;
    float joueur_monde_y = -perso.y + 400.0f;
    int tuile_joueur_x = (int)(joueur_monde_x / DISPLAY_TILE_SIZE);
    int tuile_joueur_y = (int)(joueur_monde_y / DISPLAY_TILE_SIZE);

    int capacite = W_MAP * H_MAP;
    typedef struct { int x; int y; } PositionTuile;
    PositionTuile *candidats = malloc(sizeof(PositionTuile) * capacite);
    int nb_candidats = 0;

    for (int tx = 0; tx < W_MAP; tx++) {
        for (int ty = 0; ty < H_MAP; ty++) {
            type_t t = map[tx][ty].type;
            int est_terre = (t == terreP || t == terreCHG || t == terreCHD || t == terreCBG || t == terreCBD || t == terreH || t == terreB || t == terreG || t == terreD);
            if (!est_terre) continue;
            SDL_Rect r = { tx * DISPLAY_TILE_SIZE, ty * DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE };
            if (!test_collision(tx, ty, map, 1, r)) {
                candidats[nb_candidats].x = tx;
                candidats[nb_candidats].y = ty;
                nb_candidats++;
            }
        }
    }

    for (int s = nb_candidats - 1; s > 0; s--) {
        int k = rand() % (s + 1);
        PositionTuile tmp = candidats[s];
        candidats[s] = candidats[k];
        candidats[k] = tmp;
    }

    int idx_utilise = 0;
    int moutons_restants = nb_moutons;
    int pions_restants = nb_pawns;
    int prefere = rand() % 2;

    while ((idx_utilise < nb_candidats) && (index < MAX_MOB) && (moutons_restants + pions_restants > 0)) {
        int tuile_x = candidats[idx_utilise].x;
        int tuile_y = candidats[idx_utilise].y;
        idx_utilise++;

        int veut_mouton = 0;
        if (moutons_restants == 0) veut_mouton = 0;
        else if (pions_restants == 0) veut_mouton = 1;
        else { veut_mouton = (prefere == 0) ? 1 : 0; prefere = 1 - prefere; }

    mobs[index] = malloc(sizeof(Mob));
    mobs[index]->largeur = 1;
    mobs[index]->hauteur = 1;
    int decalage = DISPLAY_TILE_SIZE / 4;
    int decal_x = (rand() % (decalage * 2 + 1)) - decalage;
    int decal_y = (rand() % (decalage * 2 + 1)) - decalage;
    mobs[index]->x = tuile_x * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE/2) + decal_x - (mobs[index]->largeur*DISPLAY_TILE_SIZE)/2;
    mobs[index]->y = tuile_y * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE/2) + decal_y - (mobs[index]->hauteur*DISPLAY_TILE_SIZE)/2;
    mobs[index]->direction = 0;
    mobs[index]->vx = 0.0f;
    mobs[index]->vy = 0.0f;
    mobs[index]->target_vx = 0.0f;
    mobs[index]->target_vy = 0.0f;
    mobs[index]->time_change_dir = 0;
    mobs[index]->vie = 3;
    mobs[index]->drop_chance = 100;

        if (veut_mouton) {
            mobs[index]->speed = 30.0f;
            mobs[index]->texture = texture_mouton;
            mobs[index]->id = 1;
            moutons_restants--;
        } else {
            mobs[index]->speed = 70.0f;
            mobs[index]->texture = texture_pawns;
            mobs[index]->id = 2;
            pions_restants--;
        }
        index++;
    }

    int tentatives = 0;
    while ((index < MAX_MOB) && ((index < nb_moutons + nb_pawns)) && tentatives < 200) {
        tentatives++;
        int minx = tuile_joueur_x - rayon_app;
        int maxx = tuile_joueur_x + rayon_app;
        int miny = tuile_joueur_y - rayon_app;
        int maxy = tuile_joueur_y + rayon_app;
        if (minx < 0) minx = 0;
        if (miny < 0) miny = 0;
        if (maxx >= W_MAP) maxx = W_MAP - 1;
        if (maxy >= H_MAP) maxy = H_MAP - 1;
        int tx = minx + (rand() % (maxx - minx + 1));
        int ty = miny + (rand() % (maxy - miny + 1));
        SDL_Rect r = { tx * DISPLAY_TILE_SIZE, ty * DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE };
        if (test_collision(tx, ty, map, 1, r)) continue;
        int utilise = 0;
        for (int m_i = 0; m_i < index; m_i++) {
            int mx_tile = (int)((mobs[m_i]->x + mobs[m_i]->largeur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
            int my_tile = (int)((mobs[m_i]->y + mobs[m_i]->hauteur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
            if (mx_tile == tx && my_tile == ty) { utilise = 1; break; }
        }
        if (utilise) continue;

        mobs[index] = malloc(sizeof(Mob));
        mobs[index]->largeur = 1; mobs[index]->hauteur = 1;
        int decal = DISPLAY_TILE_SIZE / 4;
        int offx2 = (rand() % (decal * 2 + 1)) - decal;
        int offy2 = (rand() % (decal * 2 + 1)) - decal;
        mobs[index]->x = tx * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE/2) + offx2 - (mobs[index]->largeur*DISPLAY_TILE_SIZE)/2;
        mobs[index]->y = ty * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE/2) + offy2 - (mobs[index]->hauteur*DISPLAY_TILE_SIZE)/2;
        if (index < nb_moutons) {
            mobs[index]->speed = 30.0f; mobs[index]->texture = texture_mouton; mobs[index]->id = 1;
        } else {
            mobs[index]->speed = 70.0f; mobs[index]->texture = texture_pawns; mobs[index]->id = 2;
        }
        mobs[index]->direction = 0;
        mobs[index]->vx = 0.0f; mobs[index]->vy = 0.0f;
        mobs[index]->target_vx = 0.0f; mobs[index]->target_vy = 0.0f;
        mobs[index]->time_change_dir = 0;
        mobs[index]->vie = 3; mobs[index]->drop_chance = 100;
        index++;
    }

    if (candidats) free(candidats);
    mobs[index] = NULL;

}

/* Spawn a single mob of given type id at a random valid ground tile anywhere on the map.
   Returns 0 on success, -1 on failure. */
static int spawn_one_mob_of_type(Mob * mobs[MAX_MOB], t_tile map[W_MAP][H_MAP], int type_id) {
    /* try a number of random attempts */
    for (int attempt = 0; attempt < 200; attempt++) {
        int tx = rand() % W_MAP;
        int ty = rand() % H_MAP;
        type_t t = map[tx][ty].type;
        int est_terre = (t == terreP || t == terreCHG || t == terreCHD || t == terreCBG || t == terreCBD || t == terreH || t == terreB || t == terreG || t == terreD);
        if (!est_terre) continue;
        SDL_Rect r = { tx * DISPLAY_TILE_SIZE, ty * DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE, DISPLAY_TILE_SIZE };
        if (test_collision(tx, ty, map, 1, r)) continue;

        /* find empty slot in mobs[] */
        int idx = 0;
        while (idx < MAX_MOB && mobs[idx] != NULL) idx++;
        if (idx >= MAX_MOB) return -1;

        mobs[idx] = malloc(sizeof(Mob));
        mobs[idx]->largeur = 1; mobs[idx]->hauteur = 1;
        int jitter = DISPLAY_TILE_SIZE / 4;
        int ox = (rand() % (jitter * 2 + 1)) - jitter;
        int oy = (rand() % (jitter * 2 + 1)) - jitter;
        mobs[idx]->x = tx * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE/2) + ox - (mobs[idx]->largeur*DISPLAY_TILE_SIZE)/2;
        mobs[idx]->y = ty * DISPLAY_TILE_SIZE + (DISPLAY_TILE_SIZE/2) + oy - (mobs[idx]->hauteur*DISPLAY_TILE_SIZE)/2;
        mobs[idx]->direction = 0;
        mobs[idx]->vx = 0.0f; mobs[idx]->vy = 0.0f;
        mobs[idx]->target_vx = 0.0f; mobs[idx]->target_vy = 0.0f;
        mobs[idx]->time_change_dir = 0;
        mobs[idx]->vie = 3;
        mobs[idx]->drop_chance = 100;
        if (type_id == 1) { mobs[idx]->speed = 30.0f; mobs[idx]->texture = texture_mouton; mobs[idx]->id = 1; }
        else { mobs[idx]->speed = 70.0f; mobs[idx]->texture = texture_pawns; mobs[idx]->id = 2; }
        return 0;
    }
    return -1;
}

void update_mobs(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB]) {
    static Uint32 last_time = 0;
    Uint32 now = SDL_GetTicks();
    /* process due respawns */
    EntreeRespawn entree;
    while (pop_respawn_prevu(&entree) == 0) {
        spawn_one_mob_of_type(mobs, map, entree.id);
    }
    if (last_time == 0) last_time = now;
    float dt = (now - last_time) / 1000.0f; /* seconds */
    if (dt <= 0) dt = 0.001f;

    for (int i = 0; mobs[i] != NULL; i++) {
        Mob *m = mobs[i];
        Uint32 timer = now;

        if (m->id == 3) {
            float joueur_x = -perso.x + 520.0f;
            float joueur_y = -perso.y + 430.0f;
            float minion_x = m->x + (m->largeur * DISPLAY_TILE_SIZE * 0.5f);
            float minion_y = m->y + (m->hauteur * DISPLAY_TILE_SIZE * 0.5f);
            float dx = joueur_x - minion_x;
            float dy = joueur_y - minion_y;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist > 95.0f) {
                if (dist < 0.001f) dist = 1.0f;
                m->target_vx = (dx / dist) * m->speed;
                m->target_vy = (dy / dist) * m->speed;
            } else {
                m->target_vx = 0.0f;
                m->target_vy = 0.0f;

                if (perso.vie > 0 && perso.invincibiliter_timer == 0 && (timer - minion_attack_timer) > 850) {
                    perso.vie -= 1;
                    if (perso.vie < 0) perso.vie = 0;
                    perso.invincibiliter_timer = SDL_GetTicks();
                    minion_attack_timer = timer;
                }
            }
            m->time_change_dir = timer + 120;
        } else if (timer >= m->time_change_dir) {
            float angle = (rand() % 360) * (3.14159265f / 180.0f);
            int change_ms = (m->id == 1) ? 2000 + (rand() % 2000) : 800 + (rand() % 1200);
            m->target_vx = cosf(angle) * m->speed;
            m->target_vy = sinf(angle) * m->speed;
            m->time_change_dir = timer + change_ms;
        }

        float steer_strength = (m->id == 1) ? 200.0f : ((m->id == 3) ? 650.0f : 400.0f); /* px/s^2 */
        float ax = (m->target_vx - m->vx) * fminf(1.0f, steer_strength * dt / fmaxf(1.0f, m->speed));
        float ay = (m->target_vy - m->vy) * fminf(1.0f, steer_strength * dt / fmaxf(1.0f, m->speed));
        m->vx += ax;
        m->vy += ay;

        float vmag = sqrtf(m->vx * m->vx + m->vy * m->vy);
        if (vmag > m->speed) {
            m->vx = (m->vx / vmag) * m->speed;
            m->vy = (m->vy / vmag) * m->speed;
        }

        float nouvelle_x = m->x + m->vx * dt;
        float nouvelle_y = m->y + m->vy * dt;

        SDL_Rect proj = {
            (int)nouvelle_x,
            (int)nouvelle_y,
            (int)(m->largeur * DISPLAY_TILE_SIZE),
            (int)(m->hauteur * DISPLAY_TILE_SIZE)
        };

        int tile_x = (int)((nouvelle_x + m->largeur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);
        int tile_y = (int)((nouvelle_y + m->hauteur * DISPLAY_TILE_SIZE / 2) / DISPLAY_TILE_SIZE);

        if (tile_x >= 0 && tile_x < W_MAP && tile_y >= 0 && tile_y < H_MAP) {
            if (!test_collision(tile_x, tile_y, map, 1, proj)) {
                m->x = nouvelle_x;
                m->y = nouvelle_y;
            } else {
                if (m->id != 3) {
                    float angle = (rand() % 360) * (3.14159265f / 180.0f);
                    m->target_vx = cosf(angle) * m->speed;
                    m->target_vy = sinf(angle) * m->speed;
                } else {
                    m->target_vx = 0.0f;
                    m->target_vy = 0.0f;
                }
                m->vx *= 0.3f;
                m->vy *= 0.3f;
                m->time_change_dir = timer + 300 + (rand() % 400);
            }
        } else {
            float center_x = (W_MAP * DISPLAY_TILE_SIZE) / 2.0f;
            float center_y = (H_MAP * DISPLAY_TILE_SIZE) / 2.0f;
            float dx = center_x - m->x;
            float dy = center_y - m->y;
            float ang = atan2f(dy, dx);
            m->target_vx = cosf(ang) * m->speed;
            m->target_vy = sinf(ang) * m->speed;
            m->vx *= 0.5f;
            m->vy *= 0.5f;
            m->time_change_dir = timer + 500;
        }
    }

    last_time = now;
}

void afficher_mob(SDL_Renderer * renderer, Mob * mobs[MAX_MOB]){
    for(int i = 0; mobs[i] != NULL; i++){
        if (mobs[i]->texture == NULL) {
            SDL_Log("erreur mob : texture invalide (id=%d, index=%d)", mobs[i]->id, i);
            continue;
        }
        SDL_FRect dest = {
            .x = mobs[i]->x + perso.x,
            .y = mobs[i]->y + perso.y,
            .w = mobs[i]->largeur * DISPLAY_TILE_SIZE,
            .h = mobs[i]->hauteur * DISPLAY_TILE_SIZE
        };
        SDL_FRect src_pawn = {
            .x = 0,
            .y = 0,
            .w = 192,
            .h = 192
        };
        SDL_FRect src_mouton = {
            .x = 0,
            .y = 0,
            .w = 128,
            .h = 128
        };
        if (mobs[i]->id == 3 && (texture_minion_run != NULL || texture_minion_attack != NULL)) {
            float joueur_x = -perso.x + 520.0f;
            float joueur_y = -perso.y + 430.0f;
            float minion_x = mobs[i]->x + (mobs[i]->largeur * DISPLAY_TILE_SIZE * 0.5f);
            float minion_y = mobs[i]->y + (mobs[i]->hauteur * DISPLAY_TILE_SIZE * 0.5f);
            float dx = joueur_x - minion_x;
            float dy = joueur_y - minion_y;
            float dist = sqrtf(dx * dx + dy * dy);

            int is_attack = (dist <= 100.0f);
            SDL_Texture *tex = is_attack ? texture_minion_attack : texture_minion_run;
            int frames = is_attack ? 4 : 6;
            if (tex == NULL) {
                tex = texture_minion_run;
                frames = 6;
            }
            if (tex == NULL) {
                tex = mobs[i]->texture;
                frames = 1;
            }

            float tw = 192.0f;
            float th = 192.0f;
            SDL_GetTextureSize(tex, &tw, &th);

            int frame = (int)((SDL_GetTicks() / 110) % (Uint32)frames);
            SDL_FRect src_minion = {
                .x = (tw / (float)frames) * (float)frame,
                .y = 0,
                .w = (tw / (float)frames),
                .h = th
            };
            SDL_RenderTexture(renderer, tex, &src_minion, &dest);
        } else {
            if (mobs[i]->texture == NULL) SDL_Log("erreur mob : %s", SDL_GetError());
            SDL_RenderTexture(renderer, mobs[i]->texture, mobs[i]->texture == texture_pawns ? &src_pawn : &src_mouton, &dest);
        }
    }
}

void detruire_mobs(Mob * mobs[MAX_MOB]) {
    if (texture_pawns != NULL) {
        SDL_DestroyTexture(texture_pawns);
        texture_pawns = NULL;
    }
    if (texture_mouton != NULL) {
        SDL_DestroyTexture(texture_mouton);
        texture_mouton = NULL;
    }
    if (texture_minion_run != NULL) {
        SDL_DestroyTexture(texture_minion_run);
        texture_minion_run = NULL;
    }
    if (texture_minion_attack != NULL) {
        SDL_DestroyTexture(texture_minion_attack);
        texture_minion_attack = NULL;
    }
    for (int i = 0; mobs[i] != NULL; i++) {
        mobs[i]->texture = NULL;
    }
}


void detruire_un_mob(Mob * mob) {
    if (mob != NULL) {
        free(mob);
    }
}
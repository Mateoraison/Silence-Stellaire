/**
 * @file objectifs.c
 * @brief Gestion des objectifs de progression et de leur affichage.
 */

#include "headers/main.h"

extern int  engrenages_poses;
extern bool vaisseau_repare;
extern int  argent;
extern int  index_item;
progression_jeu_t progression_jeu;
extern t_case *hotbar[HOTBAR_SIZE];
extern boss_t boss1;
extern boss_t boss3;

static int verif_boss3_tue(void) {
    return boss3.est_battu ? 1 : 0;
}

static int verif_boss1_tue(void) {
    return boss1.est_battu ? 1 : 0;
}

static int verif_ramasser_engrenage(void) {
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        if (hotbar[i] && hotbar[i]->item && hotbar[i]->item->type == ENGRENAGE)
            return 1;
    }
    return 0;
}
 

static int verif_reparer_vaisseau(void) {
    return vaisseau_repare ? 1 : 0;
}
 

static int verif_cinq_pieces(void) {
    return (argent >= 5) ? 1 : 0;
}
 

static int verif_viande_cuite(void) {
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        if (hotbar[i] && hotbar[i]->item && hotbar[i]->item->type == VIANDECUITE)
            return 1;
    }
    return 0;
}
 
static int verif_pleine_vie(void) {
    return (perso.vie >= perso.vie_max) ? 1 : 0;
}

static int verif_simon_termine(void) {
    jeu_get_progression(&progression_jeu);
    return progression_jeu.simon_reussi ? 1 : 0;
}

static int verif_mastermind_termine(void) {
    jeu_get_progression(&progression_jeu);
    return progression_jeu.mastermind_reussi ? 1 : 0;
}



void objectifs_init(t_objectifs *obj, int planete) {
    memset(obj, 0, sizeof(*obj));
    obj->visible   = true;
    obj->anim_slot = -1;
 
#define AJOUTER(desc_,verif_) do { \
    obj->objectifs[obj->nb].description = (desc_); \
    obj->objectifs[obj->nb].valide      = false; \
    obj->objectifs[obj->nb].verifier    = (verif_);\
    obj->nb++; \
} while(0)
 
        switch (planete) {
        case 1:
            AJOUTER("Ramasser un engrenage",          verif_ramasser_engrenage);
            AJOUTER("Manger de la viande cuite",       verif_viande_cuite);
            AJOUTER("Accumuler 5 pièces",              verif_cinq_pieces);
            AJOUTER("Se soigner à pleine vie",         verif_pleine_vie);
            break;
        case 2:
            AJOUTER("Ramasser un engrenage",           verif_ramasser_engrenage);
            AJOUTER("Résoudre le puzzle 1", verif_mastermind_termine);
            AJOUTER("Résoudre le puzzle 2", verif_simon_termine);
            AJOUTER("Vaincre FireDemon",       verif_boss1_tue);
            break;
        case 3:
            AJOUTER("Trouver l'engrenage dans le labyrinthe", verif_ramasser_engrenage);
            AJOUTER("Vaincre le Minotaure",                    verif_boss3_tue);
            break;
        default:
            AJOUTER("Ramasser un engrenage",           verif_ramasser_engrenage);
            AJOUTER("Réparer le vaisseau",             verif_reparer_vaisseau);
            break;
    }
#undef AJOUTER
}




void objectif_valider(t_objectifs *obj, int index) {
    if (index < 0 || index >= obj->nb) return;
    if (obj->objectifs[index].valide) return;
    obj->objectifs[index].valide = true;
    obj->anim_slot  = index;
    obj->anim_debut = SDL_GetTicks();
}



#define PANNEAU_Y       80.0f
#define PANNEAU_W      230.0f
#define LIGNE_H         28.0f
#define PADDING         10.0f
#define ANIM_DUREE     800u  




void objectifs_afficher(t_objectifs *obj, SDL_Renderer *renderer, TTF_Font *font) {
    if (!obj->visible || !font) return;

    float panneau_x = screen_widthf() - PANNEAU_W - 20.0f;
    if (panneau_x < 10.0f) panneau_x = 10.0f;
    float panneau_y = PANNEAU_Y;
 
    Uint32 maintenant = SDL_GetTicks();

    for (int i = 0; i < obj->nb; i++) {
        if (!obj->objectifs[i].valide && obj->objectifs[i].verifier) {
            if (obj->objectifs[i].verifier()) {
                obj->objectifs[i].valide = true;
                obj->anim_slot  = i;
                obj->anim_debut = maintenant;
            }
        }
    }

 
    /* Calcul hauteur totale du panneau */
    float hauteur = PADDING * 2 + 24.0f /* titre */ + PADDING + obj->nb * LIGNE_H;
 
    /*  Assombrir l'ecran pour faire ressortir la liste des objectifs */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 200);
    SDL_FRect fond = { panneau_x, panneau_y, PANNEAU_W, hauteur };
    SDL_RenderFillRect(renderer, &fond);
 
    /* Bordure */
    SDL_SetRenderDrawColor(renderer, 80, 130, 200, 220);
    SDL_RenderRect(renderer, &fond);
 
    /* Titre */
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *s_titre = TTF_RenderText_Solid(font, "Objectifs", 0, blanc);
    if (s_titre) {
        SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s_titre);
        SDL_FRect dst = {
            panneau_x + (PANNEAU_W - s_titre->w) / 2.0f,
            panneau_y + PADDING,
            (float)s_titre->w, (float)s_titre->h
        };
        SDL_RenderTexture(renderer, t, NULL, &dst);
        SDL_DestroyTexture(t);
        SDL_DestroySurface(s_titre);
    }
 
    /* Séparateur sous le titre */
    SDL_SetRenderDrawColor(renderer, 80, 130, 200, 150);
    SDL_FRect sep = { panneau_x + PADDING, panneau_y + PADDING + 22.0f, PANNEAU_W - PADDING * 2, 1.0f };
    SDL_RenderFillRect(renderer, &sep);
 
    /*  Lignes d'objectifs  */
    float y = panneau_y + PADDING + 24.0f + PADDING;
 
    for (int i = 0; i < obj->nb; i++) {
        bool en_anim = (obj->anim_slot == i) &&
                       (maintenant - obj->anim_debut < ANIM_DUREE);
 
        /* Surbrillance de validation */
        if (en_anim) {
            float alpha_f = 1.0f - (float)(maintenant - obj->anim_debut) / (float)ANIM_DUREE;
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 50, 220, 80, (Uint8)(alpha_f * 160));
            SDL_FRect hl = { panneau_x + 2, y - 2, PANNEAU_W - 4, LIGNE_H };
            SDL_RenderFillRect(renderer, &hl);
        }
 
        /* Case à cocher */
        SDL_FRect case_rect = { panneau_x + PADDING, y + 5.0f, 14.0f, 14.0f };
        SDL_SetRenderDrawColor(renderer, 80, 130, 200, 255);
        SDL_RenderRect(renderer, &case_rect);
 
        if (obj->objectifs[i].valide) {
            /* Croix verte intérieure */
            SDL_SetRenderDrawColor(renderer, 50, 220, 80, 255);
            SDL_RenderLine(renderer,
                case_rect.x + 2, case_rect.y + 2,
                case_rect.x + 12, case_rect.y + 12);
            SDL_RenderLine(renderer,
                case_rect.x + 12, case_rect.y + 2,
                case_rect.x + 2, case_rect.y + 12);
        }
 
        /* Texte de l'objectif */
        SDL_Color couleur;
        if (obj->objectifs[i].valide) {
            couleur = (SDL_Color){100, 220, 100, 255}; /* vert validé */
        } else {
            couleur = (SDL_Color){200, 200, 200, 255}; /* gris en attente */
        }
 
        SDL_Surface *s = TTF_RenderText_Solid(font, obj->objectifs[i].description, 0, couleur);
        if (s) {
            float max_w = PANNEAU_W - PADDING * 2 - 20.0f;
            float draw_w = (s->w > max_w) ? max_w : (float)s->w;
            SDL_FRect dst_txt = { panneau_x + PADDING + 20.0f, y + 2.0f, draw_w, (float)s->h };
 
            SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_FRect full_src = { 0, 0, (float)s->w, (float)s->h };
            SDL_RenderTexture(renderer, t, &full_src, &dst_txt);
            SDL_DestroyTexture(t);
            SDL_DestroySurface(s);
        }
 
        /* Trait de barrage sur texte validé */
        if (obj->objectifs[i].valide) {
            SDL_SetRenderDrawColor(renderer, 80, 180, 80, 200);
            float mid_y = y + LIGNE_H / 2.5f;
            SDL_RenderLine(renderer,
                panneau_x + PADDING + 20.0f, mid_y,
                panneau_x + PANNEAU_W - PADDING, mid_y);
        }
 
        y += LIGNE_H;
    }
 
    /* Compteur de progression en bas */
    int valides = 0;
    for (int i = 0; i < obj->nb; i++) if (obj->objectifs[i].valide) valides++;
 
    char prog[32];
    SDL_snprintf(prog, sizeof(prog), "%d / %d complétés", valides, obj->nb);
    SDL_Color gris = {140, 150, 170, 255};
    SDL_Surface *s_prog = TTF_RenderText_Solid(font, prog, 0, gris);
    if (s_prog) {
        SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s_prog);
        SDL_FRect dst = {
            panneau_x + (PANNEAU_W - s_prog->w) / 2.0f,
            panneau_y + hauteur - s_prog->h - PADDING / 2.0f,
            (float)s_prog->w, (float)s_prog->h
        };
        SDL_RenderTexture(renderer, t, NULL, &dst);
        SDL_DestroyTexture(t);
        SDL_DestroySurface(s_prog);
    }
}

void objectifs_visible(t_objectifs *obj) {
    obj->visible = !obj->visible;
}



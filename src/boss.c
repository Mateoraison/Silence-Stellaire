#include "headers/main.h"

/**
 * Initialise le boss avec ses caractéristiques.
 * Le boss est positionné à (x, y) avec un maximum de vie et une attaque spécifiés.
 * Il est statique et nous envoie des projectiles que le joueur doit renvoyer pour faire des dégâts.
 * Le boss a plusieurs phases, chacune avec des comportements et des attaques différents.
 * Phase 1 : Attaque de base 
 * Phase 2 : Attaque de base + Attaque de zone
 * Phase 3 : Attaque de base + Attaque de zone + Invocation de minions
 * Il drops un engrenage à la fin du combat.
 */

void init_boss(SDL_Renderer *renderer, boss_t *boss, float x, float y, int vie_max, int attaque) {
    boss->x = x;
    boss->y = y;
    boss->vie = vie_max;
    boss->vie_max = vie_max;
    boss->attaque = attaque;
    boss->cooldown_attaque = 0;
    boss->phase = 1;
    boss->est_battu = 0;
    boss->animation_frame_idle = 0;
    boss->animation_frame_attack = 0;
    boss->animation_frame_death = 0;
    boss->animation_state = 0;
    boss->animation_timer = SDL_GetTicks();
    boss->texture = IMG_LoadTexture(renderer, "assets/personnage/boss/boss.png");
}

void afficher_boss(SDL_Renderer *renderer, boss_t *boss) {
    SDL_FRect dstrect = { (boss->x)+perso.x,(boss->y)+perso.y, 576.0f, 320.0f };
    if(boss->animation_state == 0) {
        SDL_FRect srcrect = { boss->animation_frame_idle * 288, 0, 288, 160 };
        SDL_RenderTexture(renderer, boss->texture, &srcrect, &dstrect);
    } else if(boss->animation_state == 1) {
        SDL_FRect srcrect = { boss->animation_frame_attack * 288, 320, 288, 160 };
        SDL_RenderTexture(renderer, boss->texture, &srcrect, &dstrect);
    } else if(boss->animation_state == 2) {
        SDL_FRect srcrect = { boss->animation_frame_death * 288, 640, 288, 160 };
        SDL_RenderTexture(renderer, boss->texture, &srcrect, &dstrect);
    }
}

void update_boss(boss_t *boss) {
    if (boss->est_battu) {
        return;
    }

    boss->cooldown_attaque++;
    if (boss->cooldown_attaque >= 60) {
        boss->cooldown_attaque = 0;
    }
}

void boss_attaque(SDL_Renderer *renderer, boss_t *boss) {
    if (boss->est_battu || boss->cooldown_attaque > 0) {
        return;
    }

    SDL_Texture *attaque_texture = IMG_LoadTexture(renderer, "assets/personnage/boss/attaque.png");

    // Phase 1: Attaque de base - projectile simple
    if (boss->phase >= 1) {
        // Attaque vers le joueur
        // À implémenter avec le système de projectiles du jeu
    }

    // Phase 2: Attaque de base + Attaque de zone
    if (boss->phase >= 2) {
        // Attaque en zone
        // À implémenter
    }

    // Phase 3: Attaque de base + Attaque de zone + Invocation de minions
    if (boss->phase >= 3) {
        // Invocation de minions
        // À implémenter
    }

    // Réinitialiser le cooldown
    boss->cooldown_attaque = 60;
    SDL_DestroyTexture(attaque_texture);
}

void Destroy_boss(boss_t *boss) {
    if (boss->texture) {
        SDL_DestroyTexture(boss->texture);
        boss->texture = NULL;
    }
}

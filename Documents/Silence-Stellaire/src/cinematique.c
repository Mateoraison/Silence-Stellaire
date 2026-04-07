#include "headers/main.h"
#include "headers/cinematique.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

static void rendre_texture_centre(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, float w, float h) {
    if (!texture) return;
    SDL_FRect dst = {x - w * 0.5f, y - h * 0.5f, w, h};
    SDL_RenderTexture(renderer, texture, NULL, &dst);
}

static void rendre_texture_centre_natif(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y) {
    if (!texture) return;
    float tw = 0.0f, th = 0.0f;
    SDL_GetTextureSize(texture, &tw, &th);
    SDL_FRect dst = {x - tw * 0.5f, y - th * 0.5f, tw, th};
    SDL_RenderTexture(renderer, texture, NULL, &dst);
}

static void detruire_texture_si_besoin(SDL_Texture **texture) {
    if (texture && *texture) {
        SDL_DestroyTexture(*texture);
        *texture = NULL;
    }
}

static float borner01(float valeur) {
    if (valeur < 0.0f) return 0.0f;
    if (valeur > 1.0f) return 1.0f;
    return valeur;
}

static float acceleration_decroissance(float valeur) {
    valeur = borner01(valeur);
    return valeur * valeur * (3.0f - 2.0f * valeur);
}


int jouer_cinematique_intro(SDL_Renderer *renderer) {
    SDL_Texture *fond_ecran = IMG_LoadTexture(renderer, "assets/menu/menu.png");
    SDL_Texture *logo = IMG_LoadTexture(renderer, "assets/logo_win.png");
    TTF_Font *police_titre = TTF_OpenFont("assets/police_titre.ttf", 36);
    TTF_Font *police_info = TTF_OpenFont("assets/police.ttf", 20);

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color gris = {190, 190, 190, 255};

    SDL_Texture *texture_titre = NULL;
    SDL_Texture *texture_info = NULL;
    SDL_Texture *texture_passage = NULL;


    if (police_titre) {
        SDL_Surface *surface = TTF_RenderText_Solid(police_titre, "Silence Stellaire", 16, blanc);
        if (surface) {
            texture_titre = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }
    }

    if (police_info) {
        SDL_Surface *surface = TTF_RenderText_Solid(police_info, "Un voyage commence dans le silence de l'espace...", 50, gris);
        if (surface) {
            texture_info = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }

        surface = TTF_RenderText_Solid(police_info, "Espace / Entree / Echap pour passer", 35, gris);
        if (surface) {
            texture_passage = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }
    }

    Uint32 debut_ms = SDL_GetTicks();
    const Uint32 duree_ms = 7000;

    while (1) {
        SDL_Event evenement;
        while (SDL_PollEvent(&evenement)) {
            if (evenement.type == SDL_EVENT_QUIT) {
                if (fond_ecran) SDL_DestroyTexture(fond_ecran);
                if (logo) SDL_DestroyTexture(logo);
                if (texture_titre) SDL_DestroyTexture(texture_titre);
                if (texture_info) SDL_DestroyTexture(texture_info);
                if (texture_passage) SDL_DestroyTexture(texture_passage);
                if (police_titre) TTF_CloseFont(police_titre);
                if (police_info) TTF_CloseFont(police_info);
                return 1;
            }
            if (evenement.type == SDL_EVENT_KEY_DOWN) {
                if (evenement.key.key == SDLK_ESCAPE || evenement.key.key == SDLK_SPACE || evenement.key.key == SDLK_RETURN) {
                    if (fond_ecran) SDL_DestroyTexture(fond_ecran);
                    if (logo) SDL_DestroyTexture(logo);
                    if (texture_titre) SDL_DestroyTexture(texture_titre);
                    if (texture_info) SDL_DestroyTexture(texture_info);
                    if (texture_passage) SDL_DestroyTexture(texture_passage);
                    if (police_titre) TTF_CloseFont(police_titre);
                    if (police_info) TTF_CloseFont(police_info);
                    return 0;
                }
            }
            if (evenement.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (fond_ecran) SDL_DestroyTexture(fond_ecran);
                if (logo) SDL_DestroyTexture(logo);
                if (texture_titre) SDL_DestroyTexture(texture_titre);
                if (texture_info) SDL_DestroyTexture(texture_info);
                if (texture_passage) SDL_DestroyTexture(texture_passage);
                if (police_titre) TTF_CloseFont(police_titre);
                if (police_info) TTF_CloseFont(police_info);
                return 0;
            }
        }

        Uint32 temps_ecoule = SDL_GetTicks() - debut_ms;
        if (temps_ecoule >= duree_ms) break;

        float phase = (float)temps_ecoule / (float)duree_ms;
        float multiplicateur_alpha = 1.0f;
        if (phase < 0.15f) multiplicateur_alpha = phase / 0.15f;
        else if (phase > 0.85f) multiplicateur_alpha = (1.0f - phase) / 0.15f;
        if (multiplicateur_alpha < 0.0f) multiplicateur_alpha = 0.0f;
        if (multiplicateur_alpha > 1.0f) multiplicateur_alpha = 1.0f;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (fond_ecran) {
            SDL_SetTextureAlphaMod(fond_ecran, 255);
            SDL_RenderTexture(renderer, fond_ecran, NULL, NULL);
        }

        if (logo) {
            Uint8 alpha = (Uint8)(255.0f * multiplicateur_alpha);
            SDL_SetTextureAlphaMod(logo, alpha);
            rendre_texture_centre(renderer, logo, screen_center_x(), screen_center_y() - 100.0f, 260.0f, 260.0f);
        }

        if (texture_titre) {
            Uint8 alpha = (Uint8)(255.0f * multiplicateur_alpha);
            SDL_SetTextureAlphaMod(texture_titre, alpha);
            rendre_texture_centre_natif(renderer, texture_titre, screen_center_x(), screen_center_y() + 120.0f);
        }

        if (texture_info && temps_ecoule > 1600) {
            Uint8 alpha = (Uint8)(220.0f * multiplicateur_alpha);
            SDL_SetTextureAlphaMod(texture_info, alpha);
            rendre_texture_centre_natif(renderer, texture_info, screen_center_x(), screen_center_y() + 175.0f);
        }

        if (texture_passage) {
            Uint8 alpha = (Uint8)(150.0f * multiplicateur_alpha);
            SDL_SetTextureAlphaMod(texture_passage, alpha);
            rendre_texture_centre_natif(renderer, texture_passage, screen_center_x(), screen_heightf() - 60.0f);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (fond_ecran) SDL_DestroyTexture(fond_ecran);
    if (logo) SDL_DestroyTexture(logo);
    if (texture_titre) SDL_DestroyTexture(texture_titre);
    if (texture_info) SDL_DestroyTexture(texture_info);
    if (texture_passage) SDL_DestroyTexture(texture_passage);
    if (police_titre) TTF_CloseFont(police_titre);
    if (police_info) TTF_CloseFont(police_info);
    return 0;
}

int jouer_cinematique_crash(SDL_Renderer *renderer) {
    int sfx = 1; 

    TTF_Font *police_cinematique = TTF_OpenFont("assets/police.ttf", 40);
    SDL_Texture *fond_map = IMG_LoadTexture(renderer, "assets/carte_espace/fond_map.png");
    SDL_Texture *vaisseau_integre = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/vaisseau_non_casser.png");
    SDL_Texture *vaisseau_casse = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/vaisseau_casser.png");
    SDL_Texture *asteroides[4] = {0};
    asteroides[0] = IMG_LoadTexture(renderer, "assets/carte_espace/asteroid1.png");
    asteroides[1] = IMG_LoadTexture(renderer, "assets/carte_espace/asteroid2.png");
    asteroides[2] = IMG_LoadTexture(renderer, "assets/carte_espace/asteroid3.png");
    asteroides[3] = IMG_LoadTexture(renderer, "assets/carte_espace/asteroid4.png");

    if (vaisseau_integre) SDL_SetTextureScaleMode(vaisseau_integre, SDL_SCALEMODE_NEAREST);
    if (fond_map) SDL_SetTextureScaleMode(fond_map, SDL_SCALEMODE_NEAREST);
    if (vaisseau_casse) SDL_SetTextureScaleMode(vaisseau_casse, SDL_SCALEMODE_NEAREST);
    for (int i = 0; i < 4; i++) {
        if (asteroides[i]) SDL_SetTextureScaleMode(asteroides[i], SDL_SCALEMODE_NEAREST);
    }

    SDL_Texture *texte_depart = NULL;
    SDL_Texture *texte_impact = NULL;
    SDL_Texture *texte_crash = NULL;

    if (police_cinematique) {
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Color orange = {255, 180, 90, 255};
        SDL_Color rouge = {255, 90, 70, 255};

        SDL_Surface *surface = TTF_RenderText_Solid(police_cinematique, "Cap vers la maison...", 20, blanc);
        if (surface) {
            texte_depart = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }

        surface = TTF_RenderText_Solid(police_cinematique, "Alerte... impact detecte.", 25, orange);
        if (surface) {
            texte_impact = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }

        surface = TTF_RenderText_Solid(police_cinematique, "Crash du vaisseau...", 21, rouge);
        if (surface) {
            texte_crash = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }
    }

    Uint32 debut_ms = SDL_GetTicks();
    const Uint32 duree_ms = 6200;

    while (1) {
        SDL_Event evenement;
        while (SDL_PollEvent(&evenement)) {
            if (evenement.type == SDL_EVENT_QUIT) {
                detruire_texture_si_besoin(&fond_map);
                detruire_texture_si_besoin(&vaisseau_integre);
                detruire_texture_si_besoin(&vaisseau_casse);
                for (int i = 0; i < 4; i++) detruire_texture_si_besoin(&asteroides[i]);
                detruire_texture_si_besoin(&texte_depart);
                detruire_texture_si_besoin(&texte_impact);
                detruire_texture_si_besoin(&texte_crash);
                if (police_cinematique) TTF_CloseFont(police_cinematique);
                return 1;
            }
        }

        Uint32 temps_ecoule = SDL_GetTicks() - debut_ms;
        if (temps_ecoule >= duree_ms) break;

        float phase = (float)temps_ecoule / (float)duree_ms;
        float largeur_ecran = screen_widthf();
        float hauteur_ecran = screen_heightf();
        float centre_x = screen_center_x();
        float centre_y = screen_center_y();
        float intensite_tremblement = 0.0f;
        if (phase >= 0.62f) {
            intensite_tremblement = 22.0f * acceleration_decroissance((phase - 0.62f) / 0.18f);
            if (intensite_tremblement < 0.0f) intensite_tremblement = 0.0f;
        }
        float decalage_x = sinf((float)temps_ecoule * 0.055f) * intensite_tremblement + sinf((float)temps_ecoule * 0.13f) * (intensite_tremblement * 0.45f);
        float decalage_y = cosf((float)temps_ecoule * 0.061f) * intensite_tremblement + cosf((float)temps_ecoule * 0.11f) * (intensite_tremblement * 0.35f);

        float largeur_vaisseau = 340.0f;
        float hauteur_vaisseau = 218.0f;
        float position_vaisseau_x = -largeur_vaisseau;
        float position_vaisseau_y = centre_y - 120.0f;
        float angle_vaisseau = 0.0f;

        if (phase < 0.34f) {
            float p = phase / 0.34f;
            position_vaisseau_x = -largeur_vaisseau + (centre_x - 190.0f + largeur_vaisseau) * acceleration_decroissance(p);
            position_vaisseau_y = centre_y - 130.0f + 22.0f * sinf(p * 7.0f);
            angle_vaisseau = -6.0f + 4.5f * p;
        } else if (phase < 0.62f) {
            float p = (phase - 0.34f) / 0.28f;
            position_vaisseau_x = centre_x - 190.0f + 120.0f * acceleration_decroissance(p);
            position_vaisseau_y = centre_y - 130.0f + 18.0f * sinf((phase - 0.34f) * 24.0f) + 4.0f * sinf((phase - 0.34f) * 56.0f);
            angle_vaisseau = -1.0f + 10.0f * sinf((phase - 0.34f) * 10.0f);
        } else {
            float p = (phase - 0.62f) / 0.38f;
            position_vaisseau_x = centre_x - 165.0f + 260.0f * p + decalage_x * 0.4f;
            position_vaisseau_y = centre_y - 140.0f + 220.0f * p * p + decalage_y * 0.4f;
            angle_vaisseau = 16.0f + 118.0f * p;
        }

        float largeur_asteroide = 170.0f;
        float hauteur_asteroide = 170.0f;
        float position_asteroide_x = largeur_ecran + largeur_asteroide;
        float position_asteroide_y = centre_y - 220.0f;
        float angle_asteroide = 0.0f;

        if (phase < 0.32f) {
            float p = phase / 0.32f;
            position_asteroide_x = largeur_ecran + largeur_asteroide - (centre_x + largeur_asteroide + 80.0f) * acceleration_decroissance(p);
            position_asteroide_y = centre_y - 240.0f + 120.0f * p;
            angle_asteroide = 30.0f + 85.0f * p;
        } else if (phase < 0.62f) {
            float p = (phase - 0.32f) / 0.30f;
            position_asteroide_x = centre_x + 250.0f - 300.0f * acceleration_decroissance(p);
            position_asteroide_y = centre_y - 145.0f + 150.0f * p;
            angle_asteroide = 110.0f + 180.0f * p;
        } else {
            float p = (phase - 0.62f) / 0.38f;
            position_asteroide_x = centre_x - 60.0f + 120.0f * p + decalage_x * 0.25f;
            position_asteroide_y = centre_y - 60.0f + 280.0f * p * p + decalage_y * 0.25f;
            angle_asteroide = 250.0f + 320.0f * p;
        }

        float phase_impact = 0.0f;
        if (phase >= 0.60f && phase <= 0.82f) {
            phase_impact = acceleration_decroissance((phase - 0.60f) / 0.22f);
        }

        float alpha_flash = 0.0f;
        if (phase >= 0.60f) {
            float p = (phase - 0.60f) / 0.14f;
            p = borner01(p);
            alpha_flash = (p < 0.5f) ? (p * 2.0f) : ((1.0f - p) * 2.0f);
        }

        float alpha_fondu = 0.0f;
        if (phase > 0.74f) {
            float p = (phase - 0.74f) / 0.26f;
            alpha_fondu = borner01(p);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (fond_map) {
            SDL_RenderTexture(renderer, fond_map, NULL, NULL);
        } else {
            SDL_SetRenderDrawColor(renderer, 5, 10, 22, 255);
            SDL_FRect fond_etoiles = {0.0f, 0.0f, largeur_ecran, hauteur_ecran};
            SDL_RenderFillRect(renderer, &fond_etoiles);
        }

        if (vaisseau_integre) {
            Uint8 alpha_vaisseau = (phase < 0.60f) ? 255 : (Uint8)(255.0f * (1.0f - phase_impact));
            SDL_SetTextureAlphaMod(vaisseau_integre, alpha_vaisseau);
            SDL_FRect dst = {position_vaisseau_x + decalage_x, position_vaisseau_y + decalage_y, largeur_vaisseau, hauteur_vaisseau};
            SDL_FPoint centre = {largeur_vaisseau * 0.5f, hauteur_vaisseau * 0.5f};
            SDL_RenderTextureRotated(renderer, vaisseau_integre, NULL, &dst, angle_vaisseau, &centre, SDL_FLIP_NONE);
        }

        if (vaisseau_casse && phase >= 0.58f) {
            Uint8 alpha_vaisseau = (phase < 0.66f) ? (Uint8)(255.0f * (phase - 0.58f) / 0.08f) : 255;
            SDL_SetTextureAlphaMod(vaisseau_casse, alpha_vaisseau);
            SDL_FRect dst = {position_vaisseau_x + decalage_x - 12.0f, position_vaisseau_y + decalage_y + 10.0f, largeur_vaisseau, hauteur_vaisseau};
            SDL_FPoint centre = {largeur_vaisseau * 0.5f, hauteur_vaisseau * 0.5f};
            SDL_RenderTextureRotated(renderer, vaisseau_casse, NULL, &dst, angle_vaisseau + 15.0f + phase_impact * 35.0f, &centre, SDL_FLIP_NONE);
        }

        SDL_Texture *asteroide_selectionne = NULL;
        if (phase < 0.33f) asteroide_selectionne = asteroides[0] ? asteroides[0] : asteroides[1];
        else if (phase < 0.66f) asteroide_selectionne = asteroides[2] ? asteroides[2] : asteroides[1];
        else asteroide_selectionne = asteroides[3] ? asteroides[3] : asteroides[2];

        if (asteroide_selectionne) {
            Uint8 alpha_asteroide = (phase < 0.72f) ? 255 : (Uint8)(255.0f * (1.0f - (phase - 0.72f) / 0.18f));
            SDL_SetTextureAlphaMod(asteroide_selectionne, alpha_asteroide);
            SDL_FRect dst = {position_asteroide_x - decalage_x * 0.4f, position_asteroide_y + decalage_y * 0.3f, largeur_asteroide, hauteur_asteroide};
            SDL_FPoint centre = {largeur_asteroide * 0.5f, hauteur_asteroide * 0.5f};
            SDL_RenderTextureRotated(renderer, asteroide_selectionne, NULL, &dst, angle_asteroide, &centre, SDL_FLIP_NONE);
        }

        if (alpha_flash > 0.0f) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 248, 235, (Uint8)(255.0f * alpha_flash));
            SDL_FRect rectangle_flash = {0.0f, 0.0f, largeur_ecran, hauteur_ecran};
            SDL_RenderFillRect(renderer, &rectangle_flash);
        }

        if (phase >= 0.60f) {
            float explosion = phase_impact;
            float alpha_explosion = (Uint8)(210.0f * (1.0f - alpha_fondu) * explosion);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 170, 80, (Uint8)alpha_explosion);
            float rayon_anneau = 18.0f + explosion * 165.0f;
            for (float dy = -rayon_anneau; dy <= rayon_anneau; dy += 2.0f) {
                float dx = sqrtf(rayon_anneau * rayon_anneau - dy * dy);
                SDL_RenderLine(renderer,
                               centre_x - dx,
                               centre_y + dy,
                               centre_x + dx,
                               centre_y + dy);
            }

            SDL_SetRenderDrawColor(renderer, 255, 110, 40, (Uint8)(150.0f * alpha_explosion / 210.0f));
            for (int i = 0; i < 8; i++) {
                float angle_fragment = (float)i * 0.78539816f + (float)temps_ecoule * 0.01f;
                float longueur = 90.0f + explosion * 260.0f;
                SDL_RenderLine(renderer,
                               centre_x + cosf(angle_fragment) * 16.0f,
                               centre_y + sinf(angle_fragment) * 16.0f,
                               centre_x + cosf(angle_fragment) * longueur,
                               centre_y + sinf(angle_fragment) * longueur);
            }
        }

        if (phase >= 0.62f) {
            float phase_debris = borner01((phase - 0.62f) / 0.38f);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 220, 220, 220, (Uint8)(170.0f * (1.0f - phase_debris)));
            for (int i = 0; i < 5; i++) {
                float direction = (float)i - 2.0f;
                float fragment_x = centre_x - 30.0f + direction * 22.0f + direction * phase_debris * 120.0f + decalage_x * 0.15f;
                float fragment_y = centre_y - 10.0f + direction * 8.0f + phase_debris * phase_debris * 170.0f + decalage_y * 0.15f;
                SDL_FRect fragment = {fragment_x, fragment_y, 8.0f + i * 2.0f, 8.0f + i * 2.0f};
                SDL_RenderFillRect(renderer, &fragment);
            }
        }

        if (phase > 0.62f) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(220.0f * alpha_fondu));
            SDL_FRect rectangle_fondu = {0.0f, 0.0f, largeur_ecran, hauteur_ecran};
            SDL_RenderFillRect(renderer, &rectangle_fondu);
        }

        if (texte_depart && phase < 0.30f) {
            Uint8 alpha = (Uint8)(255.0f * borner01(phase / 0.12f));
            SDL_SetTextureAlphaMod(texte_depart, alpha);
            rendre_texture_centre_natif(renderer, texte_depart, centre_x, hauteur_ecran * 0.18f);
        }

        if (texte_impact && phase >= 0.30f && phase < 0.62f) {
            Uint8 alpha = (Uint8)(255.0f * borner01((phase - 0.30f) / 0.12f));
            SDL_SetTextureAlphaMod(texte_impact, alpha);
            rendre_texture_centre_natif(renderer, texte_impact, centre_x, hauteur_ecran * 0.18f);
        }

        if (texte_crash && phase >= 0.58f) {
            Uint8 alpha = (Uint8)(255.0f * borner01((phase - 0.58f) / 0.10f));
            SDL_SetTextureAlphaMod(texte_crash, alpha);
            rendre_texture_centre_natif(renderer, texte_crash, centre_x, hauteur_ecran * 0.18f);
            if (sfx){
                jouer_son("assets/audio/crash.mp3", 0.2f);
                sfx = 0;
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    detruire_texture_si_besoin(&fond_map);
    detruire_texture_si_besoin(&vaisseau_integre);
    detruire_texture_si_besoin(&vaisseau_casse);
    for (int i = 0; i < 4; i++) detruire_texture_si_besoin(&asteroides[i]);
    detruire_texture_si_besoin(&texte_depart);
    detruire_texture_si_besoin(&texte_impact);
    detruire_texture_si_besoin(&texte_crash);
    if (police_cinematique) TTF_CloseFont(police_cinematique);
    return 0;
}

int jouer_cinematique_fin(SDL_Renderer *renderer) {
    // Ressources
    TTF_Font *police_titre = TTF_OpenFont("assets/police_titre.ttf", 56);
    TTF_Font *police_msg = TTF_OpenFont("assets/police.ttf", 22);
    SDL_Texture *fond = IMG_LoadTexture(renderer, "assets/carte_espace/fond_map.png");
    SDL_Texture *vaisseau = IMG_LoadTexture(renderer, "assets/tileset/V2/EXT_vaisseau/vaisseau_non_casser.png");

    // Texte final
    SDL_Texture *txt_title = NULL;
    SDL_Texture *txt_sub = NULL;
    if (police_titre) {
        SDL_Color col = {255, 230, 200, 255};
        SDL_Surface *s = TTF_RenderText_Solid(police_titre, "Fin du voyage", strlen("Fin du voyage"), col);
        if (s) { txt_title = SDL_CreateTextureFromSurface(renderer, s); SDL_DestroySurface(s); }
    }
    if (police_msg) {
        SDL_Color col = {200, 220, 255, 255};
        SDL_Surface *s = TTF_RenderText_Solid(police_msg, "Le vaisseau traverse la galaxie...", strlen("Le vaisseau traverse la galaxie..."), col);
        if (s) { txt_sub = SDL_CreateTextureFromSurface(renderer, s); SDL_DestroySurface(s); }
    }

    // Particules simples pour la traînée
    #define MAX_PART 80
    typedef struct { float x,y,vx,vy,life; int layer; } Part; // layer: 0=behind, 1=front (draw over ship)
    Part parts[MAX_PART];
    for (int i=0;i<MAX_PART;i++) parts[i].life = 0.0f;

    Uint32 debut = SDL_GetTicks();
    const Uint32 duree_traverse = 6500;
    const Uint32 duree_fade = 1400;
    int sfx_played = 0;

    while (1) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                // cleanup
                if (fond) SDL_DestroyTexture(fond);
                if (vaisseau) SDL_DestroyTexture(vaisseau);
                if (txt_title) SDL_DestroyTexture(txt_title);
                if (txt_sub) SDL_DestroyTexture(txt_sub);
                if (police_titre) TTF_CloseFont(police_titre);
                if (police_msg) TTF_CloseFont(police_msg);
                return 1;
            }
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN || ev.type == SDL_EVENT_KEY_DOWN) {
                // skip cinematic
                if (fond) SDL_DestroyTexture(fond);
                if (vaisseau) SDL_DestroyTexture(vaisseau);
                if (txt_title) SDL_DestroyTexture(txt_title);
                if (txt_sub) SDL_DestroyTexture(txt_sub);
                if (police_titre) TTF_CloseFont(police_titre);
                if (police_msg) TTF_CloseFont(police_msg);
                return 0;
            }
        }

        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - debut;

        // Phase 1: traversée
        if (elapsed <= duree_traverse) {
            float t = (float)elapsed / (float)duree_traverse; // 0..1
            // position vaisseau : de gauche vers droite, légère sin pour la courbe
            float sw = screen_widthf();
            float sh = screen_heightf();
            float ship_w = 340.0f, ship_h = 218.0f;
            float x = -ship_w + (sw + ship_w) * t;
            float y = screen_center_y() - 40.0f + sinf(t * 3.14159f * 1.8f) * 80.0f - t * 40.0f;
            float angle = (sinf(t * 3.14159f * 2.0f) * 18.0f) + t * 40.0f;

            // alpha du vaisseau pour fondu aux bords (évite coupures visibles)
            float alpha_ship = 1.0f;
            float fade_margin = ship_w * 0.25f; // marge pour fondu
            if (x < -fade_margin) alpha_ship = 0.0f;
            else if (x < 0.0f) alpha_ship = (x + fade_margin) / fade_margin;
            else if (x + ship_w > sw + fade_margin) alpha_ship = 0.0f;
            else if (x + ship_w > sw) alpha_ship = (sw + fade_margin - (x + ship_w)) / fade_margin;
            alpha_ship = borner01(alpha_ship);

            // Emission particules depuis l'arrière du vaisseau seulement si vaisseau visible
                    if (alpha_ship > 0.25f) {
                        for (int i=0;i<MAX_PART;i++) {
                            if (parts[i].life <= 0.0f) {
                                parts[i].life = 0.6f + ((float)(rand() % 100) / 200.0f);
                                float behind_x = x - 24.0f;
                                float behind_y = y + ship_h * 0.5f - 20.0f + ((rand()%40)-20);
                                parts[i].x = behind_x;
                                parts[i].y = behind_y;
                                parts[i].vx = -60.0f - (rand()%80);
                                parts[i].vy = ((rand()%60)-30) * 0.3f;
                                // random layer: most behind, some front
                                parts[i].layer = (rand() % 100) < 30 ? 1 : 0;
                                break;
                            }
                        }
                    }

            // update particles
            for (int i=0;i<MAX_PART;i++) {
                if (parts[i].life > 0.0f) {
                    float dt = 0.016f;
                    parts[i].x += parts[i].vx * dt;
                    parts[i].y += parts[i].vy * dt;
                    parts[i].life -= dt;
                }
            }

            // render background
            SDL_SetRenderDrawColor(renderer, 0,0,8,255);
            SDL_RenderClear(renderer);
            if (fond) SDL_RenderTexture(renderer, fond, NULL, NULL);

            // draw particles behind ship; only draw if inside screen bounds and layer==0
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            for (int i=0;i<MAX_PART;i++) {
                if (parts[i].life > 0.0f && parts[i].layer == 0) {
                    if (parts[i].x < -8.0f || parts[i].x > sw + 8.0f) continue;
                    if (parts[i].y < -8.0f || parts[i].y > sh + 8.0f) continue;
                    float life = parts[i].life / 1.0f;
                    Uint8 alpha = (Uint8)(255.0f * borner01(life) * alpha_ship);
                    SDL_SetRenderDrawColor(renderer, 255, 180, 90, alpha);
                    SDL_FRect r = { parts[i].x, parts[i].y, 4.0f, 4.0f };
                    SDL_RenderFillRect(renderer, &r);
                }
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            // draw vaisseau with alpha
            if (vaisseau && alpha_ship > 0.0f) {
                SDL_SetTextureAlphaMod(vaisseau, (Uint8)(255.0f * alpha_ship));
                SDL_FRect dst = { x, y, ship_w, ship_h };
                SDL_FPoint centre = { ship_w * 0.5f, ship_h * 0.5f };
                SDL_RenderTextureRotated(renderer, vaisseau, NULL, &dst, angle, &centre, SDL_FLIP_NONE);
                SDL_SetTextureAlphaMod(vaisseau, 255);
            }

            // draw particles in front of ship (layer==1)
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            for (int i=0;i<MAX_PART;i++) {
                if (parts[i].life > 0.0f && parts[i].layer == 1) {
                    if (parts[i].x < -8.0f || parts[i].x > sw + 8.0f) continue;
                    if (parts[i].y < -8.0f || parts[i].y > sh + 8.0f) continue;
                    float life = parts[i].life / 1.0f;
                    Uint8 alpha = (Uint8)(255.0f * borner01(life) * alpha_ship);
                    SDL_SetRenderDrawColor(renderer, 255, 200, 120, alpha);
                    // draw slightly larger for front
                    SDL_FRect r = { parts[i].x, parts[i].y, 6.0f, 6.0f };
                    SDL_RenderFillRect(renderer, &r);
                }
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            

            // small overlay text
            if (txt_sub) {
                SDL_SetTextureAlphaMod(txt_sub, (Uint8)(220.0f));
                rendre_texture_centre_natif(renderer, txt_sub, screen_center_x(), 60.0f);
            }

            if (!sfx_played) {
                jouer_son("assets/audio/ambiance.wav", 0.25f);
                sfx_played = 1;
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }

        // Phase 2: fondu et affiche message final
        Uint32 after = elapsed - duree_traverse;
        float tf = (float)after / (float)duree_fade; // 0..1
        if (tf > 1.0f) tf = 1.0f;

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);
        if (fond) SDL_RenderTexture(renderer, fond, NULL, NULL);

        // fondu sombre
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(200.0f * tf));
        SDL_FRect rect = {0,0, screen_widthf(), screen_heightf()};
        SDL_RenderFillRect(renderer, &rect);

        // affiche texte final centré
        if (txt_title) {
            SDL_SetTextureAlphaMod(txt_title, (Uint8)(255.0f * tf));
            rendre_texture_centre_natif(renderer, txt_title, screen_center_x(), screen_center_y() - 20.0f);
        }
        if (txt_sub) {
            SDL_SetTextureAlphaMod(txt_sub, (Uint8)(200.0f * tf));
            rendre_texture_centre_natif(renderer, txt_sub, screen_center_x(), screen_center_y() + 48.0f);
        }

        if (tf >= 1.0f) {
            // jouer son de victoire une fois
            static int win_played = 0;
            if (!win_played) { jouer_son("assets/audio/win.mp3", 0.5f); win_played = 1; }
        }

        SDL_RenderPresent(renderer);
        if (after >= duree_fade + 2200) break; // laisser le message quelques instants
        SDL_Delay(16);
    }

    // cleanup
    if (fond) SDL_DestroyTexture(fond);
    if (vaisseau) SDL_DestroyTexture(vaisseau);
    if (txt_title) SDL_DestroyTexture(txt_title);
    if (txt_sub) SDL_DestroyTexture(txt_sub);
    if (police_titre) TTF_CloseFont(police_titre);
    if (police_msg) TTF_CloseFont(police_msg);
    return 0;
}

int jouer_credits(SDL_Renderer *renderer) {
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 22);
    if (!font) return 0;

    const char *lines[] = {
        "Merci d'avoir joue",
        "",
        "Developpeurs:",
        " - Raison Matéo",
        " - Donné Joshua",
        " - Leroux Maxime",
        "",
        "Musique: Musicien",
        "Sons: Effets sonores",
        "",
        "Retour au menu dans quelques instants...",
    };
    const int n = sizeof(lines)/sizeof(lines[0]);

    Uint32 start = SDL_GetTicks();
    const Uint32 duration = 22000; // plus long pour ralentir le defilement

    while (1) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                TTF_CloseFont(font);
                return 1;
            }
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN || ev.type == SDL_EVENT_KEY_DOWN) {
                TTF_CloseFont(font);
                return 0;
            }
        }

        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - start;
        if (elapsed >= duration) break;

    float t = (float)elapsed / (float)duration;
    float sh = screen_heightf();
    const float line_h = 48.0f; // espacement plus grand pour lisibilite
    const float total_h = (float)n * line_h;

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        for (int i=0;i<n;i++) {
            float y = sh + (i * line_h) - t * (total_h + sh + 120.0f);
            SDL_Color col = {230,230,230,255};
            SDL_Surface *s = TTF_RenderText_Solid(font, lines[i], strlen(lines[i]), col);
            if (s) {
                SDL_Texture *tx = SDL_CreateTextureFromSurface(renderer, s);
                SDL_FRect dst = { screen_center_x() - s->w/2.0f, y, (float)s->w, (float)s->h };
                SDL_RenderTexture(renderer, tx, NULL, &dst);
                SDL_DestroyTexture(tx);
                SDL_DestroySurface(s);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    return 0;
}
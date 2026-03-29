#include "headers/main.h"

#include <SDL3_ttf/SDL_ttf.h>

static SDL_Texture *make_text(SDL_Renderer *renderer, TTF_Font *font, const char *txt, SDL_Color color) {
    SDL_Surface *s = TTF_RenderText_Solid(font, txt, SDL_strlen(txt), color);
    if (!s) return NULL;
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_DestroySurface(s);
    return t;
}

static void draw_line(SDL_Renderer *renderer, TTF_Font *font, const char *txt, float x, float y, SDL_Color color) {
    SDL_Texture *t = make_text(renderer, font, txt, color);
    if (!t) return;
    float tw = 0.0f, th = 0.0f;
    SDL_GetTextureSize(t, &tw, &th);
    SDL_FRect dst = {x, y, tw, th};
    SDL_RenderTexture(renderer, t, NULL, &dst);
    SDL_DestroyTexture(t);
}

static void achat_kit_soin(SDL_Renderer *renderer, t_case *hotbar[HOTBAR_SIZE], char notif[96], Uint32 *notif_time) {
    int prix = 3;
    if (argent >= prix) {
        t_Item *soin = init_item(SOIN, renderer, 0.0f, 0.0f);
        if (soin) {
            ajouter_item_hotbar(hotbar, soin, renderer);
            free(soin);
            argent -= prix;
            SDL_snprintf(notif, 96, "Achat: Kit de soin");
        } else {
            SDL_snprintf(notif, 96, "Erreur creation kit de soin");
        }
    } else {
        SDL_snprintf(notif, 96, "Pas assez d'argent");
    }
    *notif_time = SDL_GetTicks();
}

static void achat_vie_max(char notif[96], Uint32 *notif_time) {
    int prix = 5;
    if (argent >= prix) {
        if (perso.vie_max < 20) {
            perso.vie_max += 2;
            perso.vie += 2;
            if (perso.vie > perso.vie_max) perso.vie = perso.vie_max;
            argent -= prix;
            SDL_snprintf(notif, 96, "Achat: Vie max +2");
        } else {
            SDL_snprintf(notif, 96, "Vie max deja au maximum");
        }
    } else {
        SDL_snprintf(notif, 96, "Pas assez d'argent");
    }
    *notif_time = SDL_GetTicks();
}

static void achat_faim_max(char notif[96], Uint32 *notif_time) {
    int prix = 4;
    if (argent >= prix) {
        if (perso.faim_max < 20) {
            perso.faim_max += 2;
            perso.faim += 2;
            if (perso.faim > perso.faim_max) perso.faim = perso.faim_max;
            argent -= prix;
            SDL_snprintf(notif, 96, "Achat: Faim max +2");
        } else {
            SDL_snprintf(notif, 96, "Faim max deja au maximum");
        }
    } else {
        SDL_snprintf(notif, 96, "Pas assez d'argent");
    }
    *notif_time = SDL_GetTicks();
}

static void achat_vitesse(char notif[96], Uint32 *notif_time) {
    int prix = 6;
    if (argent >= prix) {
        if (vitesse_bonus < 160.0f) {
            vitesse_bonus += 20.0f;
            argent -= prix;
            SDL_snprintf(notif, 96, "Achat: Vitesse +20");
        } else {
            SDL_snprintf(notif, 96, "Vitesse deja au maximum");
        }
    } else {
        SDL_snprintf(notif, 96, "Pas assez d'argent");
    }
    *notif_time = SDL_GetTicks();
}

int afficher_shop(SDL_Renderer *renderer, t_case *hotbar[HOTBAR_SIZE]) {
    TTF_Font *font_title = TTF_OpenFont("assets/police_titre.ttf", 28);
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 20);
    if (!font) {
        if (font_title) TTF_CloseFont(font_title);
        return 0;
    }
    if (!font_title) font_title = font;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color soft = {190, 190, 210, 255};
    SDL_Color ok = {110, 230, 120, 255};
    SDL_Color ko = {255, 120, 120, 255};

    SDL_Texture *btn_tex = IMG_LoadTexture(renderer, "assets/menu/bouton.png");
    if (!btn_tex) {
        if (font_title != font) TTF_CloseFont(font_title);
        TTF_CloseFont(font);
        return 0;
    }

    Bouton btn_kit, btn_vie, btn_faim, btn_vit, btn_fermer;
    Bouton_Init(&btn_kit, 175.0f, 260.0f, 650.0f, 52.0f, btn_tex);
    Bouton_Init(&btn_vie, 175.0f, 320.0f, 650.0f, 52.0f, btn_tex);
    Bouton_Init(&btn_faim, 175.0f, 380.0f, 650.0f, 52.0f, btn_tex);
    Bouton_Init(&btn_vit, 175.0f, 440.0f, 650.0f, 52.0f, btn_tex);
    Bouton_Init(&btn_fermer, 345.0f, 510.0f, 310.0f, 52.0f, btn_tex);

    char notif[96] = "";
    Uint32 notif_time = 0;

    int running = 1;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                if (font_title != font) TTF_CloseFont(font_title);
                TTF_CloseFont(font);
                return 1;
            }
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE) {
                    running = 0;
                    break;
                }

                if (ev.key.key == SDLK_1) {
                    achat_kit_soin(renderer, hotbar, notif, &notif_time);
                }

                if (ev.key.key == SDLK_2) {
                    achat_vie_max(notif, &notif_time);
                }

                if (ev.key.key == SDLK_3) {
                    achat_faim_max(notif, &notif_time);
                }

                if (ev.key.key == SDLK_4) {
                    achat_vitesse(notif, &notif_time);
                }
            }

            if (Bouton_GererEvenement(&btn_kit, &ev)) {
                achat_kit_soin(renderer, hotbar, notif, &notif_time);
            }
            if (Bouton_GererEvenement(&btn_vie, &ev)) {
                achat_vie_max(notif, &notif_time);
            }
            if (Bouton_GererEvenement(&btn_faim, &ev)) {
                achat_faim_max(notif, &notif_time);
            }
            if (Bouton_GererEvenement(&btn_vit, &ev)) {
                achat_vitesse(notif, &notif_time);
            }
            if (Bouton_GererEvenement(&btn_fermer, &ev)) {
                running = 0;
            }
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 190);
        SDL_FRect full = {0, 0, 1000, 800};
        SDL_RenderFillRect(renderer, &full);

        SDL_SetRenderDrawColor(renderer, 20, 24, 40, 240);
        SDL_FRect panel = {110.0f, 110.0f, 780.0f, 560.0f};
        SDL_RenderFillRect(renderer, &panel);
        SDL_SetRenderDrawColor(renderer, 100, 150, 220, 255);
        SDL_RenderRect(renderer, &panel);

        draw_line(renderer, font_title, "Boutique du Vaisseau", 320.0f, 145.0f, white);

        char stats[128];
        SDL_snprintf(stats, sizeof(stats), "Argent: %d$ | Vie max: %d | Faim max: %d | Bonus vitesse: +%.0f", argent, perso.vie_max, perso.faim_max, vitesse_bonus);
        draw_line(renderer, font, stats, 145.0f, 205.0f, soft);

        Bouton_Afficher(&btn_kit, renderer);
        Bouton_Afficher(&btn_vie, renderer);
        Bouton_Afficher(&btn_faim, renderer);
        Bouton_Afficher(&btn_vit, renderer);
        Bouton_Afficher(&btn_fermer, renderer);

        draw_line(renderer, font, "[1] Kit de soin (+1 en hotbar) - 3$", 210.0f, 276.0f, white);
        draw_line(renderer, font, "[2] Amelioration vie max (+2) - 5$", 210.0f, 336.0f, white);
        draw_line(renderer, font, "[3] Amelioration faim max (+2) - 4$", 210.0f, 396.0f, white);
        draw_line(renderer, font, "[4] Amelioration vitesse (+20) - 6$", 210.0f, 456.0f, white);
        draw_line(renderer, font, "Fermer la boutique", 420.0f, 526.0f, soft);
        draw_line(renderer, font, "(Souris: cliquez sur un bouton)  |  (Clavier: 1,2,3,4)", 220.0f, 595.0f, soft);

        if (notif[0] != '\0' && SDL_GetTicks() - notif_time < 1800) {
            draw_line(renderer, font, notif, 155.0f, 555.0f,
                      (SDL_strstr(notif, "Pas assez") || SDL_strstr(notif, "maximum") || SDL_strstr(notif, "Erreur")) ? ko : ok);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(btn_tex);
    if (font_title != font) TTF_CloseFont(font_title);
    TTF_CloseFont(font);
    return 0;
}
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <string.h>
#include <stdio.h>
#include "headers/main.h"
#include "headers/pause.h"

int afficher_pause(SDL_Renderer *renderer, MIX_Track *track_global) {
    if (track_global) pause_son(track_global);

    
    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur police pause : %s", SDL_GetError());
        if (track_global) reprendre_son(track_global);
        return PAUSE_REPRENDRE;
    }
    TTF_Font *font_titre = TTF_OpenFont("assets/BiggerBook.ttf", 36);
    if (!font_titre) font_titre = font;

    SDL_Texture *btn_tex = IMG_LoadTexture(renderer, "assets/menu/bouton.png");
    if (!btn_tex) {
        SDL_Log("Erreur texture bouton pause : %s", SDL_GetError());
        TTF_CloseFont(font);
        if (font_titre != font) TTF_CloseFont(font_titre);
        if (track_global) reprendre_son(track_global);
        return PAUSE_REPRENDRE;
    }

    float btn_w = 250.0f, btn_h = 65.0f, gap = 10.0f;
    float cx  = screen_center_x() - btn_w * 0.5f;
    float y0  = 290.0f;

    Bouton btn_reprendre, btn_sauvegarder, btn_options, btn_menu;
    Bouton_Init(&btn_reprendre,   cx, y0,                       btn_w, btn_h, btn_tex);
    Bouton_Init(&btn_sauvegarder, cx, y0 + (btn_h + gap),       btn_w, btn_h, btn_tex);
    Bouton_Init(&btn_options,     cx, y0 + (btn_h + gap) * 2,   btn_w, btn_h, btn_tex);
    Bouton_Init(&btn_menu,        cx, y0 + (btn_h + gap) * 3,   btn_w, btn_h, btn_tex);

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color gris  = {160, 160, 160, 255};

#define MAKE_TEX(font_, text_, color_) ({ \
    SDL_Surface *_s = TTF_RenderText_Solid(font_, text_, strlen(text_), color_); \
    SDL_Texture *_t = _s ? SDL_CreateTextureFromSurface(renderer, _s) : NULL; \
    if (_s) SDL_DestroySurface(_s); \
    _t; })

    SDL_Texture *t_rep = MAKE_TEX(font, "Reprendre",          blanc);
    SDL_Texture *t_sav = MAKE_TEX(font, "Sauvegarde rapide",  gris);
    SDL_Texture *t_opt = MAKE_TEX(font, "Options",             blanc);
    SDL_Texture *t_men = MAKE_TEX(font, "Menu principal",      blanc);
#undef MAKE_TEX

    bool   notif_sav  = false;
    Uint32 notif_time = 0;
    char   notif_msg[96] = "";

    int  resultat = PAUSE_REPRENDRE;
    bool running  = true;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_MOUSE_MOTION ||
                ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                ev.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                SDL_ConvertEventToRenderCoordinates(renderer, &ev);
            }

            if (ev.type == SDL_EVENT_QUIT) {
                resultat = PAUSE_MENU; running = false; break;
            }
            if (ev.type == SDL_EVENT_KEY_DOWN && ev.key.key == SDLK_ESCAPE) {
                resultat = PAUSE_REPRENDRE; running = false; break;
            }

            if (Bouton_GererEvenement(&btn_reprendre, &ev)) {
                resultat = PAUSE_REPRENDRE; running = false;
            }
            if (Bouton_GererEvenement(&btn_menu, &ev)) {
                resultat = PAUSE_MENU; running = false;
            }
            if (Bouton_GererEvenement(&btn_sauvegarder, &ev)) {
                int slot = sauvegarde_choisir_slot(renderer, "Sauvegarder la partie", false);
                if (slot > 0 && sauvegarder_partie_slot(slot, Planete_actuelle) == 0) {
                    SDL_snprintf(notif_msg, sizeof(notif_msg), "Sauvegarde effectuee (slot %d)", slot);
                } else if (slot > 0) {
                    SDL_snprintf(notif_msg, sizeof(notif_msg), "Erreur de sauvegarde (slot %d)", slot);
                } else {
                    SDL_snprintf(notif_msg, sizeof(notif_msg), "Sauvegarde annulee");
                }
                notif_sav  = true;
                notif_time = SDL_GetTicks();
            }
            if (Bouton_GererEvenement(&btn_options, &ev)) {
                // Ouvrir les options EN INTERNE — la pause reste active en dessous
                // La musique reste en pause pendant les options
                afficher_option(renderer, track_global);
                // On revient ici directement, rien n'a changé dans l'état du jeu
            }
        }

        
        // Fond semi-transparent 
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
        SDL_FRect ecran = {0.0f, 0.0f, screen_widthf(), screen_heightf()};
        SDL_RenderFillRect(renderer, &ecran);

        // Panneau
        float pan_w = 400.0f;
        float pan_h = btn_h * 4 + gap * 3 + 140.0f;
        float pan_x = screen_center_x() - pan_w * 0.5f;
        float pan_y = y0 - 85.0f;

        SDL_SetRenderDrawColor(renderer, 20, 20, 35, 220);
        SDL_FRect panneau = {pan_x, pan_y, pan_w, pan_h};
        SDL_RenderFillRect(renderer, &panneau);
        SDL_SetRenderDrawColor(renderer, 80, 120, 180, 255);
        SDL_RenderRect(renderer, &panneau);

        // Titre
        SDL_Surface *surf_titre = TTF_RenderText_Blended(font_titre, "PAUSE", 0, blanc);
        if (surf_titre) {
            SDL_Texture *tex_titre = SDL_CreateTextureFromSurface(renderer, surf_titre);
            SDL_FRect dst = {screen_center_x() - surf_titre->w * 0.5f, pan_y + 15.0f,
                             (float)surf_titre->w, (float)surf_titre->h};
            SDL_RenderTexture(renderer, tex_titre, NULL, &dst);
            SDL_DestroyTexture(tex_titre);
            SDL_DestroySurface(surf_titre);
        }

        // Ligne pour separer
        SDL_SetRenderDrawColor(renderer, 80, 120, 180, 150);
        SDL_FRect ligne = {pan_x + 20, y0 - 15, pan_w - 40, 2};
        SDL_RenderFillRect(renderer, &ligne);

        // Boutons + labels
        Bouton_Afficher(&btn_reprendre,   renderer);
        Bouton_Afficher(&btn_sauvegarder, renderer);
        Bouton_Afficher(&btn_options,     renderer);
        Bouton_Afficher(&btn_menu,        renderer);

        if (t_rep) afficher_texte_centre(renderer, t_rep, &btn_reprendre.rect);
        if (t_sav) afficher_texte_centre(renderer, t_sav, &btn_sauvegarder.rect);
        if (t_opt) afficher_texte_centre(renderer, t_opt, &btn_options.rect);
        if (t_men) afficher_texte_centre(renderer, t_men, &btn_menu.rect);

        // Notif sauvegarde
        if (notif_sav && SDL_GetTicks() - notif_time < 2500) {
            SDL_Color jaune = {255, 210, 50, 255};
            SDL_Surface *s = TTF_RenderText_Solid(font,
                notif_msg,
                strlen(notif_msg), jaune);
            if (s) {
                SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_FRect dst = {screen_center_x() - s->w * 0.5f,
                                 btn_sauvegarder.rect.y + btn_h + 4.0f,
                                 (float)s->w, (float)s->h};
                SDL_RenderTexture(renderer, t, NULL, &dst);
                SDL_DestroyTexture(t);
                SDL_DestroySurface(s);
            }
        } else if (notif_sav && SDL_GetTicks() - notif_time >= 2500) {
            notif_sav = false;
        }

        //Échap
        SDL_Color gris_hint = {120, 130, 150, 200};
        SDL_Surface *s_hint = TTF_RenderText_Solid(font,
            "[ Echap ] pour reprendre",
            strlen("[ Echap ] pour reprendre"), gris_hint);
        if (s_hint) {
            SDL_Texture *t_hint = SDL_CreateTextureFromSurface(renderer, s_hint);
            SDL_FRect dst = {screen_center_x() - s_hint->w * 0.5f,
                             pan_y + pan_h - s_hint->h - 8.0f,
                             (float)s_hint->w, (float)s_hint->h};
            SDL_RenderTexture(renderer, t_hint, NULL, &dst);
            SDL_DestroyTexture(t_hint);
            SDL_DestroySurface(s_hint);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (t_rep) SDL_DestroyTexture(t_rep);
    if (t_sav) SDL_DestroyTexture(t_sav);
    if (t_opt) SDL_DestroyTexture(t_opt);
    if (t_men) SDL_DestroyTexture(t_men);
    SDL_DestroyTexture(btn_tex);
    if (font_titre != font) TTF_CloseFont(font_titre);
    TTF_CloseFont(font);

    // Reprendre la musique uniquement si on retourne en jeu
    if (resultat == PAUSE_REPRENDRE && track_global)
        reprendre_son(track_global);

    return resultat;
}

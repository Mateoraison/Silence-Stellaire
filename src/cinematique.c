#include "headers/main.h"
#include "headers/cinematique.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

static void render_texture_centered(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, float w, float h) {
    if (!texture) return;
    SDL_FRect dst = {x - w * 0.5f, y - h * 0.5f, w, h};
    SDL_RenderTexture(renderer, texture, NULL, &dst);
}

static void render_texture_centered_native(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y) {
    if (!texture) return;
    float tw = 0.0f, th = 0.0f;
    SDL_GetTextureSize(texture, &tw, &th);
    SDL_FRect dst = {x - tw * 0.5f, y - th * 0.5f, tw, th};
    SDL_RenderTexture(renderer, texture, NULL, &dst);
}

int jouer_cinematique_intro(SDL_Renderer *renderer) {
    SDL_Texture *fond = IMG_LoadTexture(renderer, "assets/menu/menu.png");
    SDL_Texture *logo = IMG_LoadTexture(renderer, "assets/logo_win.png");
    TTF_Font *font_titre = TTF_OpenFont("assets/police_titre.ttf", 36);
    TTF_Font *font_info = TTF_OpenFont("assets/police.ttf", 20);

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color gris = {190, 190, 190, 255};

    SDL_Texture *tex_titre = NULL;
    SDL_Texture *tex_info = NULL;
    SDL_Texture *tex_skip = NULL;

    if (font_titre) {
        SDL_Surface *s = TTF_RenderText_Solid(font_titre, "Silence Stellaire", 16, blanc);
        if (s) {
            tex_titre = SDL_CreateTextureFromSurface(renderer, s);
            SDL_DestroySurface(s);
        }
    }

    if (font_info) {
        SDL_Surface *s = TTF_RenderText_Solid(font_info, "Un voyage commence dans le silence de l'espace...", 50, gris);
        if (s) {
            tex_info = SDL_CreateTextureFromSurface(renderer, s);
            SDL_DestroySurface(s);
        }

        s = TTF_RenderText_Solid(font_info, "Espace / Entree / Echap pour passer", 35, gris);
        if (s) {
            tex_skip = SDL_CreateTextureFromSurface(renderer, s);
            SDL_DestroySurface(s);
        }
    }

    Uint32 debut = SDL_GetTicks();
    const Uint32 duree = 7000;

    while (1) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                if (fond) SDL_DestroyTexture(fond);
                if (logo) SDL_DestroyTexture(logo);
                if (tex_titre) SDL_DestroyTexture(tex_titre);
                if (tex_info) SDL_DestroyTexture(tex_info);
                if (tex_skip) SDL_DestroyTexture(tex_skip);
                if (font_titre) TTF_CloseFont(font_titre);
                if (font_info) TTF_CloseFont(font_info);
                return 1;
            }
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE || ev.key.key == SDLK_SPACE || ev.key.key == SDLK_RETURN) {
                    if (fond) SDL_DestroyTexture(fond);
                    if (logo) SDL_DestroyTexture(logo);
                    if (tex_titre) SDL_DestroyTexture(tex_titre);
                    if (tex_info) SDL_DestroyTexture(tex_info);
                    if (tex_skip) SDL_DestroyTexture(tex_skip);
                    if (font_titre) TTF_CloseFont(font_titre);
                    if (font_info) TTF_CloseFont(font_info);
                    return 0;
                }
            }
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (fond) SDL_DestroyTexture(fond);
                if (logo) SDL_DestroyTexture(logo);
                if (tex_titre) SDL_DestroyTexture(tex_titre);
                if (tex_info) SDL_DestroyTexture(tex_info);
                if (tex_skip) SDL_DestroyTexture(tex_skip);
                if (font_titre) TTF_CloseFont(font_titre);
                if (font_info) TTF_CloseFont(font_info);
                return 0;
            }
        }

        Uint32 t = SDL_GetTicks() - debut;
        if (t >= duree) break;

        float phase = (float)t / (float)duree;
        float alpha_mul = 1.0f;
        if (phase < 0.15f) alpha_mul = phase / 0.15f;
        else if (phase > 0.85f) alpha_mul = (1.0f - phase) / 0.15f;
        if (alpha_mul < 0.0f) alpha_mul = 0.0f;
        if (alpha_mul > 1.0f) alpha_mul = 1.0f;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (fond) {
            SDL_SetTextureAlphaMod(fond, 255);
            SDL_RenderTexture(renderer, fond, NULL, NULL);
        }

        if (logo) {
            Uint8 a = (Uint8)(255.0f * alpha_mul);
            SDL_SetTextureAlphaMod(logo, a);
            render_texture_centered(renderer, logo, screen_center_x(), screen_center_y() - 100.0f, 260.0f, 260.0f);
        }

        if (tex_titre) {
            Uint8 a = (Uint8)(255.0f * alpha_mul);
            SDL_SetTextureAlphaMod(tex_titre, a);
            render_texture_centered_native(renderer, tex_titre, screen_center_x(), screen_center_y() + 120.0f);
        }

        if (tex_info && t > 1600) {
            Uint8 a = (Uint8)(220.0f * alpha_mul);
            SDL_SetTextureAlphaMod(tex_info, a);
            render_texture_centered_native(renderer, tex_info, screen_center_x(), screen_center_y() + 175.0f);
        }

        if (tex_skip) {
            Uint8 a = (Uint8)(150.0f * alpha_mul);
            SDL_SetTextureAlphaMod(tex_skip, a);
            render_texture_centered_native(renderer, tex_skip, screen_center_x(), screen_heightf() - 60.0f);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (fond) SDL_DestroyTexture(fond);
    if (logo) SDL_DestroyTexture(logo);
    if (tex_titre) SDL_DestroyTexture(tex_titre);
    if (tex_info) SDL_DestroyTexture(tex_info);
    if (tex_skip) SDL_DestroyTexture(tex_skip);
    if (font_titre) TTF_CloseFont(font_titre);
    if (font_info) TTF_CloseFont(font_info);
    return 0;
}
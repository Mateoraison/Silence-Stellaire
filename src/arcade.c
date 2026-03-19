#include "headers/main.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

#define MAX_PROJECTILES 5
#define MAX_ENNEMIS 8

typedef struct { float x, y; bool actif; } Entite;

// --- PETIT HELPER LOCAL POUR LE TEXTE (Évite vaisseau.c) ---
void arcade_draw_text(SDL_Renderer *r, const char* text, float x, float y, bool center) {
    TTF_Font *f = TTF_OpenFont("assets/police.ttf", 18); // On ouvre la police localement
    if (!f) return;
    
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Blended(f, text, 0, blanc);
    if (surf) {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
        if (tex) {
            float finalX = center ? x - (surf->w / 2.0f) : x;
            SDL_FRect dst = { finalX, y, (float)surf->w, (float)surf->h };
            SDL_RenderTexture(r, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_DestroySurface(surf);
    }
    TTF_CloseFont(f);
}

// --- JEU 1 : STARSHIP DEFENDER (SHOOTER) ---
void jouer_arcade1(SDL_Renderer *renderer) {
    bool running = true;
    int etat = 0; // 0: Menu, 1: Jeu, 2: Perdu
    float j_x = 400.0f;
    Entite tirs[MAX_PROJECTILES] = {0};
    Entite ennemis[MAX_ENNEMIS] = {0};
    int score = 0;
    Uint32 last_e = 0;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_Q) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) etat = 1;
                if (etat == 2 && ev.key.key == SDLK_R) { // Reset
                    etat = 1; score = 0; j_x = 400;
                    for(int i=0; i<MAX_PROJECTILES; i++) tirs[i].actif = false;
                    for(int i=0; i<MAX_ENNEMIS; i++) ennemis[i].actif = false;
                }
                if (etat == 1 && ev.key.key == SDLK_SPACE) {
                    for(int i=0; i<MAX_PROJECTILES; i++) if(!tirs[i].actif) {
                        tirs[i].x = j_x + 12; tirs[i].y = 530; tirs[i].actif = true; break;
                    }
                }
            }
        }

        if (etat == 1) {
            const bool *k = SDL_GetKeyboardState(NULL);
            if (k[SDL_SCANCODE_LEFT] && j_x > 305) j_x -= 5;
            if (k[SDL_SCANCODE_RIGHT] && j_x < 465) j_x += 5;

            if (SDL_GetTicks() - last_e > 1000) {
                for(int i=0; i<MAX_ENNEMIS; i++) if(!ennemis[i].actif) {
                    ennemis[i].x = 310 + (rand() % 160); ennemis[i].y = 200;
                    ennemis[i].actif = true; last_e = SDL_GetTicks(); break;
                }
            }
            for(int i=0; i<MAX_PROJECTILES; i++) if(tirs[i].actif) {
                tirs[i].y -= 7; if(tirs[i].y < 200) tirs[i].actif = false;
            }
            for(int i=0; i<MAX_ENNEMIS; i++) if(ennemis[i].actif) {
                ennemis[i].y += 3;
                if(ennemis[i].y > 550) etat = 2; // Perdu si un ennemi passe
                for(int j=0; j<MAX_PROJECTILES; j++) if(tirs[j].actif && ennemis[i].actif) {
                    if(tirs[j].x < ennemis[i].x + 20 && tirs[j].x + 5 > ennemis[i].x &&
                       tirs[j].y < ennemis[i].y + 20 && tirs[j].y + 10 > ennemis[i].y) {
                        tirs[j].actif = false; ennemis[i].actif = false; score++;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 10, 10, 15, 255);
        SDL_FRect ecran = { 300, 200, 200, 400 };
        SDL_RenderFillRect(renderer, &ecran);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderRect(renderer, &ecran);

        if (etat == 0) {
            arcade_draw_text(renderer, "STAR DEFENDER", 400, 250, true);
            arcade_draw_text(renderer, "FLECHES: BOUGER", 400, 320, true);
            arcade_draw_text(renderer, "ESPACE: TIRER", 400, 350, true);
            arcade_draw_text(renderer, "PRESSER ESPACE", 400, 450, true);
        } else if (etat == 2) {
            char res[32]; sprintf(res, "SCORE: %d", score);
            arcade_draw_text(renderer, "GAME OVER", 400, 250, true);
            arcade_draw_text(renderer, res, 400, 300, true);
            arcade_draw_text(renderer, "R: REJOUER", 400, 400, true);
            arcade_draw_text(renderer, "Q: QUITTER", 400, 430, true);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
            SDL_FRect rj = { j_x, 540, 30, 10 }; SDL_RenderFillRect(renderer, &rj);
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
            for(int i=0; i<MAX_ENNEMIS; i++) if(ennemis[i].actif) {
                SDL_FRect r = { ennemis[i].x, ennemis[i].y, 20, 20 }; SDL_RenderFillRect(renderer, &r);
            }
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            for(int i=0; i<MAX_PROJECTILES; i++) if(tirs[i].actif) {
                SDL_FRect r = { tirs[i].x, tirs[i].y, 4, 10 }; SDL_RenderFillRect(renderer, &r);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
    SDL_Delay(150);
}

// --- JEU 2 : GRAVITY FALL (ESQUIVE) ---
void jouer_arcade2(SDL_Renderer *renderer) {
    bool running = true;
    int etat = 0; 
    float j_x = 400.0f, j_y = 250.0f;
    typedef struct { float y, tx; bool a; } Br;
    Br b[4] = {0};
    int score = 0; Uint32 last = 0;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_Q) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) etat = 1;
                if (etat == 2 && ev.key.key == SDLK_R) {
                    etat = 1; score = 0; j_x = 400; last = SDL_GetTicks();
                    for(int i=0; i<4; i++) b[i].a = false;
                }
            }
        }

        if (etat == 1) {
            const bool *k = SDL_GetKeyboardState(NULL);
            if (k[SDL_SCANCODE_LEFT] && j_x > 300) j_x -= 5;
            if (k[SDL_SCANCODE_RIGHT] && j_x < 490) j_x += 5;

            if (SDL_GetTicks() - last > 1300) {
                for(int i=0; i<4; i++) if(!b[i].a) {
                    b[i].y = 600; b[i].tx = 310 + (rand()%130);
                    b[i].a = true; last = SDL_GetTicks(); break;
                }
            }
            for(int i=0; i<4; i++) if(b[i].a) {
                b[i].y -= (3.0f + score * 0.2f);
                if (j_y + 10 > b[i].y && j_y < b[i].y + 15) {
                    if (j_x < b[i].tx || j_x + 10 > b[i].tx + 50) etat = 2;
                }
                if(b[i].y < 190) { b[i].a = false; score++; }
            }
        }

        SDL_SetRenderDrawColor(renderer, 5, 20, 5, 255);
        SDL_FRect ecran = { 300, 200, 200, 400 };
        SDL_RenderFillRect(renderer, &ecran);

        if (etat == 0) {
            arcade_draw_text(renderer, "GRAVITY FALL", 400, 250, true);
            arcade_draw_text(renderer, "ESQUIVE LES MURS", 400, 320, true);
            arcade_draw_text(renderer, "PRESSER ESPACE", 400, 450, true);
        } else if (etat == 2) {
            char res[32]; sprintf(res, "PASSES: %d", score);
            arcade_draw_text(renderer, "BOUM !", 400, 250, true);
            arcade_draw_text(renderer, res, 400, 300, true);
            arcade_draw_text(renderer, "R: REJOUER", 400, 400, true);
            arcade_draw_text(renderer, "Q: QUITTER", 400, 430, true);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect p = { j_x, j_y, 10, 10 }; SDL_RenderFillRect(renderer, &p);
            SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
            for(int i=0; i<4; i++) if(b[i].a) {
                SDL_FRect g = { 300, b[i].y, b[i].tx-300, 15 };
                SDL_FRect d = { b[i].tx+50, b[i].y, 500-(b[i].tx+50), 15 };
                SDL_RenderFillRect(renderer, &g); SDL_RenderFillRect(renderer, &d);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);       
    }
    SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
    SDL_Delay(150);
}

// --- JEU 3 : SPACE PADDLE (REBONDS) ---
void jouer_arcade3(SDL_Renderer *renderer) {
    bool running = true;
    int etat = 0;
    float rx = 370, bx = 400, by = 300, vx = 3, vy = 3;
    float vitesse = 1.10f ;
    int rebonds = 0;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_Q) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) etat = 1;
                if (etat == 2 && ev.key.key == SDLK_R) {
                    etat = 1; rebonds = 0; bx = 400; by = 300; vx = 3; vy = 3;
                }
            }
        }

        if (etat == 1) {
            const bool *k = SDL_GetKeyboardState(NULL);
            if (k[SDL_SCANCODE_LEFT] && rx > 300) rx -= 7;
            if (k[SDL_SCANCODE_RIGHT] && rx < 440) rx += 7;
            bx += vx; by += vy;
            if (bx < 300 || bx > 490) vx *= -1;
            if (by < 200) vy *= -1;
            if (by > 560 && by < 575 && bx > rx && bx < rx + 60) {
                vy *= -vitesse; vx *= vitesse; by = 559; rebonds++;
            }
            if (by > 600) etat = 2;
        }

        SDL_SetRenderDrawColor(renderer, 20, 0, 20, 255);
        SDL_FRect ecran = { 300, 200, 200, 400 };
        SDL_RenderFillRect(renderer, &ecran);

        if (etat == 0) {
            arcade_draw_text(renderer, "SPACE PADDLE", 400, 250, true);
            arcade_draw_text(renderer, "NE LAISSE", 400, 320, true);
            arcade_draw_text(renderer, "PAS TOMBER", 400, 350, true);
            arcade_draw_text(renderer, "ESPACE : COMMENCER", 400, 450, true);
        } else if (etat == 2) {
            char res[32]; sprintf(res, "REBONDS: %d", rebonds);
            arcade_draw_text(renderer, "PERDU !", 400, 250, true);
            arcade_draw_text(renderer, res, 400, 300, true);
            arcade_draw_text(renderer, "R: REESSAYER", 400, 400, true);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect b = { bx, by, 10, 10 }; SDL_RenderFillRect(renderer, &b);
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            SDL_FRect r = { rx, 570, 60, 10 }; SDL_RenderFillRect(renderer, &r);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
    SDL_Delay(150);
}
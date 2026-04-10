/**
 * @file arcade.c
 * @brief Gestion de la séquence d'arcade et de ses ecrans associes.
 */

#include "headers/main.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_PROJECTILES 5
#define MAX_ENNEMIS 8

/** @brief Entite simple utilisee pour les mini-jeux d'arcade. */
typedef struct { float x, y; bool actif; } Entite;

#define ARCADE_SCREEN_X 300.0f
#define ARCADE_SCREEN_Y 200.0f
#define ARCADE_SCREEN_W 200.0f
#define ARCADE_SCREEN_H 400.0f
#define ARCADE_TEXT_PADDING 8.0f
#define ARCADE_INFO_X ARCADE_SCREEN_X
#define ARCADE_INFO_Y (ARCADE_SCREEN_Y + ARCADE_SCREEN_H + 10.0f)
#define ARCADE_INFO_W ARCADE_SCREEN_W
#define ARCADE_INFO_H 56.0f

static TTF_Font *arcade_font = NULL;
bool g_mastermind_reussi = false;
bool g_simon_reussi = false;

void arcade_draw_text(SDL_Renderer *r, const char* text, float x, float y, bool center);

static TTF_Font *arcade_get_font(void) {
    if (!arcade_font) {
        arcade_font = TTF_OpenFont("assets/police.ttf", 18);
    }
    return arcade_font;
}

static void arcade_draw_frame(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 10, 10, 15, 255);
    SDL_FRect ecran = { ARCADE_SCREEN_X, ARCADE_SCREEN_Y, ARCADE_SCREEN_W, ARCADE_SCREEN_H };
    SDL_RenderFillRect(renderer, &ecran);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderRect(renderer, &ecran);
}

static void minijeu_draw_frame(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 10, 10, 15, 255);
    SDL_FRect ecran = { ARCADE_SCREEN_X, ARCADE_SCREEN_Y, ARCADE_SCREEN_W + 100, ARCADE_SCREEN_H };
    SDL_RenderFillRect(renderer, &ecran);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderRect(renderer, &ecran);
}

static void arcade_draw_hud(SDL_Renderer *renderer, const char *line1, const char *line2, int value, const char *label) {
    char score_line[64];
    snprintf(score_line, sizeof(score_line), "%s: %d", label, value);

    SDL_SetRenderDrawColor(renderer, 10, 10, 15, 255);
    SDL_FRect panneau = { ARCADE_INFO_X, ARCADE_INFO_Y, ARCADE_INFO_W, ARCADE_INFO_H };
    SDL_RenderFillRect(renderer, &panneau);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderRect(renderer, &panneau);

    arcade_draw_text(renderer, score_line, 400, 210, true);
    arcade_draw_text(renderer, line1, 400, ARCADE_INFO_Y + 8.0f, true);
    arcade_draw_text(renderer, line2, 400, ARCADE_INFO_Y + 28.0f, true);
}

static void arcade_draw_info_panel(SDL_Renderer *renderer, const char *line1, const char *line2) {
    SDL_SetRenderDrawColor(renderer, 10, 10, 15, 255);
    SDL_FRect panneau = { ARCADE_INFO_X, ARCADE_INFO_Y, ARCADE_INFO_W, ARCADE_INFO_H };
    SDL_RenderFillRect(renderer, &panneau);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderRect(renderer, &panneau);

    arcade_draw_text(renderer, line1, 400, ARCADE_INFO_Y + 8.0f, true);
    arcade_draw_text(renderer, line2, 400, ARCADE_INFO_Y + 28.0f, true);
}

static void reset_projectiles(Entite *tirs) {
    for (int i = 0; i < MAX_PROJECTILES; i++) tirs[i].actif = false;
}

static void reset_ennemis(Entite *ennemis) {
    for (int i = 0; i < MAX_ENNEMIS; i++) ennemis[i].actif = false;
}

// --- PETIT HELPER LOCAL POUR LE TEXTE (Évite vaisseau.c) ---
void arcade_draw_text(SDL_Renderer *r, const char* text, float x, float y, bool center) {
    TTF_Font *f = arcade_get_font(); // On ouvre la police localement
    if (!f) return;
    
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Blended(f, text, 0, blanc);
    if (surf) {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
        if (tex) {
            const float max_width = ARCADE_SCREEN_W - (ARCADE_TEXT_PADDING * 2.0f);
            float scale = 1.0f;
            if ((float)surf->w > max_width && surf->w > 0) {
                scale = max_width / (float)surf->w;
            }

            float draw_w = (float)surf->w * scale;
            float draw_h = (float)surf->h * scale;
            float finalX = center ? x - (draw_w / 2.0f) : x;
            SDL_FRect dst = { finalX, y, draw_w, draw_h };
            SDL_RenderTexture(r, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_DestroySurface(surf);
    }
}

void minijeu_draw_text(SDL_Renderer *r, const char* text, float x, float y, bool center) {
    TTF_Font *f = arcade_get_font(); // On ouvre la police localement
    if (!f) return;
    
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Blended(f, text, 0, blanc);
    if (surf) {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
        if (tex) {
            const float max_width = ARCADE_SCREEN_W + 100 - (ARCADE_TEXT_PADDING * 2.0f);
            float scale = 1.0f;
            if ((float)surf->w > max_width && surf->w > 0) {
                scale = max_width / (float)surf->w;
            }

            float draw_w = (float)surf->w * scale;
            float draw_h = (float)surf->h * scale;
            float finalX = center ? x - (draw_w / 2.0f) : x;
            SDL_FRect dst = { finalX, y, draw_w, draw_h };
            SDL_RenderTexture(r, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_DestroySurface(surf);
    }
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
                if (ev.key.key == SDLK_ESCAPE) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) etat = 1;
                if (etat == 2 && ev.key.key == SDLK_R) { // Reset
                    etat = 1; score = 0; j_x = 400;
                    reset_projectiles(tirs);
                    reset_ennemis(ennemis);
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
            if ((k[SDL_SCANCODE_LEFT] || k[SDL_SCANCODE_Q] || k[SDL_SCANCODE_A]) && j_x > 305) j_x -= 5;
            if ((k[SDL_SCANCODE_RIGHT] || k[SDL_SCANCODE_D]) && j_x < 465) j_x += 5;

            Uint32 now = SDL_GetTicks();
            if (now - last_e > 1000) {
                for(int i=0; i<MAX_ENNEMIS; i++) if(!ennemis[i].actif) {
                    ennemis[i].x = 310 + (rand() % 160); ennemis[i].y = 200;
                    ennemis[i].actif = true; last_e = now; break;
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

        arcade_draw_frame(renderer);

        if (etat == 0) {
            arcade_draw_text(renderer, "STAR DEFENDER", 400, 250, true);
            arcade_draw_text(renderer, "OBJECTIF: ELIMINER LES ENNEMIS", 400, 290, true);
            arcade_draw_text(renderer, "← → / A D : BOUGER", 400, 330, true);
            arcade_draw_text(renderer, "ESPACE: TIRER", 400, 360, true);
            arcade_draw_text(renderer, "PRESSER ESPACE", 400, 450, true);
            arcade_draw_info_panel(renderer, "ESPACE: COMMENCER", "ECHAP: QUITTER LA BORNE");
        } else if (etat == 2) {
            char res[32]; sprintf(res, "SCORE: %d", score);
            arcade_draw_text(renderer, "GAME OVER", 400, 250, true);
            arcade_draw_text(renderer, res, 400, 300, true);
            arcade_draw_info_panel(renderer, "R: REJOUER", "ECHAP: QUITTER");
        } else {
            arcade_draw_hud(renderer, "← → / Q D : BOUGER", "ESPACE: TIRER | ECHAP: QUITTER", score, "SCORE");
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
    /** @brief Obstacle mobile de Gravity Fall (barriere avec ouverture). */
    typedef struct { float y, tx; bool a; } Br;
    Br b[4] = {0};
    int score = 0; Uint32 last = 0;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) etat = 1;
                if (etat == 2 && ev.key.key == SDLK_R) {
                    etat = 1; score = 0; j_x = 400; last = SDL_GetTicks();
                    for(int i=0; i<4; i++) b[i].a = false;
                }
            }
        }

        if (etat == 1) {
            const bool *k = SDL_GetKeyboardState(NULL);
            if ((k[SDL_SCANCODE_LEFT] || k[SDL_SCANCODE_Q] || k[SDL_SCANCODE_A]) && j_x > 300) j_x -= 5;
            if ((k[SDL_SCANCODE_RIGHT] || k[SDL_SCANCODE_D]) && j_x < 490) j_x += 5;

            Uint32 now = SDL_GetTicks();
            if (now - last > 1300) {
                for(int i=0; i<4; i++) if(!b[i].a) {
                    b[i].y = 600; b[i].tx = 310 + (rand()%130);
                    b[i].a = true; last = now; break;
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

        arcade_draw_frame(renderer);

        if (etat == 0) {
            arcade_draw_text(renderer, "GRAVITY FALL", 400, 250, true);
            arcade_draw_text(renderer, "OBJECTIF: PASSER LES BARRIERES", 400, 290, true);
            arcade_draw_text(renderer, "← → / Q D : BOUGER", 400, 330, true);
            arcade_draw_text(renderer, "PRESSER ESPACE", 400, 450, true);
            arcade_draw_info_panel(renderer, "ESPACE: COMMENCER", "ECHAP: QUITTER LA BORNE");
        } else if (etat == 2) {
            char res[32]; sprintf(res, "PASSES: %d", score);
            arcade_draw_text(renderer, "BOUM !", 400, 250, true);
            arcade_draw_text(renderer, res, 400, 300, true);
            arcade_draw_info_panel(renderer, "R: REJOUER", "ECHAP: QUITTER");
        } else {
            arcade_draw_hud(renderer, "← → / Q D : BOUGER", "EVITE LES MURS", score, "PASSES");
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
                if (ev.key.key == SDLK_ESCAPE) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) etat = 1;
                if (etat == 2 && ev.key.key == SDLK_R) {
                    etat = 1; rebonds = 0; bx = 400; by = 300; vx = 3; vy = 3;
                }
            }
        }

        if (etat == 1) {
            const bool *k = SDL_GetKeyboardState(NULL);
            if ((k[SDL_SCANCODE_LEFT] || k[SDL_SCANCODE_Q] || k[SDL_SCANCODE_A]) && rx > 300) rx -= 7;
            if ((k[SDL_SCANCODE_RIGHT] || k[SDL_SCANCODE_D]) && rx < 440) rx += 7;
            bx += vx; by += vy;
            if (bx < 300 || bx > 490) vx *= -1;
            if (by < 200) vy *= -1;
            if (by > 560 && by < 575 && bx > rx && bx < rx + 60) {
                vy *= -vitesse; vx *= vitesse; by = 559; rebonds++;
            }
            if (by > 600) etat = 2;
        }

        arcade_draw_frame(renderer);

        if (etat == 0) {
            arcade_draw_text(renderer, "SPACE PADDLE", 400, 250, true);
            arcade_draw_text(renderer, "OBJECTIF: TENIR LE PLUS LONGTEMPS", 400, 290, true);
            arcade_draw_text(renderer, "NE LAISSE PAS TOMBER", 400, 330, true);
            arcade_draw_text(renderer, "← → / Q D : BOUGER", 400, 360, true);
            arcade_draw_text(renderer, "PRESSER ESPACE", 400, 450, true);
            arcade_draw_info_panel(renderer, "ESPACE: COMMENCER", "ECHAP: QUITTER LA BORNE");
        } else if (etat == 2) {
            char res[32]; sprintf(res, "REBONDS: %d", rebonds);
            arcade_draw_text(renderer, "PERDU !", 400, 250, true);
            arcade_draw_text(renderer, res, 400, 300, true);
            arcade_draw_info_panel(renderer, "R: REESSAYER", "ECHAP: QUITTER");
        } else {
            arcade_draw_hud(renderer, "← → / Q D : BOUGER", "GARDE LA BALLE EN JEU | ECHAP: QUITTER", rebonds, "REBONDS");
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

// --- JEU 4 : MASTERMIND (CODE-BREAKER) ---
void mastermind(SDL_Renderer *renderer) {
    bool running = true;
    int etat = 0; // 0: Menu, 1: Jeu, 2: Gagné, 3: Perdu
    g_mastermind_reussi = false;
    
    // 6 couleurs pour Mastermind
    SDL_Color mm_colors[6] = {
        {255, 0, 0, 255},      // Rouge
        {0, 150, 255, 255},    // Bleu
        {0, 255, 0, 255},      // Vert
        {255, 255, 0, 255},    // Jaune
        {255, 100, 0, 255},    // Orange
        {255, 0, 255, 255}     // Magenta
    };
    
    // Générateur de code secret (4 couleurs de 0-5)
    int secret[4];
    for (int i = 0; i < 4; i++) secret[i] = rand() % 6;
    
    int tentatives = 0;
    int max_tentatives = 12;
    int guess[4] = {0, 0, 0, 0};
    int current_pos = 0;
    int score = 0;
    
    // Historique des tentatives
    /** @brief Tentative de Mastermind avec proposition et feedback. */
    typedef struct { int guess[4]; char feedback[4]; } Tentative;
    Tentative historique[12];
    int nb_tentatives = 0;
    
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) etat = 1;
                if (etat == 2 && ev.key.key == SDLK_R) {
                    etat = 1; tentatives = 0; current_pos = 0; nb_tentatives = 0; score = 0;
                    for (int i = 0; i < 4; i++) { secret[i] = rand() % 6; guess[i] = 0; }
                }
                if (etat == 3 && ev.key.key == SDLK_R) {
                    etat = 1; tentatives = 0; current_pos = 0; nb_tentatives = 0;
                    for (int i = 0; i < 4; i++) { secret[i] = rand() % 6; guess[i] = 0; }
                }
                
                if (etat == 1) {
                    if (ev.key.key >= SDLK_1 && ev.key.key <= SDLK_6) {
                        if (current_pos < 4) {
                            guess[current_pos] = ev.key.key - SDLK_1;
                            current_pos++;
                        }
                    }
                    if (ev.key.key == SDLK_BACKSPACE && current_pos > 0) {
                        current_pos--;
                        guess[current_pos] = 0;
                    }
                    if (ev.key.key == SDLK_RETURN && current_pos == 4) {
                        // Vérifier la tentative avec un verdict par position
                        bool secret_used[4] = {false};
                        bool guess_used[4] = {false};

                        for (int i = 0; i < 4; i++) {
                            historique[nb_tentatives].guess[i] = guess[i];
                            historique[nb_tentatives].feedback[i] = 'X';
                        }

                        // Première passe: bien placés
                        for (int i = 0; i < 4; i++) {
                            if (guess[i] == secret[i]) {
                                historique[nb_tentatives].feedback[i] = 'V';
                                secret_used[i] = true;
                                guess_used[i] = true;
                            }
                        }

                        // Deuxième passe: bonne couleur mais mal placée
                        for (int i = 0; i < 4; i++) {
                            if (!guess_used[i]) {
                                for (int j = 0; j < 4; j++) {
                                    if (!secret_used[j] && guess[i] == secret[j]) {
                                        historique[nb_tentatives].feedback[i] = 'O';
                                        secret_used[j] = true;
                                        break;
                                    }
                                }
                            }
                        }

                        int bien_place = 0;
                        for (int i = 0; i < 4; i++) {
                            if (historique[nb_tentatives].feedback[i] == 'V') bien_place++;
                        }
                        nb_tentatives++;
                        
                        if (bien_place == 4) {
                            etat = 2; // Gagné
                            g_mastermind_reussi = true;
                            score = (max_tentatives - tentatives) * 100;
                        } else if (tentatives >= max_tentatives - 1) {
                            etat = 3; // Perdu
                        }
                        
                        tentatives++;
                        current_pos = 0;
                        for (int i = 0; i < 4; i++) guess[i] = 0;
                    }
                }
            }
        }
        
        // Fond dégradé
        for (int y = 200; y < 600; y++) {
            int alpha = (y - 200) / 400 * 50;
            SDL_SetRenderDrawColor(renderer, 20 + alpha/4, 20 + alpha/4, 40 + alpha/4, 255);
            SDL_FRect line = {300, (float)y, 200, 1};
            SDL_RenderFillRect(renderer, &line);
        }
        
        minijeu_draw_frame(renderer);
        
        if (etat == 0) {
            minijeu_draw_text(renderer, "MASTERMIND", 450, 220, true);
            minijeu_draw_text(renderer, "TROUVEZ LE CODE SECRET", 450, 260, true);
            minijeu_draw_text(renderer, "6 COULEURS, 4 POSITIONS", 450, 295, true);
            minijeu_draw_text(renderer, "VOUS AVEZ 12 TENTATIVES", 450, 330, true);
            minijeu_draw_text(renderer, "1=Rouge  2=Bleu  3=Vert ", 450, 375, true);
            minijeu_draw_text(renderer, "4=Jaune  5=Orange  6=Magenta", 450, 400, true);
            minijeu_draw_text(renderer, "PRESSER ESPACE", 450, 450, true);
            arcade_draw_info_panel(renderer, "ESPACE: COMMENCER", "ECHAP: QUITTER");
        } else if (etat == 2) {
            char res[32]; sprintf(res, "BRAVO! SCORE: %d", score);
            minijeu_draw_text(renderer, "TROUVE!", 450, 250, true);
            minijeu_draw_text(renderer, res, 450, 300, true);
            minijeu_draw_text(renderer, "R: REJOUER   |   ECHAP: QUITTER", 450, 510, true);
        } else if (etat == 3) {
            minijeu_draw_text(renderer, "PERDU!", 450, 250, true);
            minijeu_draw_text(renderer, "TENTATIVES EPUISEES", 450, 300, true);
            minijeu_draw_text(renderer, "R: REJOUER   |   ECHAP: QUITTER", 450, 510, true);
        } else {
            char info[64];
            sprintf(info, "TENTATIVE: %d/%d", tentatives + 1, max_tentatives);
            
            // Affichage personnalisé 4 lignes pour Mastermind
            SDL_SetRenderDrawColor(renderer, 10, 10, 15, 255);
            SDL_FRect panneau = { ARCADE_INFO_X, ARCADE_INFO_Y, ARCADE_INFO_W + 100.0f, ARCADE_INFO_H + 60.0f };
            SDL_RenderFillRect(renderer, &panneau);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderRect(renderer, &panneau);
            
            minijeu_draw_text(renderer, info, 450, ARCADE_INFO_Y + 8.0f, true);
            minijeu_draw_text(renderer, "1=Rouge  2=Bleu  3=Vert", 450, ARCADE_INFO_Y + 30.0f, true);
            minijeu_draw_text(renderer, "4=Jaune  5=Orange  6=Magenta", 450, ARCADE_INFO_Y + 50.0f, true);
            minijeu_draw_text(renderer, "V: Bien place | O: Mal place | X: Absent", 450, ARCADE_INFO_Y + 80.0f, true);
            minijeu_draw_text(renderer, "BACKSPACE: EFFACER  |  ENTER: VALIDER", 450, ARCADE_INFO_Y + 100.0f, true);
            
            // Afficher les anciennes tentatives (dernières 5)
            float y = 450;
            int start = nb_tentatives > 5 ? nb_tentatives - 5 : 0;
            for (int i = start; i < nb_tentatives; i++) {
                // 4 petits carrés avec les couleurs de la tentative
                for (int j = 0; j < 4; j++) {
                    SDL_SetRenderDrawColor(renderer, 
                        mm_colors[historique[i].guess[j]].r,
                        mm_colors[historique[i].guess[j]].g,
                        mm_colors[historique[i].guess[j]].b, 255);
                    SDL_FRect sq = {330.0f + j * 18.0f, y, 14, 14};
                    SDL_RenderFillRect(renderer, &sq);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderRect(renderer, &sq);
                }
                
                // Afficher le verdict de chaque position
                char result[16];
                snprintf(result, sizeof(result), "%c %c %c %c",
                         historique[i].feedback[0],
                         historique[i].feedback[1],
                         historique[i].feedback[2],
                         historique[i].feedback[3]);
                minijeu_draw_text(renderer, result, 430, y - 2, false);

                y -= 22;
            }
            
            // Afficher le guess actuel (code en cours de saisie)
            float guess_y = 480;
            float base_x = 450.0f - (4 * 25.0f) / 2.0f; // centre le groupe de 4 cases
            for (int i = 0; i < 4; i++) {
                SDL_SetRenderDrawColor(renderer, 
                    mm_colors[guess[i]].r,
                    mm_colors[guess[i]].g,
                    mm_colors[guess[i]].b, 
                    i < current_pos ? 255 : 50);
                SDL_FRect sq = {base_x + i * 25.0f, guess_y, 20, 30};
                SDL_RenderFillRect(renderer, &sq);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderRect(renderer, &sq);
            }
            minijeu_draw_text(renderer, "CODE", 450, guess_y + 36, true);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
    SDL_Delay(150);
}

// --- JEU 5 : SIMON (MEMORY GAME) ---
void simon(SDL_Renderer *renderer) {
    bool running = true;
    int etat = 0; // 0: Menu, 1: Jeu, 2: Perdu, 3: Gagne
    const int objectif_sequences = 10; // Nombre de séquences à réussir pour gagner
    g_simon_reussi = false;
    
    int sequence[100] = {0};
    int sequence_len = 0;
    int player_idx = 0;
    int score = 0;
    Uint32 last_flash = 0;
    int flash_color = -1;
    bool playing_sequence = false;
    int sequence_idx = 0;
    Uint32 sequence_start = 0;
    
    // Les 4 couleurs en cercle: 0=haut-bleu, 1=bas-rouge, 2=gauche-vert, 3=droite-jaune
    SDL_Color colors[4] = {
        {0, 150, 255, 255},    // Bleu
        {255, 0, 100, 255},    // Rouge
        {100, 255, 0, 255},    // Vert
        {255, 200, 0, 255}     // Jaune
    };

    const char *color_labels[4] = {"1 ou ▲", "2 ou ▼", "3 ou <", "4 ou >"};
    
    // Positions en cercle (centre de la zone minijeu)
    float circle_x = 450.0f;
    float circle_y = 360.0f;
    float radius   = 90.0f;
    float pos_angles[4] = {-M_PI/2, M_PI/2, M_PI, 0}; // Haut, Bas, Gauche, Droite

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE) running = false;
                if (etat == 0 && ev.key.key == SDLK_SPACE) {
                    etat = 1;
                    sequence_len = 1;
                    sequence[0] = rand() % 4;
                    player_idx = 0;
                    playing_sequence = true;
                    sequence_idx = 0;
                    sequence_start = SDL_GetTicks();
                }
                if ((etat == 2 || etat == 3) && ev.key.key == SDLK_R) {
                    etat = 1;
                    sequence_len = 1;
                    player_idx = 0;
                    score = 0;
                    for (int i = 0; i < 100; i++) { sequence[i] = 0; }
                    sequence[0] = rand() % 4;
                    playing_sequence = true;
                    sequence_idx = 0;
                    sequence_start = SDL_GetTicks();
                }
                
                if (etat == 1 && !playing_sequence) {
                    int color_pressed = -1;
                    if (ev.key.key == SDLK_1 || ev.key.key == SDLK_UP) color_pressed = 0;
                    else if (ev.key.key == SDLK_2 || ev.key.key == SDLK_DOWN) color_pressed = 1;
                    else if (ev.key.key == SDLK_3 || ev.key.key == SDLK_LEFT) color_pressed = 2;
                    else if (ev.key.key == SDLK_4 || ev.key.key == SDLK_RIGHT) color_pressed = 3;

                    if (color_pressed >= 0 && player_idx < sequence_len) {
                        flash_color = color_pressed;
                        last_flash = SDL_GetTicks();
                        
                        if (color_pressed == sequence[player_idx]) {
                            player_idx++;
                            if (player_idx == sequence_len) {
                                // Correct! Ajouter une couleur
                                score++;
                                if (score >= objectif_sequences) {
                                    etat = 3; // Gagne
                                    g_simon_reussi = true;
                                    playing_sequence = false;
                                    flash_color = -1;
                                    continue;
                                }
                                if (sequence_len < 100) {
                                    sequence[sequence_len] = rand() % 4;
                                    sequence_len++;
                                }
                                player_idx = 0;
                                playing_sequence = true;
                                sequence_idx = 0;
                                sequence_start = SDL_GetTicks();
                            }
                        } else {
                            etat = 2; // Perdu
                        }
                    }
                }
            }
        }
        
        // Jouer la séquence
        Uint32 now = SDL_GetTicks();
        if (etat == 1 && playing_sequence) {
            if (now - sequence_start > 600) {
                if (sequence_idx < sequence_len) {
                    flash_color = sequence[sequence_idx];
                    last_flash = now;
                    sequence_start = now;
                    sequence_idx++;
                } else {
                    playing_sequence = false;
                    flash_color = -1;
                }
            }
        }
        
        // Flash timeout
        if (now > last_flash + 200) flash_color = -1;
        
        // Fond dégradé
        for (int y = 200; y < 600; y++) {
            int alpha = (y - 200) / 400 * 50;
            SDL_SetRenderDrawColor(renderer, 20 + alpha/4, 20 + alpha/4, 40 + alpha/4, 255);
            SDL_FRect line = {300, (float)y, 200, 1};
            SDL_RenderFillRect(renderer, &line);
        }
        
        minijeu_draw_frame(renderer);
        
        if (etat == 0) {
            minijeu_draw_text(renderer, "SIMON", 450, 220, true);
            minijeu_draw_text(renderer, "REPRODUISEZ LA SEQUENCE", 450, 260, true);
            minijeu_draw_text(renderer, "DE COULEURS", 450, 290, true);
            minijeu_draw_text(renderer, "1 ou ▲ BLEU  3 ou < VERT", 450, 340, true);
            minijeu_draw_text(renderer, "2 ou ▼ ROUGE  4 ou > JAUNE", 450, 365, true);
            minijeu_draw_text(renderer, "PRESSER ESPACE", 450, 450, true);
            arcade_draw_info_panel(renderer, "ESPACE: COMMENCER", "ECHAP: QUITTER");
        } else if (etat == 2) {
            char res[32]; sprintf(res, "SCORE: %d", score);
            minijeu_draw_text(renderer, "GAME OVER!", 450, 250, true);
            minijeu_draw_text(renderer, res, 450, 300, true);
            arcade_draw_info_panel(renderer, "R: REJOUER", "ECHAP: QUITTER");
        } else if (etat == 3) {
            char res[32]; sprintf(res, "SEQUENCES: %d", score);
            minijeu_draw_text(renderer, "VICTOIRE!", 450, 250, true);
            minijeu_draw_text(renderer, res, 450, 300, true);
            arcade_draw_info_panel(renderer, "R: REJOUER", "ECHAP: QUITTER");
        } else {
            char info[64];
            sprintf(info, "SEQUENCE: %d", sequence_len);
            arcade_draw_hud(renderer, info, "SUIVEZ LA SEQUENCE", score, "NIVEAU");
            
            // Afficher les 4 boutons de couleur en cercle
            for (int i = 0; i < 4; i++) {
                float x = circle_x + radius * cosf(pos_angles[i]);
                float y = circle_y + radius * sinf(pos_angles[i]);
                float button_radius = (flash_color == i ? 40.0f : 34.0f);
                
                SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
                
                // Remplissage disque simple (bandes horizontales)
                for (float dy = -button_radius; dy <= button_radius; dy += 1.0f) {
                    float dx = sqrtf(button_radius * button_radius - dy * dy);
                    SDL_RenderLine(renderer, x - dx, y + dy, x + dx, y + dy);
                }
                
                // Bordure blanche
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                for (float angle = 0; angle < 2.0f * (float)M_PI; angle += 0.05f) {
                    float x1 = x + button_radius * cosf(angle);
                    float y1 = y + button_radius * sinf(angle);
                    float x2 = x + button_radius * cosf(angle + 0.05f);
                    float y2 = y + button_radius * sinf(angle + 0.05f);
                    SDL_RenderLine(renderer, x1, y1, x2, y2);
                }
                
                // Label centré sous le bouton
                minijeu_draw_text(renderer, color_labels[i], x, y + button_radius + 16.0f, true);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
    SDL_Delay(150);
}

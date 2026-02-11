#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <math.h>
#include "headers/vaisseau.h"

typedef struct {
    float x, y, w, h;
    SDL_Color color;
} Planet;

static float clampf(float v, float a, float b){
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

int vaisseau_run(SDL_Window *window, SDL_Renderer *renderer){
    (void)window;
    const int win_w = 800;
    const int win_h = 600;

    SDL_FRect player = {win_w/2.f - 12.f, win_h/2.f - 12.f, 24.f, 24.f};
    float speed = 200.0f; // pixels per second

    // simple walls (rectangles)
    SDL_FRect walls[4] = {
        {0, 0, (float)win_w, 20},
        {0, (float)win_h-20, (float)win_w, 20},
        {0, 0, 20, (float)win_h},
        {(float)win_w-20, 0, 20, (float)win_h}
    };

    Planet planets[3];
    planets[0] = (Planet){120.f, 120.f, 40.f, 40.f, {200,100,50,255}};
    planets[1] = (Planet){600.f, 140.f, 48.f, 48.f, {50,150,220,255}};
    planets[2] = (Planet){380.f, 420.f, 56.f, 56.f, {120,220,90,255}};

    Uint64 last = SDL_GetTicks();
    bool running = true;
    SDL_Event event;

    while (running){
        Uint64 now = SDL_GetTicks();
        float dt = (now - last) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f;
        last = now;

        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT) {
                return 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN){
                if (event.key.key == SDLK_ESCAPE) return 1;
                if (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER){
                    // check nearest planet
                    for (int i=0;i<3;i++){
                        float cx = planets[i].x + planets[i].w/2.f;
                        float cy = planets[i].y + planets[i].h/2.f;
                        float px = player.x + player.w/2.f;
                        float py = player.y + player.h/2.f;
                        float dist = hypotf(px-cx, py-cy);
                        if (dist < 80.f){
                            // teleport to next planet (circular)
                            int ni = (i+1)%3;
                            player.x = planets[ni].x + planets[ni].w/2.f - player.w/2.f;
                            player.y = planets[ni].y + planets[ni].h/2.f - player.h/2.f;
                            break;
                        }
                    }
                }
            }
        }

        const bool *state = SDL_GetKeyboardState(NULL);
        float dx = 0, dy = 0;
        if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_Z]) dy = -1.f;
        if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) dy = 1.f;
        if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_Q]) dx = -1.f;
        if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) dx = 1.f;
        float len = sqrtf(dx*dx + dy*dy);
        if (len > 0.0001f){ dx /= len; dy /= len; }

        float next_x = player.x + dx * speed * dt;
        float next_y = player.y + dy * speed * dt;

        // clamp to window
        next_x = clampf(next_x, 20.f, win_w - 20.f - player.w);
        next_y = clampf(next_y, 20.f, win_h - 20.f - player.h);

        // collision check with walls
        SDL_FRect test = {next_x, next_y, player.w, player.h};
        bool hit = false;
        for (int i=0;i<4;i++){
            if (!(test.x + test.w < walls[i].x || test.x > walls[i].x + walls[i].w ||
                  test.y + test.h < walls[i].y || test.y > walls[i].y + walls[i].h)){
                hit = true; break;
            }
        }
        if (!hit){ player.x = next_x; player.y = next_y; }

        // render
        SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        // draw walls
        SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
        for (int i=0;i<4;i++){
            SDL_RenderFillRect(renderer, &walls[i]);
        }

        // draw planets
        for (int i=0;i<3;i++){
            SDL_SetRenderDrawColor(renderer, planets[i].color.r, planets[i].color.g, planets[i].color.b, planets[i].color.a);
            SDL_FRect r = {planets[i].x, planets[i].y, planets[i].w, planets[i].h};
            SDL_RenderFillRect(renderer, &r);
        }

        // highlight if near planet
        for (int i=0;i<3;i++){
            float cx = planets[i].x + planets[i].w/2.f;
            float cy = planets[i].y + planets[i].h/2.f;
            float px = player.x + player.w/2.f;
            float py = player.y + player.h/2.f;
            float dist = hypotf(px-cx, py-cy);
            if (dist < 80.f){
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 60);
                SDL_FRect glow = {cx - 60.f, cy - 60.f, 120.f, 120.f};
                SDL_RenderFillRect(renderer, &glow);
            }
        }

        // draw player
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderFillRect(renderer, &player);

        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }

    return 0;
}

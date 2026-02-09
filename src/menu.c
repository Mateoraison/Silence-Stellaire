#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include "headers/main.h"

int afficher_menu(SDL_Window *fenetre, SDL_Renderer *renderer) {

    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/menu.png");
    if(!texture) {
        SDL_Log("erreur chargement texture: %s", SDL_GetError());
        return 1;
    }

    int running = 1;
    int action = 0;

    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                action = 0;
                running = 0;
            } else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                int w, h;
                SDL_GetWindowSize(fenetre, &w, &h);
                float tex_w, tex_h;
                SDL_GetTextureSize(texture, &tex_w, &tex_h);
                float scale = fminf((float)w / tex_w, (float)h / tex_h);
                float scaled_w = tex_w * scale;
                float scaled_h = tex_h * scale;
                float offset_x = (w - scaled_w) / 2;
                float offset_y = (h - scaled_h) / 2;

                float btn_scale = scale;
                SDL_FRect btn_np   = { offset_x + 1226 * btn_scale, offset_y + 580 * btn_scale, 400 * btn_scale, 82 * btn_scale };
                SDL_FRect btn_quit = { offset_x + 1226 * btn_scale, offset_y + 922 * btn_scale, 400 * btn_scale, 82 * btn_scale };

                float mx = (float)event.button.x;
                float my = (float)event.button.y;

                if(mx >= btn_np.x && mx <= btn_np.x + btn_np.w &&
                   my >= btn_np.y && my <= btn_np.y + btn_np.h) {
                    action = 1;     // nouvelle partie
                    running = 0;
                } else if(mx >= btn_quit.x && mx <= btn_quit.x + btn_quit.w &&
                          my >= btn_quit.y && my <= btn_quit.y + btn_quit.h) {
                    action = 0;     // quitter
                    running = 0;
                }else if (event.type == SDL_EVENT_KEY_DOWN) {
                    if (event.key.key == SDLK_ESCAPE) {
                        action = 0; // quitter
                        running = 0;
                    }
                }
            }
        }

        SDL_RenderClear(renderer);

        int w, h;
        SDL_GetWindowSize(fenetre, &w, &h);
        float tex_w, tex_h;
        SDL_GetTextureSize(texture, &tex_w, &tex_h);
        float scale = fminf((float)w / tex_w, (float)h / tex_h);
        float scaled_w = tex_w * scale;
        float scaled_h = tex_h * scale;
        float offset_x = (w - scaled_w) / 2;
        float offset_y = (h - scaled_h) / 2;

        SDL_FRect dest = (SDL_FRect){offset_x, offset_y, scaled_w, scaled_h};
        SDL_RenderTexture(renderer, texture, NULL, &dest);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    return action;
}

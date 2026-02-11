#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>

#define TILE_SIZE 32
#define MAP_W 25  
#define MAP_H 19  
#define TILESET_COLUMNS 10 // Ton image a 10 tuiles par ligne

// 0: Sol, 1: Mur coque, 10: Console orange, 4: Grille aération, 62: Hublot
int tile_map[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,62,0,0,0,1,0,0,0,0,0,1,62,1,0,0,0,0,0,1,0,0,0,62,1},
    {1,0,0,10,0,1,0,20,20,20,0,1,0,1,0,10,10,10,0,1,0,10,0,0,1},
    {1,0,0,0,0,0,0,20,20,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,4,0,0,4,0,0,4,0,0,0,1,0,0,4,0,0,4,0,0,4,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,10,0,10,0,10,0,10,0,0,0,0,0,0,0,0,10,0,10,0,10,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,21,21,21,0,0,21,21,21,0,0,1,0,0,21,21,21,0,0,21,21,21,0,1},
    {1,0,21,21,21,0,0,21,21,21,0,0,1,0,0,21,21,21,0,0,21,21,21,0,1},
    {1,62,0,0,0,0,0,0,0,0,0,1,62,1,0,0,0,0,0,0,0,0,0,62,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void draw_map(SDL_Renderer *renderer, SDL_Texture *tileset) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            int tile_id = tile_map[y][x];

            // Calcul de la position dans le tileset (grille 10x10)
            // Ligne = ID / Colonnes, Colonne = ID % Colonnes
            SDL_FRect srcRect = { 
                (float)(tile_id % TILESET_COLUMNS) * 102.4f, // 1024px / 10 tuiles
                (float)(tile_id / TILESET_COLUMNS) * 102.4f, 
                102.4f, 102.4f 
            };

            SDL_FRect dstRect = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderTexture(renderer, tileset, &srcRect, &dstRect);
        }
    }
}

int vaisseau(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;

    SDL_Window *window = SDL_CreateWindow("Vaisseau Stellaire - Deck 1", 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    
    // Activer le filtrage pour que les tuiles soient nettes
    SDL_SetHint("SDL_RENDER_SCALE_QUALITY", "nearest");

    SDL_Texture *tileset = IMG_LoadTexture(renderer, "assets/tileset/V1/vaisseau.png");
    if (tileset) {
        SDL_SetTextureScaleMode(tileset, SDL_SCALEMODE_NEAREST);
    }

    if (!tileset) {
        SDL_Log("Erreur: %s", SDL_GetError());
        return 1;
    }

    SDL_FRect player = { 400, 300, 20, 20 };
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        // Déplacement simple
        const bool *state = SDL_GetKeyboardState(NULL);
        float next_x = player.x, next_y = player.y;
        if (state[SDL_SCANCODE_W]) next_y -= 3;
        if (state[SDL_SCANCODE_S]) next_y += 3;
        if (state[SDL_SCANCODE_A]) next_x -= 3;
        if (state[SDL_SCANCODE_D]) next_x += 3;

        // Collision très basique avec les murs (ID 1)
        int grid_x = (int)(next_x + 10) / TILE_SIZE;
        int grid_y = (int)(next_y + 10) / TILE_SIZE;
        if (tile_map[grid_y][grid_x] != 1) {
            player.x = next_x;
            player.y = next_y;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_map(renderer, tileset);

        // Dessin du joueur (un petit curseur de vaisseau)
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderFillRect(renderer, &player);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(tileset);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
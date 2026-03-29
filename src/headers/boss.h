#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

void init_boss(SDL_Renderer *renderer, boss_t *boss, float x, float y, int vie_max, int attaque);
void afficher_boss(SDL_Renderer *renderer, boss_t *boss);
void update_boss(SDL_Renderer *renderer, boss_t *boss);
void boss_attaque(SDL_Renderer *renderer, boss_t *boss);
void Destroy_boss(boss_t *boss);

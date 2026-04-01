#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

void init_boss(SDL_Renderer *renderer, boss_t *boss, type_boss_t type, float x, float y);
void afficher_boss(SDL_Renderer *renderer, boss_t *boss);
void mettre_a_jour_boss(SDL_Renderer *renderer, boss_t *boss);
void boss_attaque(SDL_Renderer *renderer, boss_t *boss);
void mettre_a_jour_animation_boss(boss_t *boss);
void detruire_boss(boss_t *boss);
void boss_set_navigation_map(t_tile map[W_MAP][H_MAP]);

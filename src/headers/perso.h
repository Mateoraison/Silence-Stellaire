#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>



int afficher_perso(SDL_Renderer *renderer);
int deplacer_perso(SDL_Event event);
void update_animation();
int test_collision(int x, int y, t_tile map[W_MAP][H_MAP]);


#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>



int afficher_perso(SDL_Renderer *renderer);
int deplacer_perso(float delta_time);
void update_animation();
int test_collision(int x, int y, t_tile map[W_MAP][H_MAP], int est_mob, SDL_Rect hitbox);
int afficher_stat(SDL_Renderer *renderer);
void gerer_combat(SDL_Event event);
void afficher_combat(SDL_Renderer *renderer);
void update_combat(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB], SDL_Renderer * renderer, t_Item * items[MAX_ITEMS]);
void tester_collision_combat(t_tile map[W_MAP][H_MAP], Mob * mobs[MAX_MOB], SDL_Renderer * renderer, t_Item * items[MAX_ITEMS]);
void possible_ramasser_item(t_Item * items[MAX_ITEMS], SDL_Renderer * renderer, t_case * hotbar[HOTBAR_SIZE]);



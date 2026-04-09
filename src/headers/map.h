#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

int afficher_map(SDL_Renderer *renderer, MIX_Track *track_global);
/* release resources created by afficher_map */
void free_map_resources(void);

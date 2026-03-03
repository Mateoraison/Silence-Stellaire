#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#define MAX_ITEMS 200





t_Item * init_item(typeItem type,SDL_Renderer * renderer);
void afficher_item(t_Item * item, float x, float y,SDL_Renderer * renderer);

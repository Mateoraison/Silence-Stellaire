#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>





t_Item * init_item(typeItem type,SDL_Renderer * renderer,float x,float y);
void afficher_item(t_Item * items[MAX_ITEMS],SDL_Renderer * renderer);
void detruire_item(t_Item ** item);
void ramasser_item(t_Item * item, SDL_Renderer * renderer, t_case * hotbar[HOTBAR_SIZE]);
void detruire_tout_item(t_Item * items[MAX_ITEMS]);
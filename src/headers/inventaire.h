#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>


void afficher_hotbar(t_case * hotbar[HOTBAR_SIZE], SDL_Renderer *renderer);
void ajouter_item_hotbar(t_case *hotbar[HOTBAR_SIZE], t_Item *item, SDL_Renderer *renderer);
void afficher_inventaire(t_case *inventaire[], SDL_Renderer *renderer, int inventaire_size, int inventaire_cols, int inventaire_rows);
void ajouter_item_inventaire(t_case **inventaire, int inventaire_size, t_Item *item);

void gerer_clic_inventaire(t_case **inventaire, t_case **hotbar, SDL_Event *event, int inventaire_size, int inventaire_cols, int inventaire_rows);
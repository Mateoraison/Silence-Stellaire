#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>


#define MAX_MOB 100

typedef struct {
    int x;
    int y;
    int direction;
    int vitesse_x;
    int vitesse_y;
    float largeur,hauteur;
    Uint32 time_change_dir;
    SDL_Texture * texture;
    int vie;
}Mob;

extern Mob mobs[MAX_MOB];
extern int nb_mobs;

void init_mobs(SDL_Renderer * renderer);
void update_mobs();
void afficher_mob(SDL_Renderer * renderer);
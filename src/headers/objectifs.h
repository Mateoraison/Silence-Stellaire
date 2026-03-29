#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#define MAX_OBJECTIFS 8


typedef struct {
    char * description;
    bool valide;
    int  (*verifier)(void);
}t_objectif;


typedef struct {
    t_objectif objectifs[MAX_OBJECTIFS];
    int nb;
    
    bool visible;

    int          anim_slot;
    Uint32       anim_debut;
}t_objectifs;



void objectifs_init(t_objectifs *obj, int planete);
void objectifs_afficher(t_objectifs *obj, SDL_Renderer *renderer, TTF_Font *font);
void objectif_valider(t_objectifs *obj, int index);
void objectifs_visible(t_objectifs *obj);
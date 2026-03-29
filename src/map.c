#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "headers/main.h"



typedef struct{
    IMG_Animation * anim;
    Uint32 dernier_frame;
    int frame_actuelle;
    float x,y,echelle;
}t_Animation;


t_Animation animation_init(char * chemin, float x , float y, float echelle){
    t_Animation a = {0};
    a.anim = IMG_LoadAnimation(chemin);
    if(!a.anim){
        SDL_Log("Erreur animation GIF: %s", SDL_GetError());
        return a;
    }
    a.dernier_frame = SDL_GetTicks();
    a.frame_actuelle = 0;
    a.x = x;
    a.y = y;
    a.echelle = echelle;
    return a;
}

void animation_update(t_Animation * a){
    Uint32 maintenant = SDL_GetTicks();
    if(maintenant - a->dernier_frame >= (Uint32)a->anim->delays[a->frame_actuelle]){
            a->frame_actuelle = (a->frame_actuelle + 1) % a->anim->count;
            a->dernier_frame = maintenant;
        }
}

void afficher_animation(SDL_Renderer * renderer, t_Animation *a){
    SDL_Texture *frame_texture = SDL_CreateTextureFromSurface(renderer,a->anim->frames[a->frame_actuelle]);
        if(frame_texture){
            SDL_FRect dest = {
                .x = a->x,
                .y = a->y,
                .w = a->anim->w * a->echelle,
                .h = a->anim->h * a->echelle

            };
            SDL_RenderTexture(renderer,frame_texture,NULL,&dest);
            SDL_DestroyTexture(frame_texture);
        }
}

void detruire_animation(t_Animation * a){
    if(a->anim){
        IMG_FreeAnimation(a->anim);
        a->anim = NULL;
    }
}


int afficher_map(SDL_Renderer *renderer) {
    update_screen_metrics(renderer);
    float offx = screen_center_x() - 500.0f;
    float offy = screen_center_y() - 400.0f;

    TTF_Font *font = TTF_OpenFont("assets/police.ttf", 24);
    if (!font) {
        SDL_Log("Erreur chargement police %s, %s", "assets/police.ttf", SDL_GetError());
        TTF_Quit();
        return 1;
    }

    TTF_Font *font_titre = TTF_OpenFont("assets/police_titre.ttf", 24);
    if (!font_titre) {
        SDL_Log("Erreur chargement police %s, %s", "assets/police_titre.ttf", SDL_GetError());
        TTF_Quit();
        return 1;
    }
    
    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/carte_espace/fond_map.png");
    if (!texture) {
        SDL_Log("erreur chargement texture %s", SDL_GetError());
        TTF_CloseFont(font);
        TTF_Quit();
        return 1;
    }
    
    SDL_Color blanc = {255, 255, 255, 255};
    

    t_Animation planete1 = animation_init("assets/carte_espace/planete1_map.gif",100 + offx,200 + offy,1.5f);
    t_Animation planete2 = animation_init("assets/carte_espace/planete2_map.gif",300 + offx,500 + offy,1.5f);
    t_Animation planete3 = animation_init("assets/carte_espace/planete3_map.gif",500 + offx,200 + offy,1.5f);
    t_Animation planete4 = animation_init("assets/carte_espace/planete4_map.gif",700 + offx,500 + offy,1.5f);
    Bouton P1;
    Bouton P2;
    Bouton P3;
    Bouton P4;
    Bouton_Init(&P1,planete1.x,planete1.y,planete1.anim->w,planete1.anim->h,NULL);
    Bouton_Init(&P2,planete2.x,planete2.y,planete2.anim->w,planete2.anim->h,NULL);
    Bouton_Init(&P3,planete3.x,planete3.y,planete3.anim->w,planete3.anim->h,NULL);
    Bouton_Init(&P4,planete4.x,planete4.y,planete4.anim->w,planete4.anim->h,NULL);
    


    SDL_Texture * asteroid1 = IMG_LoadTexture(renderer,"assets/carte_espace/asteroid1.png");
    SDL_FRect dest_asteroid1 = {100 + offx,700 + offy,100,100};
    SDL_Texture * asteroid2 = IMG_LoadTexture(renderer,"assets/carte_espace/asteroid2.png");
    SDL_FRect dest_asteroid2 = {300 + offx,200 + offy,100,100};
    SDL_Texture * asteroid3 = IMG_LoadTexture(renderer,"assets/carte_espace/asteroid3.png");
    SDL_FRect dest_asteroid3 = {800 + offx,300 + offy,100,100};
    SDL_Texture * asteroid4 = IMG_LoadTexture(renderer,"assets/carte_espace/asteroid4.png");
    SDL_FRect dest_asteroid4 = {600 + offx,600 + offy,100,100};


    int running = 1;
    int planete_choisie = 0;
    while (running){
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_MOUSE_MOTION ||
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                SDL_ConvertEventToRenderCoordinates(renderer, &event);
            }

            if(event.type == SDL_EVENT_QUIT){
                running = 0;
            }
            if(event.type == SDL_EVENT_KEY_DOWN){
                if( event.key.key == SDLK_ESCAPE){
                    running = 0;
                }
            }
            if(Bouton_GererEvenement(&P1,&event)){
                SDL_Log("planete 1 cliquer");
                planete_choisie = 1;
                running = 0;
            }
            if(Bouton_GererEvenement(&P2,&event)){
                SDL_Log("planete 2 cliquer");
                planete_choisie = 2;
                running = 0;
            }
            if(Bouton_GererEvenement(&P3,&event)){
                SDL_Log("planete 3 cliquer");
                planete_choisie = 3;
                running = 0;
            }
            if(Bouton_GererEvenement(&P4,&event)){
                SDL_Log("planete 4 cliquer");
            }
            
        }
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);


        SDL_Surface *surface_titre = TTF_RenderText_Solid(font_titre, "Cartes des planetes", strlen("Cartes des planetes"), blanc);
        if (!surface_titre) SDL_Log("Erreur texte: %s", SDL_GetError());
        SDL_Texture *texture_titre = SDL_CreateTextureFromSurface(renderer, surface_titre);
        if (!texture_titre) SDL_Log("Erreur texture: %s", SDL_GetError());
        SDL_FRect rect_titre = {.x = screen_center_x() - surface_titre->w/2,.y = 50,.w = surface_titre->w,.h = surface_titre->h};
        

        SDL_Surface * nom_P1 = TTF_RenderText_Solid(font,"Planete du crash", strlen("Planete du crash"),blanc);
        SDL_Texture *nom_P1_tex = SDL_CreateTextureFromSurface(renderer, nom_P1);
        SDL_FRect rect_nom_P1 = {.x = planete1.x-20 ,.y = planete1.y - 40,.w = nom_P1->w,.h = nom_P1->h};

        SDL_Surface * nom_P2 = TTF_RenderText_Solid(font,"Planete de Lave", strlen("Planete de Lave"),blanc);
        SDL_Texture *nom_P2_tex = SDL_CreateTextureFromSurface(renderer, nom_P2);
        SDL_FRect rect_nom_P2 = {.x = planete2.x-30 ,.y = planete2.y + 150,.w = nom_P2->w,.h = nom_P2->h};

        SDL_Surface * nom_P3 = TTF_RenderText_Solid(font,"Planete de Glace", strlen("Planete de Glace"),blanc);
        SDL_Texture *nom_P3_tex = SDL_CreateTextureFromSurface(renderer, nom_P3);
        SDL_FRect rect_nom_P3 = {.x = planete3.x + 150 ,.y = planete3.y,.w = nom_P3->w,.h = nom_P3->h};

        SDL_Surface * nom_P4 = TTF_RenderText_Solid(font,"Galaxie", strlen("Galaxie"),blanc);
        SDL_Texture *nom_P4_tex = SDL_CreateTextureFromSurface(renderer, nom_P4);
        SDL_FRect rect_nom_P4 = {.x = planete4.x +30  ,.y = planete4.y + 130,.w = nom_P4->w,.h = nom_P4->h};

        SDL_SetRenderDrawColor(renderer, 255, 255, 200, 200);
        SDL_RenderLine(renderer, planete1.x + planete1.anim->w/2, planete1.y + planete1.anim->h/2,planete2.x + planete2.anim->w/2, planete2.y + planete2.anim->h/2);
        SDL_RenderLine(renderer, planete2.x + planete2.anim->w/2, planete2.y + planete2.anim->h/2,planete3.x + planete3.anim->w/2, planete3.y + planete3.anim->h/2);
        SDL_RenderLine(renderer, planete3.x + planete3.anim->w/2, planete3.y + planete3.anim->h/2,planete4.x + planete3.anim->w/2, planete4.y + planete4.anim->h/2);


        animation_update(&planete1);
        animation_update(&planete2);
        animation_update(&planete3);
        animation_update(&planete4);
        afficher_animation(renderer,&planete1);
        afficher_animation(renderer,&planete2);
        afficher_animation(renderer,&planete3);
        afficher_animation(renderer,&planete4);
    
        
        
        SDL_RenderTexture(renderer,asteroid1,NULL,&dest_asteroid1);
        SDL_RenderTexture(renderer,asteroid2,NULL,&dest_asteroid2);
        SDL_RenderTexture(renderer,asteroid3,NULL,&dest_asteroid3);
        SDL_RenderTexture(renderer,asteroid4,NULL,&dest_asteroid4);


        SDL_RenderTexture(renderer, texture_titre, NULL, &rect_titre);
        SDL_DestroyTexture(texture_titre);
        SDL_DestroySurface(surface_titre);

        SDL_RenderTexture(renderer, nom_P1_tex, NULL, &rect_nom_P1);
        SDL_DestroyTexture(nom_P1_tex);
        SDL_DestroySurface(nom_P1);

        SDL_RenderTexture(renderer, nom_P2_tex, NULL, &rect_nom_P2);
        SDL_DestroyTexture(nom_P2_tex);
        SDL_DestroySurface(nom_P2);

        SDL_RenderTexture(renderer, nom_P3_tex, NULL, &rect_nom_P3);
        SDL_DestroyTexture(nom_P3_tex);
        SDL_DestroySurface(nom_P3);
        
        SDL_RenderTexture(renderer, nom_P4_tex, NULL, &rect_nom_P4);
        SDL_DestroyTexture(nom_P4_tex);
        SDL_DestroySurface(nom_P4);

        SDL_RenderPresent(renderer);
    }
    
    detruire_animation(&planete1);
    detruire_animation(&planete2);
    detruire_animation(&planete3);
    detruire_animation(&planete4);
    SDL_DestroyTexture(asteroid1);
    SDL_DestroyTexture(asteroid2);
    SDL_DestroyTexture(asteroid3);
    SDL_DestroyTexture(asteroid4);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font_titre);
    TTF_CloseFont(font);
    return planete_choisie;
}


    
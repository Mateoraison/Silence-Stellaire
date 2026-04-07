#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <limits.h>
#include "headers/main.h"



typedef struct{
    IMG_Animation * anim;
    Uint32 dernier_frame;
    int frame_actuelle;
    float x,y,echelle;
}t_Animation;

static int compter_engrenages_slots(t_case *slots[], int size) {
    int total = 0;
    for (int i = 0; i < size; i++) {
        if (slots[i] && slots[i]->item && slots[i]->item->type == ENGRENAGE) {
            total += slots[i]->quantiter;
        }
    }
    return total;
}

static int compter_engrenages_totaux(void) {
    return compter_engrenages_slots(hotbar, HOTBAR_SIZE)
        + compter_engrenages_slots(inventaire, INVENTAIRE_SIZE)
        + compter_engrenages_slots(caisse_outils, CAISSE_OUTILS_SIZE);
}

static int transition_planete_autorisee(int planete_depart, int planete_arrivee, int nb_engrenages) {
    if (planete_arrivee > planete_depart + 1) {
        return 0;
    }

    if ((planete_depart == 2 && planete_arrivee == 3) ||
        (planete_depart == 3 && planete_arrivee == 4)) {
        return nb_engrenages >= 2;
    }
    return 1;
}

static const char *message_transition_bloquee(int planete_depart, int planete_arrivee) {
    if (planete_arrivee > planete_depart + 1) {
        return "Passe d'abord par la planete précédente";
    }
    if (planete_depart == 2 && planete_arrivee == 3) {
        return "Il faut 2 engrenages pour passer a la planete suivante";
    }
    if (planete_depart == 3 && planete_arrivee == 4) {
        return "Il faut 2 engrenages pour passer a la planete suivante";
    }
    return "Transition non autorisee";
}


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
    if (!a || !a->anim) {
        return;
    }
    Uint32 maintenant = SDL_GetTicks();
    if(maintenant - a->dernier_frame >= (Uint32)a->anim->delays[a->frame_actuelle]){
            a->frame_actuelle = (a->frame_actuelle + 1) % a->anim->count;
            a->dernier_frame = maintenant;
        }
}

void afficher_animation(SDL_Renderer * renderer, t_Animation *a){
    if (!a || !a->anim) {
        return;
    }
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
        return 1;
    }

    TTF_Font *font_titre = TTF_OpenFont("assets/police_titre.ttf", 24);
    if (!font_titre) {
        SDL_Log("Erreur chargement police %s, %s", "assets/police_titre.ttf", SDL_GetError());
        TTF_CloseFont(font);
        return 1;
    }
    
    SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/carte_espace/fond_map.png");
    if (!texture) {
        SDL_Log("erreur chargement texture %s", SDL_GetError());
        TTF_CloseFont(font_titre);
        TTF_CloseFont(font);
        return 1;
    }
    
    SDL_Color blanc = {255, 255, 255, 255};
    

    t_Animation planete1 = animation_init("assets/carte_espace/planete1_map.gif",100 + offx,200 + offy,1.5f);
    t_Animation planete2 = animation_init("assets/carte_espace/planete2_map.gif",300 + offx,500 + offy,1.5f);
    t_Animation planete3 = animation_init("assets/carte_espace/planete3_map.gif",400 + offx,20 + offy,1.5f);
    t_Animation planete4 = animation_init("assets/carte_espace/planete4_map.gif",700 + offx,500 + offy,1.5f);
    if (!planete1.anim || !planete2.anim || !planete3.anim || !planete4.anim) {
        detruire_animation(&planete1);
        detruire_animation(&planete2);
        detruire_animation(&planete3);
        detruire_animation(&planete4);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font_titre);
        TTF_CloseFont(font);
        return 1;
    }
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
    SDL_Texture *engrenage_tex = IMG_LoadTexture(renderer, "assets/UI/engrenage.png");
    if (engrenage_tex) {
        SDL_SetTextureScaleMode(engrenage_tex, SDL_SCALEMODE_NEAREST);
    }


    int running = 1;
    int planete_choisie = 0;
    char message_info[128] = "";
    Uint32 message_info_expire = 0;
    while (running){
        int nb_engrenages = compter_engrenages_totaux();
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
                if (transition_planete_autorisee(Planete_actuelle, 3, nb_engrenages)) {
                    planete_choisie = 3;
                    running = 0;
                } else {
                    snprintf(message_info, sizeof(message_info), "%s", message_transition_bloquee(Planete_actuelle, 3));
                    message_info_expire = SDL_GetTicks() + 2500;
                }
            }
            if(Bouton_GererEvenement(&P4,&event)){
                SDL_Log("planete 4 cliquer");
                if (transition_planete_autorisee(Planete_actuelle, 4, nb_engrenages)) {
                    // Jouer la cinematique de fin puis les credits, puis retourner au menu principal
                    int code = jouer_cinematique_fin(renderer);
                    if (code == 1) { running = 0; break; }
                    // si la cinematique s'est terminee, afficher les credits
                    int code_credits = jouer_credits(renderer);
                    (void)code_credits;
                    // signaler au main de retourner au menu principal
                    planete_choisie = -1;
                    running = 0;
                } else {
                    snprintf(message_info, sizeof(message_info), "%s", message_transition_bloquee(Planete_actuelle, 4));
                    message_info_expire = SDL_GetTicks() + 2500;
                }
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

        SDL_Surface * nom_P3 = TTF_RenderText_Solid(font,"Planete du Minautor", strlen("Planete du Minautore"),blanc);
        SDL_Texture *nom_P3_tex = SDL_CreateTextureFromSurface(renderer, nom_P3);
        SDL_FRect rect_nom_P3 = {.x = planete3.x + 150 ,.y = planete3.y,.w = nom_P3->w,.h = nom_P3->h};

        SDL_Surface * nom_P4 = TTF_RenderText_Solid(font,"Galaxie", strlen("Galaxie"),blanc);
        SDL_Texture *nom_P4_tex = SDL_CreateTextureFromSurface(renderer, nom_P4);
        SDL_FRect rect_nom_P4 = {.x = planete4.x +30  ,.y = planete4.y + 130,.w = nom_P4->w,.h = nom_P4->h};

        SDL_Color vert = {120, 230, 140, 255};
        SDL_Color rouge = {235, 100, 100, 255};
        SDL_Color couleur_requis = (nb_engrenages >= 2) ? vert : rouge;

        char texte_regle_23[80];
        snprintf(texte_regle_23, sizeof(texte_regle_23), "Planete de Lave -> Planete du Minautor");
        SDL_Surface *regle_23 = TTF_RenderText_Solid(font, texte_regle_23, strlen(texte_regle_23), couleur_requis);
        SDL_Texture *regle_23_tex = SDL_CreateTextureFromSurface(renderer, regle_23);

        char texte_regle_34[80];
        snprintf(texte_regle_34, sizeof(texte_regle_34), "Planete du Minautor -> Galaxie");
        SDL_Surface *regle_34 = TTF_RenderText_Solid(font, texte_regle_34, strlen(texte_regle_34), couleur_requis);
        SDL_Texture *regle_34_tex = SDL_CreateTextureFromSurface(renderer, regle_34);

        char texte_stock[80];
        snprintf(texte_stock, sizeof(texte_stock), "Engrenages en stock: %d", nb_engrenages);
        SDL_Surface *stock = TTF_RenderText_Solid(font, texte_stock, strlen(texte_stock), couleur_requis);
        SDL_Texture *stock_tex = SDL_CreateTextureFromSurface(renderer, stock);

        const float panneau_x = 12.0f;
        const float panneau_y = 12.0f;
        const float panneau_padding = 10.0f;
        const float ligne_h = 30.0f;
        const float engrenage_size = 20.0f;
        const float engrenage_gap = 6.0f;
        const float groupe_2_engrenages_w = (engrenage_size * 2.0f) + engrenage_gap;

        float largeur_texte_max = (float)regle_23->w;
        if ((float)regle_34->w > largeur_texte_max) largeur_texte_max = (float)regle_34->w;
        if ((float)stock->w > largeur_texte_max) largeur_texte_max = (float)stock->w;

        float panneau_w = panneau_padding + largeur_texte_max + 14.0f + groupe_2_engrenages_w + panneau_padding;
        float panneau_h = panneau_padding + (ligne_h * 3.0f) + panneau_padding;

        SDL_FRect panneau_regles = {panneau_x, panneau_y, panneau_w, panneau_h};

        SDL_FRect rect_regle_23 = {
            .x = panneau_x + panneau_padding,
            .y = panneau_y + panneau_padding,
            .w = (float)regle_23->w,
            .h = (float)regle_23->h
        };
        SDL_FRect rect_regle_34 = {
            .x = panneau_x + panneau_padding,
            .y = rect_regle_23.y + ligne_h,
            .w = (float)regle_34->w,
            .h = (float)regle_34->h
        };
        SDL_FRect rect_stock = {
            .x = panneau_x + panneau_padding,
            .y = rect_regle_34.y + ligne_h,
            .w = (float)stock->w,
            .h = (float)stock->h
        };

        float engrenages_x = panneau_x + panneau_w - panneau_padding - groupe_2_engrenages_w;
        SDL_FRect g23_1 = {engrenages_x, rect_regle_23.y - 1.0f, engrenage_size, engrenage_size};
        SDL_FRect g23_2 = {engrenages_x + engrenage_size + engrenage_gap, rect_regle_23.y - 1.0f, engrenage_size, engrenage_size};
        SDL_FRect g34_1 = {engrenages_x, rect_regle_34.y - 1.0f, engrenage_size, engrenage_size};
        SDL_FRect g34_2 = {engrenages_x + engrenage_size + engrenage_gap, rect_regle_34.y - 1.0f, engrenage_size, engrenage_size};
        SDL_FRect g_stock = {engrenages_x + engrenage_size + (engrenage_gap * 0.5f), rect_stock.y - 1.0f, engrenage_size, engrenage_size};
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 8, 18, 30, 180);
        SDL_RenderFillRect(renderer, &panneau_regles);
        SDL_SetRenderDrawColor(renderer, 120, 180, 220, 180);
        SDL_RenderRect(renderer, &panneau_regles);

        SDL_SetRenderDrawColor(renderer, 255, 255, 200, 200);
        SDL_RenderLine(renderer, planete1.x + planete1.anim->w/2, planete1.y + planete1.anim->h/2,planete2.x + planete2.anim->w/2, planete2.y + planete2.anim->h/2);
        SDL_RenderLine(renderer, planete2.x + planete2.anim->w/2, planete2.y + planete2.anim->h/2,planete3.x + planete3.anim->w/2+100, planete3.y+100 + planete3.anim->h/2);
        SDL_RenderLine(renderer, planete3.x+20 + planete3.anim->w/2, planete3.y+100 + planete3.anim->h/2,planete4.x + planete4.anim->w/2, planete4.y + planete4.anim->h/2);


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

        SDL_RenderTexture(renderer, regle_23_tex, NULL, &rect_regle_23);
        SDL_DestroyTexture(regle_23_tex);
        SDL_DestroySurface(regle_23);

        if (engrenage_tex) {
            SDL_RenderTexture(renderer, engrenage_tex, NULL, &g23_1);
            SDL_RenderTexture(renderer, engrenage_tex, NULL, &g23_2);
        }

        SDL_RenderTexture(renderer, regle_34_tex, NULL, &rect_regle_34);
        SDL_DestroyTexture(regle_34_tex);
        SDL_DestroySurface(regle_34);

        if (engrenage_tex) {
            SDL_RenderTexture(renderer, engrenage_tex, NULL, &g34_1);
            SDL_RenderTexture(renderer, engrenage_tex, NULL, &g34_2);
        }

        SDL_RenderTexture(renderer, stock_tex, NULL, &rect_stock);
        SDL_DestroyTexture(stock_tex);
        SDL_DestroySurface(stock);

        if (engrenage_tex) {
            SDL_RenderTexture(renderer, engrenage_tex, NULL, &g_stock);
        }

        if (message_info[0] != '\0' && SDL_GetTicks() < message_info_expire) {
            SDL_Surface *msg = TTF_RenderText_Solid(font, message_info, strlen(message_info), blanc);
            if (msg) {
                SDL_Texture *msg_tex = SDL_CreateTextureFromSurface(renderer, msg);
                SDL_FRect rect_msg = {.x = panneau_x + panneau_padding, .y = panneau_y + panneau_h + 8.0f, .w = (float)msg->w, .h = (float)msg->h};
                SDL_RenderTexture(renderer, msg_tex, NULL, &rect_msg);
                SDL_DestroyTexture(msg_tex);
                SDL_DestroySurface(msg);
            }
        }

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
    SDL_DestroyTexture(engrenage_tex);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font_titre);
    TTF_CloseFont(font);
    return planete_choisie;
}






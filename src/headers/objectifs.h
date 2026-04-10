/**
 * @file objectifs.h
 * @brief Fichier de suivi des objectifs de quete et de leur affichage.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#define MAX_OBJECTIFS 8


/**
 * @struct t_objectif
 * @brief Unite d'objectif individuelle (description, etat valide, predicate de verification).
 */
typedef struct {
    char * description;
    bool valide;
    int  (*verifier)(void);
}t_objectif;


/**
 * @struct t_objectifs
 * @brief Ensemble des objectifs actifs, avec etat d'affichage et animation de validation.
 */
typedef struct {
    t_objectif objectifs[MAX_OBJECTIFS];
    int nb;
    
    bool visible;

    int          anim_slot;
    Uint32       anim_debut;
}t_objectifs;



/**
 * @fn void objectifs_init(t_objectifs *obj, int planete)
 * @brief Initialise la liste des objectifs selon la planete en cours.
 * @param obj Structure des objectifs a consulter ou modifier.
 * @param planete Identifiant de planete concernee par l'operation.
 */
void objectifs_init(t_objectifs *obj, int planete);
/**
 * @fn void objectifs_afficher(t_objectifs *obj, SDL_Renderer *renderer, TTF_Font *font)
 * @brief Affiche le panneau des objectifs et met a jour leur etat de validation.
 * @param obj Structure des objectifs a consulter ou modifier.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param font Police TTF employee pour dessiner les textes de l'interface.
 */
void objectifs_afficher(t_objectifs *obj, SDL_Renderer *renderer, TTF_Font *font);
/**
 * @fn void objectif_valider(t_objectifs *obj, int index)
 * @brief Force la validation d'un objectif et declenche son animation visuelle.
 * @param obj Structure des objectifs a consulter ou modifier.
 * @param index Indice de l'element vise dans la collection concernee.
 */
void objectif_valider(t_objectifs *obj, int index);
/**
 * @fn void objectifs_visible(t_objectifs *obj)
 * @brief Bascule la visibilite du panneau d'objectifs.
 * @param obj Structure des objectifs a consulter ou modifier.
 */
void objectifs_visible(t_objectifs *obj);

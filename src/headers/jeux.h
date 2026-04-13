/**
 * @file jeux.h
 * @brief Fichier de la boucle de jeu principale et de son initialisation.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

typedef struct {
	int planete2_mastermind_engrenage_donne;
	int planete2_simon_termine;
	int planete1_engrenage_objectifs_donne;
	int planete2_engrenage_objectifs_donne;
	int planete3_engrenage_objectifs_donne;
	int planete2_barriere4_ouverte;
	int planete3_engrenage_recupere;
	int planete3_boss_spawned;
	int mastermind_reussi;
	int simon_reussi;
} progression_jeu_t;

/**
 * @brief Execute la boucle principale de gameplay pour la planete courante.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param planete Identifiant de planete concernee par l'operation.
 * @param track_global Piste audio globale (musique d'ambiance) partagee entre ecrans.
 * @param reprendre_partie Indique s'il faut relancer une partie depuis une sauvegarde.
 * @return 0 si la boucle se termine normalement, 1 en cas de retour menu/fermeture ou erreur de chargement.
 */
int jeu_principal(SDL_Renderer *renderer, int planete, MIX_Track *track_global, bool reprendre_partie);
/**
 * @brief Initialise la caisse a outils et prepare les ressources necessaires.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 */
void init_caisse_outils(SDL_Renderer *renderer);

/**
 * @brief Recupere l'etat de progression global du gameplay.
 * @param out Structure de sortie a remplir.
 */
void jeu_get_progression(progression_jeu_t *out);

/**
 * @brief Applique un etat de progression global au gameplay.
 * @param in Structure source a appliquer.
 */
void jeu_set_progression(const progression_jeu_t *in);

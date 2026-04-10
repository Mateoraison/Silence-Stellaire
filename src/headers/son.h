/**
 * @file son.h
 * @brief Fichier audio: musique, effets sonores et etats d'activation.
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>



/**
 * @brief Charge (ou recupere du cache) puis joue une piste audio.
 * @param chemin Chemin du fichier audio a lire (SFX ou ambiance).
 * @param volume Gain applique a la piste, typiquement entre 0.0f et 1.0f.
 * @return Piste MIX creee/reutilisee, ou NULL si le chargement/lecture echoue.
 */
MIX_Track * jouer_son(const char* chemin, float volume);

/**
 * @brief Met en pause une piste audio si elle est valide.
 * @param track Piste cible a mettre en pause.
 */
void pause_son(MIX_Track *track);

/**
 * @brief Reprend une piste si le son global et la categorie associee sont actifs.
 * @param track Piste cible a reprendre.
 */
void reprendre_son(MIX_Track *track);

/**
 * @brief Indique si l'audio global du jeu est active.
 * @return 1 si le son global est actif, 0 sinon.
 */
int son_est_actif(void);

/**
 * @brief Active ou desactive l'audio global (musiques + SFX) et applique l'etat aux pistes chargees.
 * @param actif Etat cible (0 = coupe, non-zero = actif).
 */
void son_definir_actif(int actif);

/**
 * @brief Indique si la categorie ambiance/musique est active.
 * @return 1 si les ambiances sont actives, 0 sinon.
 */
int son_ambiance_est_active(void);

/**
 * @brief Active ou desactive uniquement les pistes d'ambiance.
 * @param actif Etat cible (0 = coupe, non-zero = actif).
 */
void son_definir_ambiance_active(int actif);

/**
 * @brief Indique si la categorie SFX est active.
 * @return 1 si les effets sonores sont actifs, 0 sinon.
 */
int son_sfx_est_actif(void);

/**
 * @brief Active ou desactive uniquement les effets sonores (SFX).
 * @param actif Etat cible (0 = coupe, non-zero = actif).
 */
void son_definir_sfx_active(int actif);

/**
 * @brief Precharge les effets sonores frequents dans le cache audio.
 */
void son_precharger_sfx(void);

/**
 * @brief Met en pause toutes les pistes actuellement chargees.
 */
void son_pause_tout(void);

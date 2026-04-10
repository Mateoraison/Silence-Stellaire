/**
 * @file sauvegarde.h
 * @brief Fichier des slots de sauvegarde et du chargement de progression.
 */

#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define NB_SLOTS_SAUVEGARDE 3

/**
 * @brief Verifie si un fichier de sauvegarde existe pour un slot donne.
 * @param slot Indice du slot de sauvegarde cible.
 * @return 1 si le slot existe, 0 sinon.
 */
int sauvegarde_slot_existe(int slot);
/**
 * @brief Affiche l'interface de selection de slot (chargement ou sauvegarde).
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @param titre Texte affiche en titre de la fenetre (ex: "Charger" / "Sauvegarder").
 * @param mode_chargement true pour charger un slot existant, false pour ecraser/creer.
 * @return Numero de slot choisi (1..NB_SLOTS_SAUVEGARDE), ou 0 si annulation.
 */
int sauvegarde_choisir_slot(SDL_Renderer *renderer, const char *titre, bool mode_chargement);

/**
 * @brief Serialize l'etat de partie courant dans le slot cible.
 * @param slot Indice du slot de sauvegarde cible.
 * @param planete Identifiant de planete concernee par l'operation.
 * @return 0 en cas de succes, -1 en cas d'erreur E/S ou de slot invalide.
 */
int sauvegarder_partie_slot(int slot, int planete);
/**
 * @brief Charge les donnees brutes d'un slot et les place en attente d'application.
 * @param slot Indice du slot de sauvegarde cible.
 * @param planete_out Pointeur de sortie optionnel pour recuperer la planete sauvegardee.
 * @return 0 en cas de succes, -1 en cas d'erreur ou de slot invalide.
 */
int charger_partie_slot(int slot, int *planete_out);

/**
 * @brief Retourne le slot marque comme actif dans la session courante.
 * @return Numero du slot actif.
 */
int sauvegarde_get_slot_actif(void);
/**
 * @brief Definit le slot actif pour les operations ulterieures.
 * @param slot Indice du slot de sauvegarde cible.
 */
void sauvegarde_set_slot_actif(int slot);

/* Applique une sauvegarde chargee en attente sur l'etat global du jeu. */
/**
 * @brief Applique a l'etat global les donnees precedemment chargees si presentes.
 * @param renderer Contexte SDL utilise pour dessiner les elements 2D et l'interface.
 * @return 1 si une sauvegarde en attente a ete appliquee, 0 sinon.
 */
int sauvegarde_appliquer_si_disponible(SDL_Renderer *renderer);

#endif

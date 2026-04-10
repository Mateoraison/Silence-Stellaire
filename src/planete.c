/**
 * @file planete.c
 * @brief Gestion des planetes et des transitions d'exploration.
 */

#include <SDL3/SDL.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include "headers/planete.h"

static int p3_dans_limites(int x, int y)
{
    return x >= 0 && y >= 0 && x < W_MAP && y < H_MAP;
}

static void p3_set_tile(t_tile map[W_MAP][H_MAP], int x, int y, type_t type)
{
    if (!p3_dans_limites(x, y))
    {
        return;
    }
    map[x][y].type = type;
    map[x][y].width = DISPLAY_TILE_SIZE;
    map[x][y].height = DISPLAY_TILE_SIZE;
}

static void p3_fill_map(t_tile map[W_MAP][H_MAP], type_t type)
{
    for (int x = 0; x < W_MAP; x++)
    {
        for (int y = 0; y < H_MAP; y++)
        {
            p3_set_tile(map, x, y, type);
        }
    }
}

static void p3_open_room(t_tile map[W_MAP][H_MAP], int rx, int ry, int rw, int rh)
{
    for (int x = rx; x < rx + rw; x++)
    {
        for (int y = ry; y < ry + rh; y++)
        {
            p3_set_tile(map, x, y, terreP);
        }
    }
}

static void p3_build_wall_ring(t_tile map[W_MAP][H_MAP], int rx, int ry, int rw, int rh)
{
    for (int x = rx - 1; x <= rx + rw; x++)
    {
        p3_set_tile(map, x, ry - 1, eau);
        p3_set_tile(map, x, ry + rh, eau);
    }
    for (int y = ry - 1; y <= ry + rh; y++)
    {
        p3_set_tile(map, rx - 1, y, eau);
        p3_set_tile(map, rx + rw, y, eau);
    }
}

static void p3_carve_line(t_tile map[W_MAP][H_MAP], int x0, int y0, int x1, int y1)
{
    int x = x0;
    int y = y0;
    while (x != x1)
    {
        p3_set_tile(map, x, y, terreP);
        x += (x1 > x) ? 1 : -1;
    }
    while (y != y1)
    {
        p3_set_tile(map, x, y, terreP);
        y += (y1 > y) ? 1 : -1;
    }
    p3_set_tile(map, x, y, terreP);
}


/** @brief Cellule de file utilisee pour le remplissage de zone de la planete 3. */
typedef struct
{
    int x;
    int y;
} p3_cell_t;

static void p3_remplissage_zone(t_tile map[W_MAP][H_MAP], unsigned char visite[W_MAP * H_MAP], int sx, int sy)
{
    int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    p3_cell_t *queue = malloc(sizeof(p3_cell_t) * W_MAP * H_MAP);
    int front = 0;
    int rear = 0;

    if (!queue)
    {
        return;
    }

    queue[rear++] = (p3_cell_t){sx, sy};
    visite[sx + sy * W_MAP] = 1;

    while (front < rear)
    {
        p3_cell_t cur = queue[front++];

        for (int i = 0; i < 4; i++)
        {
            int nx = cur.x + dirs[i][0];
            int ny = cur.y + dirs[i][1];

            if (!p3_dans_limites(nx, ny) || visite[nx + ny * W_MAP])
            {
                continue;
            }
            if (map[nx][ny].type != terreP)
            {
                continue;
            }

            visite[nx + ny * W_MAP] = 1;
            queue[rear++] = (p3_cell_t){nx, ny};
        }
    }

    free(queue);
}

static void p3_generer_labyrinthe_dfs(t_tile map[W_MAP][H_MAP], unsigned char *visited, int start_x, int start_y)
{
    int dirs[4][2] = {{0, -2}, {2, 0}, {0, 2}, {-2, 0}}; // Haut, Droite, Bas, Gauche

    int max_cells = ((W_MAP - 1) / 2) * ((H_MAP - 1) / 2);
    p3_cell_t *stack = malloc(sizeof(p3_cell_t) * max_cells);
    if (!stack)
        return;

    int top = 0;
    stack[top++] = (p3_cell_t){start_x, start_y};
    visited[start_x + start_y * W_MAP] = 1;
    p3_set_tile(map, start_x, start_y, terreP);

    while (top > 0)
    {
        p3_cell_t cur = stack[top - 1];

        int shuffled_dirs[4][2];
        for (int i = 0; i < 4; i++)
        {
            shuffled_dirs[i][0] = dirs[i][0];
            shuffled_dirs[i][1] = dirs[i][1];
        }
        for (int i = 3; i > 0; i--)
        {
            int j = rand() % (i + 1);
            int tmp_x = shuffled_dirs[i][0];
            int tmp_y = shuffled_dirs[i][1];
            shuffled_dirs[i][0] = shuffled_dirs[j][0];
            shuffled_dirs[i][1] = shuffled_dirs[j][1];
            shuffled_dirs[j][0] = tmp_x;
            shuffled_dirs[j][1] = tmp_y;
        }

        int moved = 0;
        for (int i = 0; i < 4; i++)
        {
            int nx = cur.x + shuffled_dirs[i][0];
            int ny = cur.y + shuffled_dirs[i][1];

            if (nx <= 0 || ny <= 0 || nx >= W_MAP - 1 || ny >= H_MAP - 1)
            {
                continue;
            }
            if (visited[nx + ny * W_MAP])
            {
                continue;
            }

            visited[nx + ny * W_MAP] = 1;
            p3_set_tile(map, cur.x + shuffled_dirs[i][0] / 2, cur.y + shuffled_dirs[i][1] / 2, terreP);
            p3_set_tile(map, nx, ny, terreP);
            stack[top++] = (p3_cell_t){nx, ny};
            moved = 1;
            break;
        }

        if (!moved)
        {
            top--;
        }
    }

    free(stack);
}

static void p3_connect_isolated_regions(t_tile map[W_MAP][H_MAP])
{
    unsigned char *visite = calloc((size_t)W_MAP * (size_t)H_MAP, sizeof(unsigned char));
    if (!visite)
        return;

    const int room_x = 4;
    const int room_y = 4;
    const int room_w = PLANETE3_ROOM_W;
    const int room_h = PLANETE3_ROOM_H;
    int start_x = room_x + (room_w / 2);
    int start_y = room_y + (room_h / 2);

    p3_remplissage_zone(map, visite, start_x, start_y);

    for (int x = 1; x < W_MAP - 1; x++)
    {
        for (int y = 1; y < H_MAP - 1; y++)
        {
            if (map[x][y].type != terreP || visite[x + y * W_MAP])
                continue;

            // Trouver le point le plus proche déjà connecté
            int best_dist = INT_MAX;
            int best_cx = -1;
            int best_cy = -1;

            for (int cx = 1; cx < W_MAP - 1; cx++)
            {
                for (int cy = 1; cy < H_MAP - 1; cy++)
                {
                    if (!visite[cx + cy * W_MAP] || map[cx][cy].type != terreP)
                        continue;

                    int dist = (x - cx) * (x - cx) + (y - cy) * (y - cy);
                    if (dist < best_dist)
                    {
                        best_dist = dist;
                        best_cx = cx;
                        best_cy = cy;
                    }
                }
            }

            if (best_cx >= 0 && best_cy >= 0)
            {
                p3_carve_line(map, x, y, best_cx, best_cy);
                p3_remplissage_zone(map, visite, x, y);
            }
        }
    }

    free(visite);
}

void generer_labyrinthe_planete3(t_tile map[W_MAP][H_MAP], int *engrenage_case_x, int *engrenage_case_y)
{
    // Paramètres des salles
    const int room_x = 4;
    const int room_y = 4;
    const int room_w = PLANETE3_ROOM_W;
    const int room_h = PLANETE3_ROOM_H;

    const int end_x = W_MAP - PLANETE3_END_W - 4;
    const int end_y = H_MAP - PLANETE3_END_H - 4;
    const int end_w = PLANETE3_END_W;
    const int end_h = PLANETE3_END_H;

    const int room_exit_x = room_x + room_w;
    const int room_exit_y = room_y + (room_h / 2);
    const int end_entry_x = end_x - 1;
    const int end_entry_y = end_y + (end_h / 2);

    // Remplir la carte avec de l'eau
    p3_fill_map(map, eau);

    // Initialiser le générateur aléatoire
    srand((unsigned int)SDL_GetTicks());

    // Allouer la mémoire pour le maze
    unsigned char *visited = calloc((size_t)W_MAP * (size_t)H_MAP, sizeof(unsigned char));
    if (!visited)
    {
        // Fallback: créer simplement les salles
        p3_open_room(map, room_x, room_y, room_w, room_h);
        p3_open_room(map, end_x, end_y, end_w, end_h);
        if (engrenage_case_x)
            *engrenage_case_x = end_x + (end_w / 2);
        if (engrenage_case_y)
            *engrenage_case_y = end_y + (end_h / 2);
        return;
    }

    // Générer le labyrinthe avec DFS
    p3_generer_labyrinthe_dfs(map, visited, 1, 1);

    free(visited);

    // Créer la salle de départ
    p3_open_room(map, room_x, room_y, room_w, room_h);
    p3_build_wall_ring(map, room_x, room_y, room_w, room_h);
    p3_set_tile(map, room_exit_x, room_exit_y, terreP);

    // Créer la salle d'arrivée
    p3_open_room(map, end_x, end_y, end_w, end_h);
    p3_build_wall_ring(map, end_x, end_y, end_w, end_h);
    p3_set_tile(map, end_entry_x, end_entry_y, terreP);
    p3_set_tile(map, end_x, end_entry_y, terreP);

    // Connecter la salle de départ à la sortie
    // Couloir direct supprimé - passage uniquement via le labyrinthe

    // Connecter les zones isolées
    p3_connect_isolated_regions(map);

    // Position de l'engrenage (centroïde de la salle d'arrivée)
    if (engrenage_case_x)
        *engrenage_case_x = end_x + (end_w / 2);
    if (engrenage_case_y)
        *engrenage_case_y = end_y + (end_h / 2);
}

void appliquer_vision_reduite_planete3(SDL_Renderer *renderer, float centre_x, float centre_y, float largeur_ecran, float hauteur_ecran)
{
    const float rayon_vision = 190.0f;
    const SDL_Color couleur_obscurite = {17, 25, 24, 245};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, couleur_obscurite.r, couleur_obscurite.g, couleur_obscurite.b, couleur_obscurite.a);

    for (int y = 0; y < (int)hauteur_ecran; y++)
    {
        float dy = (float)y - centre_y;
        float abs_dy = fabsf(dy);

        if (abs_dy >= rayon_vision)
        {
            SDL_FRect rect = {0.0f, (float)y, largeur_ecran, 1.0f};
            SDL_RenderFillRect(renderer, &rect);
            continue;
        }

        float dx = sqrtf((rayon_vision * rayon_vision) - (abs_dy * abs_dy));
        float gauche = centre_x - dx;
        float droite = centre_x + dx;

        if (gauche > 0.0f)
        {
            SDL_FRect rect_gauche = {0.0f, (float)y, gauche, 1.0f};
            SDL_RenderFillRect(renderer, &rect_gauche);
        }

        if (droite < largeur_ecran)
        {
            SDL_FRect rect_droite = {droite, (float)y, largeur_ecran - droite, 1.0f};
            SDL_RenderFillRect(renderer, &rect_droite);
        }
    }
}

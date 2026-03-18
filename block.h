/*
 * Projet Block1
 * Auteur: kheira derdour
 */

#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>

typedef struct s_block t_block;

struct s_block {
    int index;
    char *data;
    unsigned long nonce;
    unsigned long previous_hash;
    unsigned long hash;
    t_block *next;
};

unsigned long calculer_hash_chaine(const char *str, unsigned long seed);
unsigned long calculer_hash_bloc(const t_block *bloc);
t_block *creer_bloc(int index, const char *data, unsigned long hash_precedent);
void ajouter_bloc(t_block **chaine, const char *data);
void recalculer_hash_bloc(t_block *bloc);
int chaine_est_valide(const t_block *chaine);
int miner_bloc(t_block *bloc, unsigned int difficulte);
int sauvegarder_chaine(const t_block *chaine, const char *nom_fichier);
int exporter_chaine_json(const t_block *chaine, const char *nom_fichier);
t_block *charger_chaine(const char *nom_fichier);
void liberer_chaine(t_block *chaine);

#endif

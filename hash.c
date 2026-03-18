/*
 * Projet Block1
 * Auteur: kheira derdour
 */

#include "block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================= HASH ========================= */

static int hash_commence_par_zero(unsigned long hash, unsigned int difficulte)
{
    unsigned int i;
    unsigned int total_nibbles;
    unsigned int shift;

    total_nibbles = (unsigned int)(sizeof(unsigned long) * 2UL);
    if (difficulte > total_nibbles)
        difficulte = total_nibbles;
    i = 0;
    while (i < difficulte)
    {
        shift = (total_nibbles - 1U - i) * 4U;
        if (((hash >> shift) & 0xFUL) != 0)
            return 0;
        i++;
    }
    return 1;
}

unsigned long calculer_hash_chaine(const char *str, unsigned long seed)
{
    unsigned long hash;
    size_t i;

    hash = 5381UL + seed;
    i = 0;
    while (str && str[i])
    {
        hash = ((hash << 5) + hash) + (unsigned char)str[i];
        i++;
    }
    return hash;
}

unsigned long calculer_hash_bloc(const t_block *bloc)
{
    unsigned long hash;
    char buffer[32];

    if (!bloc)
        return 0;
    hash = calculer_hash_chaine(bloc->data, (unsigned long)bloc->index);
    snprintf(buffer, sizeof(buffer), "%lu", bloc->previous_hash);
    hash = calculer_hash_chaine(buffer, hash);
    snprintf(buffer, sizeof(buffer), "%lu", bloc->nonce);
    hash = calculer_hash_chaine(buffer, hash);
    return hash;
}

/* ========================= BLOCS ========================= */

t_block *creer_bloc(int index, const char *data, unsigned long hash_precedent)
{
    t_block *bloc;

    bloc = malloc(sizeof(*bloc));
    if (!bloc)
        return NULL;
    bloc->index = index;
    bloc->data = strdup(data ? data : "");
    if (!bloc->data)
    {
        free(bloc);
        return NULL;
    }
    bloc->nonce = 0;
    bloc->previous_hash = hash_precedent;
    bloc->next = NULL;
    bloc->hash = calculer_hash_bloc(bloc);
    return bloc;
}

void ajouter_bloc(t_block **chaine, const char *data)
{
    t_block *curseur;
    t_block *nouveau_bloc;
    int index;
    unsigned long hash_precedent;

    if (!chaine)
        return;
    if (!*chaine)
    {
        *chaine = creer_bloc(0, data ? data : "Genesis Block", 0);
        return;
    }
    curseur = *chaine;
    while (curseur->next)
        curseur = curseur->next;
    index = curseur->index + 1;
    hash_precedent = curseur->hash;
    nouveau_bloc = creer_bloc(index, data ? data : "", hash_precedent);
    if (!nouveau_bloc)
        return;
    curseur->next = nouveau_bloc;
}

void recalculer_hash_bloc(t_block *bloc)
{
    if (!bloc)
        return;
    bloc->hash = calculer_hash_bloc(bloc);
}

/* ==================== VALIDATION / MINAGE ==================== */

int chaine_est_valide(const t_block *chaine)
{
    const t_block *curseur;
    unsigned long hash_attendu;

    if (!chaine)
        return 1;
    curseur = chaine;
    while (curseur)
    {
        hash_attendu = calculer_hash_bloc(curseur);
        if (curseur->hash != hash_attendu)
            return 0;
        if (curseur->next && curseur->next->previous_hash != curseur->hash)
            return 0;
        curseur = curseur->next;
    }
    return 1;
}

int miner_bloc(t_block *bloc, unsigned int difficulte)
{
    if (!bloc)
        return 0;
    bloc->nonce = 0;
    bloc->hash = calculer_hash_bloc(bloc);
    while (!hash_commence_par_zero(bloc->hash, difficulte))
    {
        bloc->nonce++;
        bloc->hash = calculer_hash_bloc(bloc);
    }
    return 1;
}

/* ======================= PERSISTANCE ======================= */

static int ecrire_chaine_json_echappee(FILE *fp, const char *texte)
{
    size_t i;
    unsigned char c;

    if (!fp || !texte)
        return 0;
    i = 0;
    while (texte[i])
    {
        c = (unsigned char)texte[i];
        if (c == '"' || c == '\\')
        {
            if (fputc('\\', fp) == EOF || fputc((int)c, fp) == EOF)
                return 0;
        }
        else if (c == '\n')
        {
            if (fputs("\\n", fp) == EOF)
                return 0;
        }
        else if (c < 32)
        {
            if (fprintf(fp, "\\u%04x", c) < 0)
                return 0;
        }
        else
        {
            if (fputc((int)c, fp) == EOF)
                return 0;
        }
        i++;
    }
    return 1;
}

int sauvegarder_chaine(const t_block *chaine, const char *nom_fichier)
{
    FILE *fp;
    const t_block *curseur;
    unsigned int count;
    unsigned int len;

    if (!nom_fichier)
        return 0;
    fp = fopen(nom_fichier, "w");
    if (!fp)
        return 0;
    count = 0;
    curseur = chaine;
    while (curseur)
    {
        count++;
        curseur = curseur->next;
    }
    if (fprintf(fp, "count %u\n", count) < 0)
        return (fclose(fp), 0);
    curseur = chaine;
    while (curseur)
    {
        len = (unsigned int)strlen(curseur->data);
        if (fprintf(fp, "%d %lu %lu %lu %u\n", curseur->index, curseur->nonce,
                curseur->previous_hash, curseur->hash, len) < 0
            || (len > 0 && fwrite(curseur->data, 1, len, fp) != len)
            || fputc('\n', fp) == EOF)
            return (fclose(fp), 0);
        curseur = curseur->next;
    }
    fclose(fp);
    return 1;
}

int exporter_chaine_json(const t_block *chaine, const char *nom_fichier)
{
    FILE *fp;
    const t_block *curseur;
    int premier;

    if (!nom_fichier)
        return 0;
    fp = fopen(nom_fichier, "w");
    if (!fp)
        return 0;
    if (fputs("{\n  \"blocs\": [\n", fp) == EOF)
        return (fclose(fp), 0);
    curseur = chaine;
    premier = 1;
    while (curseur)
    {
        if (!premier)
        {
            if (fputs(",\n", fp) == EOF)
                return (fclose(fp), 0);
        }
        if (fprintf(fp,
                "    {\n"
                "      \"index\": %d,\n"
                "      \"nonce\": %lu,\n"
                "      \"previous_hash\": %lu,\n"
                "      \"hash\": %lu,\n"
                "      \"data\": \"",
                curseur->index, curseur->nonce, curseur->previous_hash,
                curseur->hash) < 0)
            return (fclose(fp), 0);
        if (!ecrire_chaine_json_echappee(fp, curseur->data))
            return (fclose(fp), 0);
        if (fputs("\"\n    }", fp) == EOF)
            return (fclose(fp), 0);
        premier = 0;
        curseur = curseur->next;
    }
    if (fputs("\n  ]\n}\n", fp) == EOF)
        return (fclose(fp), 0);
    fclose(fp);
    return 1;
}

t_block *charger_chaine(const char *nom_fichier)
{
    FILE *fp;
    t_block *chaine;
    t_block *dernier;
    t_block *bloc;
    unsigned int count;
    unsigned int i;
    unsigned int len;
    char *data;
    int index;
    unsigned long nonce;
    unsigned long hash_precedent;
    unsigned long hash_lu;
    int ch;

    if (!nom_fichier)
        return NULL;
    fp = fopen(nom_fichier, "r");
    if (!fp)
        return NULL;
    if (fscanf(fp, "count %u\n", &count) != 1)
        return (fclose(fp), NULL);
    chaine = NULL;
    dernier = NULL;
    i = 0;
    while (i < count)
    {
        if (fscanf(fp, "%d %lu %lu %lu %u\n", &index, &nonce,
                &hash_precedent, &hash_lu, &len) != 5)
            return (fclose(fp), liberer_chaine(chaine), NULL);
        data = malloc((size_t)len + 1UL);
        if (!data)
            return (fclose(fp), liberer_chaine(chaine), NULL);
        if (len > 0 && fread(data, 1, len, fp) != len)
            return (free(data), fclose(fp), liberer_chaine(chaine), NULL);
        data[len] = '\0';
        ch = fgetc(fp);
        if (ch == '\r')
            ch = fgetc(fp);
        if (ch != '\n' && ch != EOF)
            return (free(data), fclose(fp), liberer_chaine(chaine), NULL);
        bloc = creer_bloc(index, data, hash_precedent);
        free(data);
        if (!bloc)
            return (fclose(fp), liberer_chaine(chaine), NULL);
        bloc->nonce = nonce;
        bloc->hash = calculer_hash_bloc(bloc);
        (void)hash_lu;
        if (!chaine)
            chaine = bloc;
        else
            dernier->next = bloc;
        dernier = bloc;
        i++;
    }
    fclose(fp);
    return chaine;
}

/* ======================= MEMOIRE ======================= */

void liberer_chaine(t_block *chaine)
{
    t_block *suivant;

    while (chaine)
    {
        suivant = chaine->next;
        free(chaine->data);
        free(chaine);
        chaine = suivant;
    }
}
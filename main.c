/*
 * Projet Block1
 * Auteur: kheira derdour
 */

#include "block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAIN_FILE "blockchain.db"
#define CHAIN_JSON_FILE "blockchain.json"

#define C_RESET "\033[0m"
#define C_BOLD "\033[1m"
#define C_CYAN "\033[36m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_MAGENTA "\033[35m"
#define C_RED "\033[31m"

static int compter_blocs(const t_block *chaine)
{
    int total;

    total = 0;
    while (chaine)
    {
        total++;
        chaine = chaine->next;
    }
    return total;
}

static void effacer_ecran(void)
{
    printf("\033[2J\033[H");
}

static void supprimer_retour_ligne(char *texte)
{
    size_t len;

    if (!texte)
        return;
    len = strlen(texte);
    if (len > 0 && texte[len - 1] == '\n')
        texte[len - 1] = '\0';
}

static void afficher_chaine(const t_block *chaine, const char *titre)
{
    const t_block *curseur;

    printf("\n%s%s=== %s ===%s\n", C_CYAN, C_BOLD, titre, C_RESET);
    curseur = chaine;
    while (curseur)
    {
        printf("%sBlock %d%s\n", C_MAGENTA, curseur->index, C_RESET);
        printf("  data          : %s\n", curseur->data);
        printf("  nonce         : %lu\n", curseur->nonce);
        printf("  previous_hash : %016lx\n", curseur->previous_hash);
        printf("  hash          : %016lx\n", curseur->hash);
        curseur = curseur->next;
    }
}

static void afficher_interface(const t_block *chaine)
{
    int valide;
    int total;

    valide = chaine_est_valide(chaine);
    total = compter_blocs(chaine);
    effacer_ecran();
    printf("%s%s+--------------------------------------------+%s\n", C_CYAN,
        C_BOLD, C_RESET);
    printf("%s%s|        BLOCKCHAIN INTERACTIVE - KHEIRA      |%s\n", C_CYAN,
        C_BOLD, C_RESET);
    printf("%s%s+--------------------------------------------+%s\n", C_CYAN,
        C_BOLD, C_RESET);
    printf("%sBlocs: %d%s  |  Etat: %s%s%s\n", C_YELLOW, total, C_RESET,
        valide ? C_GREEN : C_RED, valide ? "VALIDE" : "INVALIDE", C_RESET);
    printf("%sCommande de sortie:%s tape %s:q%s puis Entree\n", C_YELLOW,
        C_RESET, C_BOLD, C_RESET);
    afficher_chaine(chaine, "Derniers blocs");
    printf("\n%s%s[ Zone de saisie ]%s\n", C_CYAN, C_BOLD, C_RESET);
}

static int sauvegarder_ou_erreur(const t_block *chaine)
{
    if (!sauvegarder_chaine(chaine, CHAIN_FILE))
    {
        printf("%sErreur:%s impossible de sauvegarder '%s'\n", C_RED, C_RESET,
            CHAIN_FILE);
        return 0;
    }
    if (!exporter_chaine_json(chaine, CHAIN_JSON_FILE))
    {
        printf("%sErreur:%s impossible d'exporter '%s'\n", C_RED, C_RESET,
            CHAIN_JSON_FILE);
        return 0;
    }
    return 1;
}

static int verifier_ou_erreur(const t_block *chaine)
{
    if (!chaine_est_valide(chaine))
    {
        printf("%sErreur:%s la chaine est invalide. Arret de securite.\n", C_RED,
            C_RESET);
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    t_block *chaine;
    char input[1024];
    int i;

    chaine = charger_chaine(CHAIN_FILE);
    if (!chaine)
        ajouter_bloc(&chaine, "Genesis");
    if (!verifier_ou_erreur(chaine))
    {
        liberer_chaine(chaine);
        return 1;
    }

    i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], ":q") == 0)
            break;
        ajouter_bloc(&chaine, argv[i]);
        if (!verifier_ou_erreur(chaine))
        {
            liberer_chaine(chaine);
            return 1;
        }
        if (!sauvegarder_ou_erreur(chaine))
        {
            liberer_chaine(chaine);
            return 1;
        }
        i++;
    }

    afficher_interface(chaine);
    while (1)
    {
        printf("%s> %s", C_GREEN, C_RESET);
        if (!fgets(input, sizeof(input), stdin))
            break;
        supprimer_retour_ligne(input);
        if (strcmp(input, ":q") == 0)
            break;
        if (input[0] == '\0')
            continue;
        ajouter_bloc(&chaine, input);
        if (!verifier_ou_erreur(chaine))
        {
            liberer_chaine(chaine);
            return 1;
        }
        if (!sauvegarder_ou_erreur(chaine))
        {
            liberer_chaine(chaine);
            return 1;
        }
        afficher_interface(chaine);
        printf("%sBloc ajoute et sauvegarde.%s\n", C_GREEN, C_RESET);
    }

    if (sauvegarder_ou_erreur(chaine))
        printf("%sBlockchain sauvegardee dans '%s'. Sortie propre.%s\n", C_GREEN,
            CHAIN_FILE, C_RESET);

    liberer_chaine(chaine);
    return 0;
}

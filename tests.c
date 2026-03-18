/*
 * Projet Block1
 * Auteur: kheira derdour
 */

#include "block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_DB "blockchain_test.db"
#define TEST_JSON "blockchain_test.json"

static int tests_ok = 0;
static int tests_fail = 0;

static void verifier(int condition, const char *nom_test)
{
    if (condition)
    {
        tests_ok++;
        printf("[OK] %s\n", nom_test);
    }
    else
    {
        tests_fail++;
        printf("[KO] %s\n", nom_test);
    }
}

static t_block *creer_chaine_de_test(void)
{
    t_block *chaine;

    chaine = NULL;
    ajouter_bloc(&chaine, "Genesis");
    ajouter_bloc(&chaine, "Alice paye Bob");
    ajouter_bloc(&chaine, "Bob paye Karim");
    return chaine;
}

static void test_validite_chaine(void)
{
    t_block *chaine;

    chaine = creer_chaine_de_test();
    verifier(chaine != NULL, "creation chaine");
    verifier(chaine_est_valide(chaine) == 1, "chaine valide apres creation");
    liberer_chaine(chaine);
}

static void test_chaine_invalide_apres_modif(void)
{
    t_block *chaine;
    t_block *bloc1;
    char *nouvelle_data;

    chaine = creer_chaine_de_test();
    bloc1 = chaine ? chaine->next : NULL;
    verifier(bloc1 != NULL, "presence bloc 1");
    if (!bloc1)
    {
        liberer_chaine(chaine);
        return;
    }
    nouvelle_data = strdup("Alice vole Bob");
    verifier(nouvelle_data != NULL, "allocation nouvelle data");
    if (!nouvelle_data)
    {
        liberer_chaine(chaine);
        return;
    }
    free(bloc1->data);
    bloc1->data = nouvelle_data;
    recalculer_hash_bloc(bloc1);
    verifier(chaine_est_valide(chaine) == 0, "chaine invalide apres attaque");
    liberer_chaine(chaine);
}

static void test_sauvegarde_et_rechargement(void)
{
    t_block *source;
    t_block *chargee;

    source = creer_chaine_de_test();
    verifier(sauvegarder_chaine(source, TEST_DB) == 1, "sauvegarde chaine");
    chargee = charger_chaine(TEST_DB);
    verifier(chargee != NULL, "rechargement chaine");
    verifier(chaine_est_valide(chargee) == 1, "chaine chargee valide");
    liberer_chaine(source);
    liberer_chaine(chargee);
    remove(TEST_DB);
}

static void test_export_json(void)
{
    t_block *chaine;
    FILE *fp;
    char contenu[4096];
    size_t lu;

    chaine = creer_chaine_de_test();
    verifier(exporter_chaine_json(chaine, TEST_JSON) == 1, "export JSON");
    fp = fopen(TEST_JSON, "r");
    verifier(fp != NULL, "ouverture JSON");
    if (!fp)
    {
        liberer_chaine(chaine);
        return;
    }
    lu = fread(contenu, 1, sizeof(contenu) - 1, fp);
    contenu[lu] = '\0';
    fclose(fp);
    verifier(strstr(contenu, "\"blocs\"") != NULL, "JSON contient blocs");
    verifier(strstr(contenu, "\"data\": \"Genesis\"") != NULL,
        "JSON contient Genesis");
    remove(TEST_JSON);
    liberer_chaine(chaine);
}

int main(void)
{
    printf("=== Lancement des tests unitaires ===\n");
    test_validite_chaine();
    test_chaine_invalide_apres_modif();
    test_sauvegarde_et_rechargement();
    test_export_json();
    printf("\nResultat: %d OK, %d KO\n", tests_ok, tests_fail);
    return tests_fail == 0 ? 0 : 1;
}

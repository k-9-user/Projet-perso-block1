# Projet Blockchain en C

Projet personnel realise par **Kheira** pour comprendre et implementer les mecanismes de base d'une blockchain sans bibliotheque externe.

## Objectif
Construire une mini blockchain locale qui permet de:
- creer des blocs
- chainer les blocs via le hash precedent
- verifier l'integrite de la chaine
- stocker et recharger les donnees depuis un fichier
- interagir en ligne de commande
- interagir via une interface web locale

## Fonctionnalites principales
- Chargement automatique de la chaine depuis `blockchain.db`
- Creation d'un bloc `Genesis` si aucun fichier n'existe
- Ajout de blocs via arguments CLI (`./blockchain "bloc 1" "bloc 2"`)
- Ajout de blocs en mode interactif (une ligne + `Entree`)
- Sortie propre avec sauvegarde via `:q`
- Persistance en format texte lisible
- Export JSON automatique dans `blockchain.json`

## Architecture du projet
- `block.h`: structure de donnees et prototypes
- `hash.c`: hash, gestion des blocs, validation, minage, persistance
- `main.c`: interface console interactive
- `Makefile`: compilation et nettoyage

## Concepts blockchain implementes
- `previous_hash`: chaque bloc pointe vers le hash du bloc precedent
- `hash`: hash calcule a partir des donnees du bloc
- `nonce`: support d'un mini proof-of-work
- validation: verification de la coherence hash/hash precedent

## Compilation et execution
Compiler:
```bash
make
```

Recompiler proprement:
```bash
make re
```

Lancer les tests unitaires:
```bash
make test
```

Lancer l'interface web:
```bash
make web
```

Executer:
```bash
./blockchain "block texte..."
```

Mode interactif:
- saisir du texte puis `Entree` pour ajouter un bloc
- saisir `:q` puis `Entree` pour sauvegarder et quitter

Mode web:
- ouvrir `http://127.0.0.1:8080`
- ecrire dans la zone de texte puis cliquer sur `Ajouter et sauvegarder`
- la chaine se met a jour en direct

## Deux facons d'utiliser le projet
1. Via la ligne de commande (terminal):
- compiler avec `make` ou `make re`
- lancer avec `./blockchain "ton bloc"`
- utiliser `:q` pour sauvegarder et quitter

2. Via le serveur web local:
- lancer avec `make web`
- ouvrir `http://127.0.0.1:8080`
- ajouter les blocs depuis l'interface graphique

## Exemple de fichier sauvegarde
Le fichier `blockchain.db` est en texte lisible:
```txt
count 3
0 0 0 249844503202014813 7
Genesis
1 0 249844503202014813 12730453349292749821 14
Entre ton texte
...
```

Export JSON genere automatiquement:
```json
{
	"blocs": [
		{
			"index": 0,
			"nonce": 0,
			"previous_hash": 0,
			"hash": 249844503202014813,
			"data": "Genesis"
		}
	]
}
```

## Ce que j'ai appris
- manipuler des listes chainees en C
- gerer la memoire dynamique proprement
- concevoir une logique modulaire (`.h` + `.c`)
- serialiser/deserialiser des donnees
- structurer un petit projet avec `Makefile`

## Difficultes rencontrees
- boucle de minage trop longue au debut
- format binaire illisible avec `cat`
- besoin de sortie propre pour ne pas perdre les donnees


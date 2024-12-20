# Projet de Manipulation d'Images Matricielles

## Description
Projet développé dans le cadre du cours d'Informatique (XLG1IE010) de Licence 1 à Nantes Université. Il permet de créer des animations à partir d'images PNG en appliquant différentes transformations (fondu, flou, masquage, etc.).

## Prérequis
- Compilateur C++ et outils de base : `sudo apt install g++ build-essential`
- Bibliothèques requises : `sudo apt install libpng-dev imagemagick`
- OpenMP : `sudo apt install libomp-dev`

## Installation
1. Extraire le zip du projet OU cloner le dépôt :
```bash
git clone https://github.com/ixtaab/Groupe2TDINFO184
```

2. Compiler le projet :
```bash
./build.sh
```

## Utilisation
1. Créer un fichier de configuration
2. Exécuter le programme :
```bash
./bin/program votre_config.txt
```

3. Pour tester toutes les fonctionnalités :
```bash
./test.sh
```

## Fonctionnalités principales
- Fondu au noir/niveaux de gris
- Bruitage d'image
- Floutage
- Masquage (rideau/persiennes)
- Transitions entre images
- Rétrécissement
- ...

## Structure du projet
- `src/` : Fichiers sources
- `headers/` : Fichiers d'en-tête
- `pngio/` : Bibliothèque PNG
- `test_ressources/` : Ressources pour les tests

## Auteurs
- Raphael MUYARD
- Alexandre GODFROY
- Arnaud CHESSÉ
- Marc-Eliel Ouattara
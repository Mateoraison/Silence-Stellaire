# 🌌 Silence Stellaire

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![SDL3](https://img.shields.io/badge/SDL-3.0-green.svg)](https://www.libsdl.org/)

> Un jeu de survie spatial en 2D développé en C avec SDL3

## 📋 Table des matières

# 🌌 Silence Stellaire

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![SDL3](https://img.shields.io/badge/SDL-3.0-green.svg)](https://www.libsdl.org/)

> Un jeu de survie spatial en 2D développé en C avec SDL3

## 📋 Table des matières

- [À propos](#à-propos)
- [Fonctionnalités](#fonctionnalités)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Architecture du projet](#architecture-du-projet)
- [Équipe](#équipe)
- [Planification](#planification)
- [Licence](#licence)

## 🎮 À propos

**Silence Stellaire** est un jeu de survie et d'aventure en 2D vue de dessus, développé dans le cadre du module Projet de L2 Informatique 2025-2026.

### Histoire

Suite à un accident catastrophique, votre astronaute se retrouve échoué sur une planète inconnue avec un vaisseau spatial endommagé. Votre mission : parcourir différentes planètes hostiles, collecter des ressources, affronter des créatures extraterrestres et résoudre des énigmes pour réparer votre vaisseau et rentrer sur Terre.

### Période de développement

- **Début** : 1er décembre 2025
- **Fin prévue** : 17 avril 2026

## ✨ Fonctionnalités

- **Exploration spatiale** : Voyagez à travers plusieurs planètes aux environnements uniques
- **Combat** : Affrontez des créatures hostiles dans des combats dynamiques
- **Énigmes** : Résolvez des puzzles pour progresser dans votre aventure
- **Environnements variés** : Chaque planète offre des défis et une atmosphère différents
- **Sauvegarde** : Système de sauvegarde de progression

## 🔧 Prérequis

- **Compilateur C** : GCC
- **Make** : Pour la compilation automatisée
- **SDL3** : Bibliothèque graphique principale
- **SDL3_image** : Gestion des formats d'images (PNG, JPG, etc.)
- **SDL3_ttf** : Rendu du texte à l'écran

## 🚀 Installation

1. **Cloner le dépôt**
```bash
git clone https://github.com/Mateoraison/Silence-Stellaire.git
cd Silence-Stellaire
```

2. **Compiler le projet**
```bash
make all        # Linux, Mac et Windows
build_run.bat   # Windows
```

3. **Lancer le jeu**
```bash
./bin/silenceStellaire       # Linux
./bin/Silence-Stellaire.exe  # Windows
```

## 🎯 Utilisation

### Commandes de base

- **Déplacement** : ZQSD ou WASD
- **Combat** : Clic Gauche
- **Inventaire** : I
- **Rammaser Items** : E
- **Interraction** : Clic Droit
- **Pause** : Échap

## 📁 Architecture du projet

```
Silence-Stellaire/
├── src/              # Code source du jeu
├── include/          # Fichiers d'en-tête des librairies
├── bin/              # Lien de librairie et executable
├── lib/              # Librairie
├── obj/              # Fichiers .o
├── test/             # Jeux de test
├── assets/           # Ressources graphiques et sonores
├── doc/              # Documentation technique
├── Makefile          # Script de compilation
└── README.md         # Ce fichier
```

## 👥 Équipe

| Nom | Rôle | GitHub |
|-----|------|--------|
| **Raison Matéo** | Chef de projet / Développeur | [@Mateoraison](https://github.com/Mateoraison) |
| **Joshua Donné** | Développeur | [@joshuadonne](https://github.com/joshuadonne) |
| **Leroux Maxime** | Développeur | [@OniZurKa](https://github.com/OniZurKa) |

## 📊 Planification

Le suivi du projet et le diagramme de Gantt sont disponibles sur Google Sheets :

🔗 [Diagramme de Gantt du projet](https://docs.google.com/spreadsheets/d/1Crgyg5-aRagyHwb9pbF1vICBsLmRDF0shIkXx3tQi3M/edit?usp=sharing)

## 📝 Licence

Ce projet est réalisé dans le cadre universitaire à l'Université du Mans.

---

**Projet de L2 Informatique 2025-2026** | Université du Mans

# 🌳 | Ctree
![Badge Langage](https://img.shields.io/badge/Langage%20:-C-blue?style=plastic)
![Badge Licence](https://img.shields.io/badge/Licence%20:-MIT-darkblue?style=plastic)
![Badge Version](https://img.shields.io/badge/Version%20:-v11-lim?style=plastic)
![Badge OS](https://img.shields.io/badge/OS%20:-LINUX-blue?style=plastic)
![Badge État](https://img.shields.io/badge/État%20%3A-En%20cours...-orange?style=plastic)

**CTree** est un outil de visualisation d'arboresence de répertoire Linux

## 📋 | Fonctionnalités de CTree

| **Paramètres & Fonctionnalités**                                              | **Version** |
|-------------------------------------------------------------------------------|:-----------:|
| [🌲] *Affichage*                                                              |             |
| **Arborescence récursive** avec préfixes `├──` / `└──`                        | `v3`        |
| **Coloration** des répertoires en vert, fichiers inconnus en rouge            | `v3`        |
| **Dégradé de couleur** selon la taille des fichiers                           | `v6`        |
| **Surlignage** des résultats de recherche avec fond coloré                    | `v10`       |
| [⚙️] *Paramètres*                                                             |             |
| **`-path`** Affichage du chemin de chaque fichier                             | `v5`        |
| **`-size`** Affichage du poids de chaque fichier et répertoire                | `v5`        |
| **`-deep`** Affichage combiné du chemin et du poids                           | `v5`        |
| **`-cut X Y`** Limite la profondeur et la longueur de l'arbre                 | `v8`        |
| **`-ban [...]`** Exclusion de fichiers ou dossiers de l'arbre                 | `v9`        |
| **`-search [...]`** Recherche et mise en évidence dans l'arbre                | `v10`       |
| **`-stats`** Affichage des statistiques globales de l'arbre                   | `v11`       |
| **`-debug`** Mode debug pour diagnostiquer les erreurs                        | `v8`        |
| **`-info`** Menu d'aide intégré                                               | `v8`        |
| [🔩] *Technique*                                                              |             |
| **Calcul récursif** de la taille des répertoires                              | `v5`        |
| **Gestion mémoire** complète avec `freeDirectory()`                           | `v2`        |
| **Validation** des paramètres avec détection de doublons et erreurs           | `v5`        |
| **Protection** contre les doubles slashs dans les chemins                     | `v8`        |

## ⚙️ | Fonctionnement et déploiement 
### Requis :
- *Tout version* -  N'importe quel PC sous Linux (*gcc* conseillée)

## 💡 | Remarques 
**CTree** est une application créée pour apprendre et m'améliorer en C et dans l'algorithmie en général, dans le cadre de mes études en informatique... Donc c'est pas du sérieux les gars.

## 📜 | Licence

Ce projet est sous licence **MIT** – Vous pouvez faire un peu ce que vous voulez tant que je suis crédité.

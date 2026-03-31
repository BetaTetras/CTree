# 🌳 | Ctree
![Badge Langage](https://img.shields.io/badge/Langage%20:-C-blue?style=plastic)
![Badge Licence](https://img.shields.io/badge/Licence%20:-MIT-darkblue?style=plastic)
![Badge Version](https://img.shields.io/badge/Version%20:-v4.4-lim?style=plastic)
![Badge OS](https://img.shields.io/badge/OS%20:-LINUX-blue?style=plastic)
![Badge État](https://img.shields.io/badge/État%20%3A-Released-green?style=plastic)

**CTree** est un outil de visualisation d'arborescence de répertoire Linux

## 📋 | Fonctionnalités de CTree v5.0

| **Paramètres & Fonctionnalités**                                              | **Version** |
|-------------------------------------------------------------------------------|:-----------:|
| [🌲] *Affichage*                                                              |             |
| **Arborescence récursive** avec préfixes `├──` / `└──`                        | `v2.1`      |
| **Coloration** des répertoires en vert, fichiers inconnus en rouge            | `v2.1`      |
| **Dégradé de couleur** selon la taille des fichiers                           | `v4.0`      |
| **Dégradé de couleur** sur les paths affichés                                 | `v4.0`      |
| **`[Cuted]`** Indicateur visuel sur les répertoires tronqués                  | `v4.2`      |
| **Surlignage** des résultats de recherche avec fond coloré                    | `v4.4`      |
| [⚙️] *Paramètres*                                                             |             |
| **`<path>`** Choix du répertoire à afficher en argument                       | `v2.0`      |
| **`-path`** Affichage du chemin de chaque fichier                             | `v4.0`      |
| **`-size`** Affichage du poids de chaque fichier et répertoire                | `v4.0`      |
| **`-deep`** Affichage combiné du chemin et du poids                           | `v4.0`      |
| **`-cut X Y`** Limite la profondeur et la longueur de l'arbre                 | `v4.2`      |
| **`-ban [...]`** Exclusion de fichiers ou dossiers de l'arbre                 | `v4.3`      |
| **`-search [...]`** Recherche et mise en évidence dans l'arbre                | `v4.4`      |
| **`-debug`** Mode debug pour diagnostiquer les erreurs                        | `v4.2`      |
| **`-info`** Menu d'aide intégré                                               | `v4.2`      |
| [🔩] *Technique*                                                              |             |
| **Récursivité** complète via `scanDirectory()`                                | `v2.0`      |
| **Gestion mémoire** complète avec `freeDirectory()`                           | `v2.0`      |
| **Structures** `Directory` et `Element` pour organiser les données            | `v2.0`      |
| **Calcul récursif** de la taille des répertoires via `getDirectorySize()`     | `v3.0`      |
| **`getSizeCategory()`** Catégorisation du poids des fichiers                  | `v3.0`      |
| **`getParameter()`** Validation des paramètres avec détection de doublons     | `v3.0`      |
| **`_atoi()`** Conversion string → int maison avec gestion d'erreur           | `v4.2`      |
| **`printf_wave_utf8()`** Affichage en dégradé de couleur                      | `v4.0`      |
| **`sizeToGradientColor()`** Couleur dynamique selon le poids du fichier       | `v4.0`      |
| **`printf_RGB()`** Fonction d'affichage coloré RGB                            | `v3.0`      |
| **`printf_RGB_BG()`** Affichage coloré avec fond coloré                       | `v4.4`      |
| **Protection** contre les doubles slashs dans les chemins                     | `v4.2`      |
| **`indexLastVisible`** Gestion correcte du dernier élément non banni          | `v4.3`      |

## ⚙️ | Fonctionnement et déploiement

### Requis :
- *v4.4* - N'importe quel PC sous Linux (*gcc* conseillée)

### Compilation :
```bash
gcc CTree.c -o CTree
./CTree <path> [-size] [-path] [-deep] [-cut X Y] [-ban f1 f2...] [-search f1 f2...] [-debug]
```

## 💡 | Remarques
**CTree v4.4** introduit `-search` qui permet de rechercher et mettre en évidence des fichiers ou dossiers dans l'arborescence avec un surlignage visuel grâce à `printf_RGB_BG()`.

## 📜 | Licence
Ce projet est sous licence **MIT** – Vous pouvez faire un peu ce que vous voulez tant que je suis crédité.
# 🌳 | Ctree
![Badge Langage](https://img.shields.io/badge/Langage%20:-C-blue?style=plastic)
![Badge Licence](https://img.shields.io/badge/Licence%20:-MIT-darkblue?style=plastic)
![Badge Version](https://img.shields.io/badge/Version%20:-clear-lim?style=plastic)
![Badge OS](https://img.shields.io/badge/OS%20:-LINUX-blue?style=plastic)
![Badge État](https://img.shields.io/badge/État%20%3A-Released-green?style=plastic)

**CTree** est un outil de visualisation d'arborescence de répertoire Linux

## 📋 | Fonctionnalités de CTree v6.0

| **Paramètres & Fonctionnalités**                                              | **Version** |
|-------------------------------------------------------------------------------|:-----------:|
| [🌲] *Affichage*                                                              |             |
| **Arborescence récursive** avec préfixes `├──` / `└──`                        | `v2.1`      |
| **Coloration** des répertoires en vert, fichiers inconnus en rouge            | `v2.1`      |
| **Dégradé de couleur** selon la taille des fichiers                           | `v4.0`      |
| **Dégradé de couleur** sur les paths affichés                                 | `v4.0`      |
| **Barre de progression** arc-en-ciel pour les statistiques d'extensions       | `v5.0`      |
| **Box colorées** pour l'affichage des statistiques et résultats               | `v5.0`      |
| **Mise en évidence** des éléments trouvés dans l'arbre (`fond coloré`)        | `v6.0`      |
| **Box récapitulative** des éléments recherchés avec nom, type et path         | `v6.0`      |
| **Animation d'attente** pendant le scan de l'arborescence                     | `v6.0`      |
| [⚙️] *Paramètres*                                                             |             |
| **`<path>`** Choix du répertoire à afficher en argument                       | `v2.0`      |
| **`-path`** Affichage du chemin de chaque fichier                             | `v4.0`      |
| **`-size`** Affichage du poids de chaque fichier et répertoire                | `v4.0`      |
| **`-deep`** Affichage combiné du chemin et du poids                           | `v4.0`      |
| **`-cut`** Limite de profondeur et de longueur de l'arborescence              | `v4.0`      |
| **`-ban`** Exclusion de fichiers/répertoires de l'affichage                   | `v5.0`      |
| **`-stats`** Affichage des statistiques globales de l'arborescence            | `v5.0`      |
| **`-search`** Recherche d'éléments par nom dans l'arborescence                | `v6.0`      |
| **`-out`** Export de l'arborescence dans un fichier `.tree`                   | `v6.0`      |
| **`-debug`** Affichage des paramètres et temps d'exécution                    | `v5.0`      |
| [🔩] *Technique*                                                              |             |
| **Récursivité** complète via `scanDirectory()`                                | `v2.0`      |
| **Gestion mémoire** complète avec `freeDirectory()`                           | `v2.0`      |
| **Structures** `Directory` et `Element` pour organiser les données            | `v2.0`      |
| **Calcul récursif** de la taille des répertoires via `getDirectorySize()`     | `v3.0`      |
| **`getSizeStr()`** Représentation textuelle unifiée du poids                  | `v5.0`      |
| **`getParameter()`** Validation des paramètres avec détection de doublons     | `v3.0`      |
| **`printf_wave_utf8()`** Affichage en dégradé avec sortie RGB                 | `v4.0`      |
| **`sizeToGradientColor()`** Couleur dynamique selon le poids du fichier       | `v4.0`      |
| **`printf_RGB()`** / **`printf_RGB_BG()`** Affichage coloré RGB et RGB+fond  | `v3.0`      |
| **`print_rainbow()`** Affichage arc-en-ciel caractère par caractère           | `v5.0`      |
| **`printBarre()`** Barre de progression arc-en-ciel                           | `v5.0`      |
| **`getExt()`** Extraction de l'extension d'un fichier                         | `v5.0`      |
| **`addExt()` / `triABulleExt()`** Comptage et tri des extensions              | `v5.0`      |
| **`addToTop10()`** Top 10 des fichiers les plus lourds avec `strdup`          | `v5.0`      |
| **`ExtStat`** Structure pour les statistiques par extension                   | `v5.0`      |
| **`printf_searched()`** Affichage de la box récapitulative de recherche       | `v6.0`      |
| **`printf_debug()`** Fonction dédiée au mode debug avec temps d'exécution     | `v6.0`      |
| **`strpartcmp()`** Comparaison partielle de chaînes                           | `v6.0`      |
| **Projet multi-fichiers** `CTree.c` / `tools.c` / `output.c` + headers       | `v6.0`      |
| **Compilation via** `makefile` avec binaire dans `bin/`                       | `v6.0`      |
| **Passage unique** `readdir` avec `realloc`                                   | `v5.0`      |

## ⚙️ | Fonctionnement et déploiement

### Requis :
- *v6.0* - N'importe quel PC sous Linux (*gcc* conseillée)

### Compilation :
```bash
make
./bin/CTree <path> [-size] [-path] [-deep] [-cut X Y] [-ban ...] [-stats] [-search ...] [-out] [-debug]
```

## 💡 | Remarques
**CTree v6.0** est une version majeure sur le plan architectural — le projet est désormais découpé en plusieurs fichiers sources (`CTree.c`, `tools.c`, `output.c`) avec leurs headers dans `include/`, compilés via un `makefile`. Le paramètre `-out` permet d'exporter l'arborescence dans un fichier `.tree`. Une animation d'attente s'affiche pendant le scan et le mode `-debug` affiche désormais le temps d'exécution.

## 📜 | Licence
Ce projet est sous licence **MIT** – Vous pouvez faire un peu ce que vous voulez tant que je suis crédité.
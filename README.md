# 🌳 | Ctree
![Badge Langage](https://img.shields.io/badge/Langage%20:-C-blue?style=plastic)
![Badge Licence](https://img.shields.io/badge/Licence%20:-MIT-darkblue?style=plastic)
![Badge Version](https://img.shields.io/badge/Version%20:-v4.0-lim?style=plastic)
![Badge OS](https://img.shields.io/badge/OS%20:-LINUX-blue?style=plastic)
![Badge État](https://img.shields.io/badge/État%20%3A-Released-green?style=plastic)

**CTree** est un outil de visualisation d'arborescence de répertoire Linux

## 📋 | Fonctionnalités de CTree v4.0

| **Paramètres & Fonctionnalités**                                              | **Version** |
|-------------------------------------------------------------------------------|:-----------:|
| [🌲] *Affichage*                                                              |             |
| **Arborescence récursive** avec préfixes `├──` / `└──`                        | `v2.1`      |
| **Coloration** des répertoires en vert, fichiers inconnus en rouge            | `v2.1`      |
| **Dégradé de couleur** selon la taille des fichiers                           | `v4.0`      |
| **Dégradé de couleur** sur les paths affichés                                 | `v4.0`      |
| [⚙️] *Paramètres*                                                             |             |
| **`<path>`** Choix du répertoire à afficher en argument                       | `v2.0`      |
| **`-path`** Affichage du chemin de chaque fichier                             | `v4.0`      |
| **`-size`** Affichage du poids de chaque fichier et répertoire                | `v4.0`      |
| **`-deep`** Affichage combiné du chemin et du poids                           | `v4.0`      |
| **`-cut`** Limite de l'arborescence *(work in progress)*                      | `v4.0`      |
| [🔩] *Technique*                                                              |             |
| **Récursivité** complète via `scanDirectory()`                                | `v2.0`      |
| **Gestion mémoire** complète avec `freeDirectory()`                           | `v2.0`      |
| **Structures** `Directory` et `Element` pour organiser les données            | `v2.0`      |
| **Calcul récursif** de la taille des répertoires via `getDirectorySize()`     | `v3.0`      |
| **`getSizeCategory()`** Catégorisation du poids des fichiers                  | `v3.0`      |
| **`getParameter()`** Validation des paramètres avec détection de doublons     | `v3.0`      |
| **`printf_wave_utf8()`** Affichage en dégradé de couleur                      | `v4.0`      |
| **`sizeToGradientColor()`** Couleur dynamique selon le poids du fichier       | `v4.0`      |
| **`printf_RGB()`** Fonction d'affichage coloré RGB                            | `v3.0`      |

## ⚙️ | Fonctionnement et déploiement

### Requis :
- *v4.0* - N'importe quel PC sous Linux (*gcc* conseillée)

### Compilation :
```bash
gcc CTree.c -o CTree
./CTree <path> [-size] [-path] [-deep]
```

## 💡 | Remarques
**CTree v4.0** est une version majeure sur le plan visuel — les dégradés de couleur sur les tailles et les paths rendent l'arborescence bien plus lisible. La syntaxe des paramètres passe définitivement aux mots-clés (`-size`, `-path`, `-deep`) ce qui casse la compatibilité avec la v3.0.

## 📜 | Licence
Ce projet est sous licence **MIT** – Vous pouvez faire un peu ce que vous voulez tant que je suis crédité.

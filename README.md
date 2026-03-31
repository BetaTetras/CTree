# 🌳 | Ctree
![Badge Langage](https://img.shields.io/badge/Langage%20:-C-blue?style=plastic)
![Badge Licence](https://img.shields.io/badge/Licence%20:-MIT-darkblue?style=plastic)
![Badge Version](https://img.shields.io/badge/Version%20:-v2.1-lim?style=plastic)
![Badge OS](https://img.shields.io/badge/OS%20:-LINUX-blue?style=plastic)
![Badge État](https://img.shields.io/badge/État%20%3A-Released-green?style=plastic)

**CTree** est un outil de visualisation d'arborescence de répertoire Linux

## 📋 | Fonctionnalités de CTree v2.1

| **Paramètres & Fonctionnalités**                                              | **Version** |
|-------------------------------------------------------------------------------|:-----------:|
| [🌲] *Affichage*                                                              |             |
| **Arborescence récursive** avec préfixes `├──` / `└──`                        | `v2.1`      |
| **Coloration** des répertoires en vert                                        | `v1.0`      |
| **Coloration** des fichiers inconnus en rouge                                 | `v2.1`      |
| [⚙️] *Paramètres*                                                             |             |
| **`<path>`** Choix du répertoire à afficher en argument                       | `v2.0`      |
| [🔩] *Technique*                                                              |             |
| **Récursivité** complète via `scanDirectory()`                                | `v2.0`      |
| **Gestion mémoire** complète avec `freeDirectory()`                           | `v2.0`      |
| **Structures** `Directory` et `Element` pour organiser les données            | `v2.0`      |
| **`depthCount()`** Calcul de la profondeur depuis le path                     | `v2.0`      |
| **`T_UNKN`** Gestion des fichiers non reconnus                                | `v2.1`      |
| **`printf_GREEN()`** Fonction d'affichage coloré maison                       | `v1.0`      |
| **`printf_RED()`** Fonction d'affichage rouge maison                          | `v2.1`      |

## ⚙️ | Fonctionnement et déploiement

### Requis :
- *v2.1* - N'importe quel PC sous Linux (*gcc* conseillée)

### Compilation :
```bash
gcc CTree.c -o CTree
./CTree <path>
```

## 💡 | Remarques
**CTree v2.1** est une version d'amélioration visuelle. Les préfixes `L_` / `|` laissent place aux standards `├──` / `└──`, et les fichiers non reconnus sont désormais détectés et affichés en rouge.

## 📜 | Licence
Ce projet est sous licence **MIT** – Vous pouvez faire un peu ce que vous voulez tant que je suis crédité.
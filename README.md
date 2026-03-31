# 🌳 | Ctree
![Badge Langage](https://img.shields.io/badge/Langage%20:-C-blue?style=plastic)
![Badge Licence](https://img.shields.io/badge/Licence%20:-MIT-darkblue?style=plastic)
![Badge Version](https://img.shields.io/badge/Version%20:-v2.0-lim?style=plastic)
![Badge OS](https://img.shields.io/badge/OS%20:-LINUX-blue?style=plastic)
![Badge État](https://img.shields.io/badge/État%20%3A-Released-green?style=plastic)

**CTree** est un outil de visualisation d'arborescence de répertoire Linux

## 📋 | Fonctionnalités de CTree v2.0

| **Paramètres & Fonctionnalités**                                              | **Version** |
|-------------------------------------------------------------------------------|:-----------:|
| [🌲] *Affichage*                                                              |             |
| **Arborescence récursive** avec préfixes `L_` / `\|`                          | `v2.0`      |
| **Coloration** des répertoires en vert                                        | `v1.0`      |
| [⚙️] *Paramètres*                                                             |             |
| **`<path>`** Choix du répertoire à afficher en argument                       | `v2.0`      |
| [🔩] *Technique*                                                              |             |
| **Récursivité** complète via `scanDirectory()`                                | `v2.0`      |
| **Gestion mémoire** complète avec `freeDirectory()`                           | `v2.0`      |
| **Structures** `Directory` et `Element` pour organiser les données            | `v2.0`      |
| **`depthCount()`** Calcul de la profondeur depuis le path                     | `v2.0`      |
| **`printf_GREEN()`** Fonction d'affichage coloré maison avec `va_list`        | `v1.0`      |

## ⚙️ | Fonctionnement et déploiement

### Requis :
- *v2.0* - N'importe quel PC sous Linux (*gcc* conseillée)

### Compilation :
```bash
gcc CTree.c -o CTree
./CTree <path>
```

## 💡 | Remarques
**CTree v2.0** marque le début de la gestion d'affichage dynamique en profondeur. À partir de cette version, la profondeur est représentée par des **tabulations** ce qui permet une meilleure visualisation de l'arborescence. Le path est désormais choisisable en paramètre lors de l'appel du programme.

## 📜 | Licence
Ce projet est sous licence **MIT** – Vous pouvez faire un peu ce que vous voulez tant que je suis crédité.
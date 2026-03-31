# 🌳 | Ctree
![Badge Langage](https://img.shields.io/badge/Langage%20:-C-blue?style=plastic)
![Badge Licence](https://img.shields.io/badge/Licence%20:-MIT-darkblue?style=plastic)
![Badge Version](https://img.shields.io/badge/Version%20:-v1.0-lim?style=plastic)
![Badge OS](https://img.shields.io/badge/OS%20:-LINUX-blue?style=plastic)
![Badge État](https://img.shields.io/badge/État%20%3A-Released-green?style=plastic)

**CTree** est un outil de visualisation d'arborescence de répertoire Linux

## 📋 | Fonctionnalités de CTree v1.0

| **Paramètres & Fonctionnalités**                                              | **Version** |
|-------------------------------------------------------------------------------|:-----------:|
| [🌲] *Affichage*                                                              |             |
| **Arborescence simple** du répertoire courant `./`                            | `v1.0`      |
| **Coloration** des répertoires en vert                                        | `v1.0`      |
| [🔩] *Technique*                                                              |             |
| **Double passage** `readdir` pour compter puis lister les éléments            | `v1.0`      |
| **Distinction** fichier / répertoire via `d_type`                             | `v1.0`      |
| **`printf_GREEN()`** Fonction d'affichage coloré maison avec `va_list`        | `v1.0`      |

## ⚙️ | Fonctionnement et déploiement

### Requis :
- *v1.0* - N'importe quel PC sous Linux (*gcc* conseillée)

### Compilation :
```bash
gcc CTree.c -o CTree
./CTree
```

## 💡 | Remarques
**CTree v1.0** est la première version de CTree, cette dernière n'est pas tres fournis en paramétre (voir pas du tout) mais est fonctionnelle. Pour cette version le path n'est pas choisisable et est hardcoded pour afficher le répertoire courant.

## 📜 | Licence
Ce projet est sous licence **MIT** – Vous pouvez faire un peu ce que vous voulez tant que je suis crédité.
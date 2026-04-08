#ifndef TYPES_H
#define TYPES_H

#include <sys/stat.h>

typedef char* String;

/* TYPE DE DONNEE
* Pour savoir qu'elle type de fichier on vas avoir en face je préfère faire mon propre type pour
* prévoire les cas envisagable
*/
typedef enum {
    T_FILE, T_DIR, T_UNKN
} ElementType;

/* STRUCTURE DES PARAMETRE
* Résumé des différent paramétre d'entrée pour une utilisation booleain dans les fonction utile 
* Presque tout les valeur son des int "booleain" (0 ou 1) sauf pour deph et lenght qui sont de
* entier standare
*/
typedef struct {  
    int sizeParam;
    int pathParam;

    int deepParam;

    int cutParam;
    int cutDepth;
    int cutLenght;

    int banParam;
    int nbBanParam;
    char** banNameParam;

    int searchParam;
    int nbSearchParam;
    char** searchNameParam;

    int statsParam;

    int outParam;

    int errorDouble;    // Erreur si l'utilisateur entre 2x le même param
    int errorNotValide; // Erreur si l'utilisateur entre un param invalide
    int errorCutEntry;  // Erreur si l'utilisateur fais un choix non valide du param Cut (deph = -1000 ou lenght 0) 
    int errorBanEntry;  // Erreur si la liste de param ne prend pas en compte des nom de fichier (-ban puis plus rien comme param)
    int errorSearchEntry;

    int debug;
} Param;

typedef struct Directory Directory;
typedef struct Element Element;

/* TYPE DE REPERTOIRE 
* Chaque repertoire, a en plus des data qui lui son ratachée pour son utilisation telle que la liste
* des élement qui contien (Un repertoire contien des élément -> des fichiers) ainsi quela liste de 
* ses repertoire enfant.
* Il contine aussi le nombre totale de fichier ou de repertoire (Pour des soucis de boucle) ainsi que
* sa profondeur (pour se reperée dans l'architecture).
*/
struct Directory {
    Element *elements;      // Liste des fichier
    Directory *Directorys;  // Liste des repertoire
    String path;            // Son path (Normalement pas utilise vue que il est aussi un élément)
    int depth;              // Sa profondeur par rapport au répertoire racine choisi
    int nbDir;              // Nombre de directory -> Répertoire
    int nbFile;             // Nombre de fichiers
};

/* TYPE DE FICHIER
* chaque elemnt (fichier ou repetoire)) on leur propre data comme leur nom , le path ainsi que leur
* propre type. Utile pour la récursivitée 
*/
struct Element{
    String name;
    ElementType type;
    String path;
    String ext;
    long long size;
    double sizeConverted;
    String sizeStr;

    Directory* subDir;
};

/* NOMBRE DE FOIS PAR EXTENTION
 * Permet de faire les stat sur les extention ...
 */
typedef struct { 
    char* ext; 
    int count; 
} ExtStat;

// Valeur statistique
extern int g_nbDIR;
extern int g_nbFILE;
extern long long int g_globalSize;

  // Tableau des info des extention
extern ExtStat* g_extStats;
extern int g_nbExt;

extern Element g_topFiles[10];
extern int g_topFilesInit;

extern Element* g_searchedElements;
extern int g_searchedElementsIndex;

#endif
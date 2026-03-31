#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>

typedef char* String;

void printf_RGB(int r, int g, int b, const char* format, ...);

void printf_wave_utf8(int r1, int g1, int b1,int r2, int g2, int b2,const char* text,int step);

void sizeToGradientColor(long long size, int* r, int* g, int* b);

/* TYPE DE DONNEE
 * Pour savoir qu'elle type de fichier on vas avoir en face je préfère faire mon propre type pour
 * prévoire les cas envisagable
*/
typedef enum {
    T_FILE, // Fichier normal (txt,mp4 ...)
    T_DIR,  // Fichier repertoire 
    T_UNKN  // Fichier non standare ou non reconnue (cas d'echec principalement)
} ElementType;

typedef enum {
    O,
    KO,
    MO,
    GO,
    TO
} ElementSizeCategory;

typedef struct{
    int sizeParam;
    int pathParam;

    int deepParam;

    int cutParam;
    int depthCutParam;
    int lengthCutParam;

    int errorDouble;
    int errorNotValide;
}Param;

/* TYPE DE FICHIER
 * chaque elemnt (fichier ou repetoire)) on leur propre data comme leur nom , le path ainsi que leur
 * propre type. Utile pour la récursivitée 
*/
typedef struct{
    String name;
    ElementType type;
    String path;
    long long size;
    double sizeConverted;
    ElementSizeCategory sizeCategory;
}Element;

/* TYPE DE REPERTOIRE 
 * Chaque repertoire, a en plus des data qui lui son ratachée pour son utilisation telle que la liste
 * des élement qui contien (Un repertoire contien des élément -> des fichiers) ainsi quela liste de 
 * ses repertoire enfant.
 * Il contine aussi le nombre totale de fichier ou de repertoire (Pour des soucis de boucle) ainsi que
 * sa profondeur (pour se reperée dans l'architecture).
*/
typedef struct Directory Directory;
struct Directory{
    Element *elements;      // Liste des fichier
    Directory *Directorys;  // Liste des repertoire
    String path;            // Son path (Normalement pas utilise vue que il est aussi un élément)
    int depth;              // Sa profondeur par rapport au répertoire racine choisi
    int nbDir;              // Nombre de directory -> Répertoire
    int nbFile;             // Nombre de fichiers
};


Directory scanDirectory(Element element);
void echoDirectory(Directory directory,String prefix,Param p,int depth);
void freeDirectory(Directory* dir);

Param getParameter(int argc, char *argv[]);
ElementSizeCategory getSizeCategory(long long size);
double getDirectorySize(Directory dir);
int depthCount(String path);


////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    // Si le nombre de param est < 2 ou > 3 => Erreur, pas valable
    if(argc < 2){
        printf_RGB(255,0,0,"[!] Error : Mauvaise utilisation de la commande\n");
        return 1;
    }

    int _break = 0;
    Param p;

    if(argc > 2){
        p = getParameter(argc,argv);
    }


    if(p.errorDouble == 1){
        printf_RGB(255,0,0,"[!] Error : Doublon dans les paramétre détecter !\n");
        _break = 1;
    }if(p.errorNotValide == 1){
        printf_RGB(255,0,0,"[!] error : Erreur dans les paramétre détercter !\n");
        printf_RGB(255,0,0,"            s => affichage du poid de chaque fichier\n");
        printf_RGB(255,0,0,"            p => affichage du path de chaque fichier\n");
        printf_RGB(255,0,0,"            d => ...\n");
        printf_RGB(255,0,0,"            c => affichage simplifier pour les plus gros arbres\n");
        _break = 1;
    }
    if(_break == 1){
        return 1;
    }

    // Création du pointeur vers le fichier
    DIR *dir;
    struct dirent *data; // Création 
    struct stat st;

    // Définire le chemin racine passer en paramétre
    String PATH = (String)calloc(1024,sizeof(char));
    strcpy(PATH,argv[1]);

    // On crée un pointeur qui pointe vers le fichier passer en paramétre
    dir = opendir(PATH);
    // Si l'ouverture renvois un NULL alors ca veut dire que le fichier n'existe pas
    if(!dir){
        printf("%s: No such directory\n",argv[1]);
        return 1;
    }
    closedir(dir);

    /*
     * Je crée l'element parent qui a comme nom son chemin (pour simplifier)
     * comme chemin le path passer en paramétre et comme type un repertoire
     * A RETRAVAILLER EN CAS DE PASSAGE D'UN FICHIER SIMPLE !!!
    */
    Element parent;
    parent.name = PATH;
    parent.path = PATH;
    parent.type = T_DIR;

    /*
     * On fais appelle a la fonction scanDirecory qui me permet de remplire
     * le typdef pour l'exploiter juste apres
     */
    Directory Dir = scanDirectory(parent);
    printf_RGB(0,255,0,"%s\n",parent.name); // On affiche le path parent
    echoDirectory(Dir,"",p,0);

    freeDirectory(&Dir);
    free(PATH);
    return 0;
}



////////////////////////////////////////////////////////////////

Directory scanDirectory(Element element){
    DIR *dir;  
    Directory NewDirectory;

    //On alloue de la place pour le path du directory parent
    NewDirectory.path = calloc(1024,sizeof(char));
    strcpy(NewDirectory.path,element.path);
    
    // Depuis le path de l'élement parent on détermine sa profondeur (Le nombre de /);
    NewDirectory.depth = depthCount(NewDirectory.path);

    NewDirectory.nbDir = 0;
    NewDirectory.nbFile = 0;

    struct dirent *data;

    dir = opendir(NewDirectory.path);

    while((data = readdir(dir)) != NULL){
        if(strcmp(data->d_name, ".") == 0 || strcmp(data->d_name, "..") == 0){
            continue;
        }else{
            if(data->d_type == DT_DIR){
                NewDirectory.nbDir ++;
            }else if(data->d_type == DT_REG){
                NewDirectory.nbFile ++;
            }else if(data->d_type == DT_UNKNOWN){
                continue;
            }
        }
    }
    rewinddir(dir);
    NewDirectory.elements = calloc(NewDirectory.nbDir + NewDirectory.nbFile,sizeof(Element));
    NewDirectory.Directorys = calloc(NewDirectory.nbDir,sizeof(Directory));

    int i = 0;
    int IndexDire = 0;
    while((data = readdir(dir)) != NULL){
        if(strcmp(data->d_name, ".") == 0 || strcmp(data->d_name, "..") == 0){
            continue;
        }

        NewDirectory.elements[i].name = calloc(1024,sizeof(char));
        strcpy(NewDirectory.elements[i].name, data->d_name);
        
        NewDirectory.elements[i].path = calloc(1024,sizeof(char));
        strcpy(NewDirectory.elements[i].path,NewDirectory.path);
        strcat(NewDirectory.elements[i].path,"/");
        strcat(NewDirectory.elements[i].path,NewDirectory.elements[i].name);

        if(data->d_type == DT_DIR){
            NewDirectory.elements[i].type = T_DIR;

            // Scanner le sous-répertoire
            Directory subDir = scanDirectory(NewDirectory.elements[i]);
            NewDirectory.Directorys[IndexDire] = subDir;
            IndexDire++;

            // Calculer la taille réelle du sous-répertoire
            double dirSize = getDirectorySize(subDir);
            NewDirectory.elements[i].size = dirSize;
            NewDirectory.elements[i].sizeCategory = getSizeCategory((long long)dirSize);

            switch(NewDirectory.elements[i].sizeCategory) {
                case O:  
                    NewDirectory.elements[i].sizeConverted = dirSize;
                    break;
                case KO: 
                    NewDirectory.elements[i].sizeConverted = dirSize / 1024.0;
                    break;
                case MO: 
                    NewDirectory.elements[i].sizeConverted = dirSize / (1024.0*1024.0);
                    break;
                case GO: 
                    NewDirectory.elements[i].sizeConverted = dirSize / (1024.0*1024.0*1024.0);
                    break;
                case TO: 
                    NewDirectory.elements[i].sizeConverted = dirSize / (1024.0*1024.0*1024.0*1024.0);
                    break;
                default: 
                    NewDirectory.elements[i].sizeConverted = dirSize;
                    break;
            }
        }else if(data->d_type == DT_REG){
            NewDirectory.elements[i].type = T_FILE;

            struct stat st;
            stat(NewDirectory.elements[i].path, &st);
            long long buffer = st.st_size;
            NewDirectory.elements[i].size = buffer;
            NewDirectory.elements[i].sizeCategory = getSizeCategory(buffer);
            switch(NewDirectory.elements[i].sizeCategory) {
                case O:  
                    NewDirectory.elements[i].sizeConverted = (double)st.st_size; 
                break;
                case KO: 
                    NewDirectory.elements[i].sizeConverted = (double)st.st_size / 1024.0; 
                break;
                case MO: 
                    NewDirectory.elements[i].sizeConverted = (double)st.st_size / (1024.0*1024.0); 
                break;
                case GO: 
                    NewDirectory.elements[i].sizeConverted = (double)st.st_size / (1024.0*1024.0*1024.0); 
                break;
                case TO: 
                    NewDirectory.elements[i].sizeConverted = (double)st.st_size / (1024.0*1024.0*1024.0*1024.0); 
                break;
                default: 
                    NewDirectory.elements[i].sizeConverted = (double)st.st_size; 
                break;
            }
        }else{
            NewDirectory.elements[i].type = T_UNKN;
        }
        i++;
    }
    closedir(dir);
    return NewDirectory;
}

void echoDirectory(Directory directory,String prefix,Param p,int depth){
    int nbElement = directory.nbDir + directory.nbFile;
    int nbDir = directory.nbDir;

    int IndexDir = 0;

    for(int i=0;i<nbElement;i++){

        int last = (i == nbElement - 1);
        
        printf("%s",prefix);

        if(last){
            printf("└── ");
        }else{
            printf("├── ");
        }
        
        if(directory.elements[i].type == T_FILE){
            printf("%s  ",directory.elements[i].name);
            if(p.pathParam == 1){
                String bufferName = (String)calloc(strlen(directory.elements[i].path) + 4,sizeof(char));
                strcat(bufferName,directory.elements[i].path);
                strcat(bufferName,"   ");
                printf_wave_utf8(0,0,255,255,0,255,bufferName,2);
                free(bufferName);
            }if(p.sizeParam == 1){
                int r,g,b;
                sizeToGradientColor(directory.elements[i].size, &r, &g, &b);
                switch(directory.elements[i].sizeCategory){
                    case O:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"O   ");
                    break;
                    case KO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"KO   ");
                    break;
                    case MO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"MO   ");
                    break;
                    case GO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"GO   ");
                    break;
                    case TO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"TO   ");
                    break;
                    default:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"Error   ");
                }
            }
            printf("\n");
        }else if(directory.elements[i].type == T_DIR){
            printf_RGB(0,255,0,"%s  ",directory.elements[i].name);
            if(p.pathParam == 1){
                String bufferName = (String)calloc(strlen(directory.elements[i].path) + 4,sizeof(char));
                strcat(bufferName,directory.elements[i].path);
                strcat(bufferName,"   ");
                printf_wave_utf8(255,0,255,0,0,255,bufferName,2);
                free(bufferName);
            }if(p.sizeParam == 1){
                int r,g,b;
                sizeToGradientColor(directory.elements[i].size, &r, &g, &b);
                switch(directory.elements[i].sizeCategory){
                    case O:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"O   ");
                    break;
                    case KO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"KO   ");
                    break;
                    case MO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"MO   ");
                    break;
                    case GO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"GO   ");
                    break;
                    case TO:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"TO   ");
                    break;
                    default:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"Error   ");
                }
            }
            printf("\n");
            String NewPrefix = (String)calloc(1024,sizeof(char));
            strcpy(NewPrefix,prefix);
            if(last){
                strcat(NewPrefix,"   ");
            }else{
                strcat(NewPrefix,"|  ");
            }
            echoDirectory(directory.Directorys[IndexDir],NewPrefix,p,depth + 1);
            free(NewPrefix);
            IndexDir++;
        }else if(directory.elements[i].type == DT_UNKNOWN){
            printf_RGB(255,0,0,"%s\n",directory.elements[i].name);
        }
    }
}

void freeDirectory(Directory* dir) {
    for(int i=0;i<dir->nbDir;i++)
        freeDirectory(&dir->Directorys[i]);
    for(int i=0;i<dir->nbDir + dir->nbFile;i++) {
        free(dir->elements[i].name);
        free(dir->elements[i].path);
    }
    free(dir->elements);
    free(dir->Directorys);
    free(dir->path);
}


int depthCount(String path){
    int depth = 0;
    int i = 0;

    if(path[i] == '.' && path[i+1] == '/'){
            i = 2;
        }

    for(;i<strlen(path);i++){
        if(path[i] == '/'){
            depth ++;
        }
    }

    return depth;
}

double getDirectorySize(Directory dir) {
    double totalSize = 0.0;
    
    // Additionner seulement les FICHIERS du répertoire courant
    for(int i = 0; i < dir.nbFile + dir.nbDir; i++) {
        if(dir.elements[i].type == T_FILE) {  // ← Ajoute cette condition !
            totalSize += dir.elements[i].size;
        }
    }
    
    // Additionner récursivement les sous-répertoires
    for(int i = 0; i < dir.nbDir; i++) {
        totalSize += getDirectorySize(dir.Directorys[i]);
    }
    
    return totalSize;
}

ElementSizeCategory getSizeCategory(long long size)
{
    const long long  SIZE_KO = 1024LL;
    const long long  SIZE_MO = 1024LL * 1024LL;
    const long long  SIZE_GO = 1024LL * 1024LL * 1024LL;
    const long long  SIZE_TO = 1024LL * 1024LL * 1024LL * 1024LL;

    if (size < SIZE_KO)
        return O;
    else if (size < SIZE_MO)
        return KO;
    else if (size < SIZE_GO)
        return MO;
    else if (size < SIZE_TO)
        return GO;
    else
        return TO;
}

Param getParameter(int argc,char* argv[]){
    Param param;
    param.errorDouble = 0;
    param.errorNotValide = 0;

    param.sizeParam = 0;
    param.pathParam = 0;
    param.deepParam = 0;
    param.cutParam = 0;

    for(int i = 2; i < argc; i++) {
        if(strcmp(argv[i], "-path") == 0) {
            if(param.pathParam) {
                param.errorDouble = 1;
            } else {
                param.pathParam = 1;
            }
        } else if(strcmp(argv[i], "-size") == 0) {
            if(param.sizeParam) {
                param.errorDouble = 1;
            } else {
                param.sizeParam = 1;
            }
        } else if(strcmp(argv[i], "-cut") == 0) {
            if(param.cutParam) {
                param.errorDouble = 1;
            } else {
                param.cutParam = 1;
            }
        } else if(strcmp(argv[i], "-deep") == 0) {
            if(param.deepParam || param.pathParam || param.sizeParam) {
                param.errorDouble = 1;
            } else {
                param.deepParam = 1;
                param.pathParam = 1;
                param.sizeParam = 1;
            }
        } else {
            param.errorNotValide = 1;
        }

        if(param.errorDouble || param.errorNotValide) {
            break;
        }
    }


    return param;
}


void printf_RGB(int r, int g, int b, const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("\033[38;2;%d;%d;%dm", r, g, b);

    vprintf(format, args);

    printf("\033[0m");

    va_end(args);
}

void printf_wave_utf8(int r1, int g1, int b1,
                      int r2, int g2, int b2,
                      const char* text,
                      int step) {
    const char *p = text;
    
    // compter le nombre de caractères pour le gradient
    int char_count = 0;
    while (*p) {
        unsigned char c = (unsigned char)*p;
        if ((c & 0xC0) != 0x80) char_count++; // uniquement le premier octet d'un caractère
        p++;
    }

    p = text;
    int index = 0;
    while (*p) {
        unsigned char c = (unsigned char)*p;
        int bytes = 1;
        if (c >= 0xF0) bytes = 4;
        else if (c >= 0xE0) bytes = 3;
        else if (c >= 0xC0) bytes = 2;

        // gradient
        int r = r1 + (r2 - r1) * index / (char_count - 1);
        int g = g1 + (g2 - g1) * index / (char_count - 1);
        int b = b1 + (b2 - b1) * index / (char_count - 1);

        // step
        if (r1 < r2) r += step * index; else r -= step * index;
        if (g1 < g2) g += step * index; else g -= step * index;
        if (b1 < b2) b += step * index; else b -= step * index;

        // clamp
        r = (r>255)?255:(r<0?0:r);
        g = (g>255)?255:(g<0?0:g);
        b = (b>255)?255:(b<0?0:b);

        // afficher le caractère complet
        printf("\033[38;2;%d;%d;%dm", r, g, b); // juste le code couleur
        printf("%.*s", bytes, p);
        p += bytes;
        index++;
    }
}

void sizeToGradientColor(long long size, int* r, int* g, int* b) {
    // Définir les couleurs de début et fin pour chaque catégorie
    int r0, g0, b0; // début
    int r1, g1, b1; // fin
    long long min, max;

    // Définition globale des bornes
    const long long SIZE_O  = 1024LL;                       // 1 Ko
    const long long SIZE_KO = 1024LL * 1024LL;              // 1 Mo
    const long long SIZE_MO = 1024LL * 1024LL * 1024LL;     // 1 Go
    const long long SIZE_GO = 1024LL * 1024LL * 1024LL * 1024LL; // 1 To

    if(size < SIZE_O){
        min = 0; max = SIZE_O;
        r0=0; g0=255; b0=0;      // vert
        r1=128; g1=255; b1=0;    // vert-jaune
    } else if(size < SIZE_KO){
        min = SIZE_O; max = SIZE_KO;
        r0=128; g0=255; b0=0;    // vert-jaune
        r1=255; g1=255; b1=0;    // jaune
    } else if(size < SIZE_MO){
        min = SIZE_KO; max = SIZE_MO;
        r0=255; g0=255; b0=0;    // jaune
        r1=255; g1=128; b1=0;    // orange
    } else if(size < SIZE_GO){
        min = SIZE_MO; max = SIZE_GO;
        r0=255; g0=128; b0=0;    // orange
        r1=255; g1=0; b1=0;      // rouge
    } else {
        min = SIZE_GO; max = SIZE_GO*10; // bornes arbitraires pour les très gros fichiers
        r0=255; g0=0; b0=0;      // rouge
        r1=128; g1=0; b1=0;      // rouge foncé
    }

    // Calculer ratio
    double ratio = (double)(size - min) / (double)(max - min);
    if(ratio > 1.0) ratio = 1.0;
    if(ratio < 0.0) ratio = 0.0;

    // Calculer couleur intermédiaire
    *r = (int)(r0 + (r1 - r0) * ratio);
    *g = (int)(g0 + (g1 - g0) * ratio);
    *b = (int)(b0 + (b1 - b0) * ratio);
}
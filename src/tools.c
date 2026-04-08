#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "tools.h"

/* depthCount
* Fonction qui donne la profondeur d'un fichier depuis son path
*/
int depthCount(String path){
    int depth = 0;
    int i = 0;

    if(path[i] == '.' && path[i+1] == '/'){
            i = 2;
        }

    for(;i<(int)strlen(path);i++){
        if(path[i] == '/'){
            depth ++;
        }
    }

    return depth;
}

/* getDirectorySize
* Donne le poids d'un répertoire
*/
double getDirectorySize(Directory dir) {
    double totalSize = 0.0;
    
    // Additionner seulement les FICHIERS du répertoire courant
    for(int i = 0; i < dir.nbFile + dir.nbDir; i++) {
        if(dir.elements[i].type == T_FILE) {
            totalSize += dir.elements[i].size;
        }
    }
    
    // Additionner récursivement les sous-répertoires
    for(int i = 0; i < dir.nbDir; i++) {
        totalSize += getDirectorySize(dir.Directorys[i]);
    }
    
    return totalSize;
}

/* getSizeCategory
* Donne la catégorie du poids d'un fichier
*/
String getSizeStr(long long size){
    char* str = calloc(20, sizeof(char));
    if(size < 1024LL)
        snprintf(str, 20, "%.2f O", (double)size);
    else if(size < 1024LL*1024LL)
        snprintf(str, 20, "%.2f KO", (double)size / 1024.0);
    else if(size < 1024LL*1024LL*1024LL)
        snprintf(str, 20, "%.2f MO", (double)size / (1024.0*1024.0));
    else if(size < 1024LL*1024LL*1024LL*1024LL)
        snprintf(str, 20, "%.2f GO", (double)size / (1024.0*1024.0*1024.0));
    else
        snprintf(str, 20, "%.2f TO", (double)size / (1024.0*1024.0*1024.0*1024.0));
    return str;
}
/* getParameter
* Traite les paramétre passée en paramétre a l'appelle de la fonction et renvois une structure
* décrivant tout les paramétre associée 
*/
Param getParameter(int argc,char* argv[]){
    Param param;
    param.errorDouble      = 0;
    param.errorNotValide   = 0;
    param.errorCutEntry    = 0;
    param.errorBanEntry    = 0;
    param.errorSearchEntry = 0;

    param.sizeParam   = 0;
    param.pathParam   = 0;
    param.deepParam   = 0;
    param.cutParam    = 0;

    param.cutDepth    = 10;
    param.cutLenght   = 10;

    param.debug       = 0;
    param.banParam    = 0;
    param.searchParam = 0;

    param.statsParam  = 0;

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
            }else if (i + 2 >= argc) {
                // Pas assez d'arguments après -cut
                param.errorCutEntry = 1;
            } else {
                param.cutParam = 1;
                int buffer = _atoi(argv[i+1]);
                if(buffer == -32767){
                    param.errorCutEntry = 1;
                }else{
                    if(_atoi(argv[i+1]) == 0){
                        param.cutDepth = 32767;
                    }else{
                        param.cutDepth = _atoi(argv[i+1]); 
                    }   
                }
                buffer = _atoi(argv[i+2]);
                if(buffer == -32767){
                    param.errorCutEntry = 1;
                }else{
                    if(_atoi(argv[i+2]) == 0){
                        param.cutLenght = 32767;
                    }else{
                        param.cutLenght = _atoi(argv[i+2]);
                    }  
                }
                i += 2;
            }
        } else if(strcmp(argv[i], "-deep") == 0) {
            if(param.deepParam || param.pathParam || param.sizeParam) {
                param.errorDouble = 1;
            } else {
                param.deepParam = 1;
                param.pathParam = 1;
                param.sizeParam = 1;
            }
        }else if(strcmp(argv[i], "-debug") == 0){
            if(param.debug){
                param.errorDouble = 1;
            }else{
                param.debug = 1;
            }
        } else if(strcmp(argv[i], "-ban") == 0) {
            if(i + 1 >= argc || argv[i+1][0] == '-'){
                param.errorBanEntry = 1;
            }else{
                param.banParam = 1;
                int indexParamBan = i + 1;
                param.nbBanParam = 0;
                while(indexParamBan < argc){
                    if(argv[indexParamBan][0] == '-') break;
                    param.nbBanParam++;
                    indexParamBan++;
                }
                param.banNameParam = (char**)calloc(param.nbBanParam, sizeof(char*));
                int index = 0;
                for(int y = i+1; y < indexParamBan; y++){
                    param.banNameParam[index] = calloc(strlen(argv[y]) + 1, sizeof(char));
                    strcpy(param.banNameParam[index], argv[y]);
                    index++;
                }
                i = indexParamBan - 1;  // ← MANQUANT, sauter les args consommés
            }
        } else if(strcmp(argv[i], "-search") == 0) {
            if(i + 1 >= argc || argv[i+1][0] == '-'){
                param.errorSearchEntry = 1;
            }else{
                param.searchParam = 1;
                int indexParamSearch = i + 1;
                param.nbSearchParam = 0;
                while(indexParamSearch < argc){
                    if(argv[indexParamSearch][0] == '-') break;
                    param.nbSearchParam++;
                    indexParamSearch++;
                }
                param.searchNameParam = (char**)calloc(param.nbSearchParam, sizeof(char*));
                int index = 0;
                for(int y = i+1; y < indexParamSearch; y++){
                    param.searchNameParam[index] = calloc(strlen(argv[y]) + 1, sizeof(char));
                    strcpy(param.searchNameParam[index], argv[y]);
                    index++;
                }
                i = indexParamSearch - 1;
            }
        }else if(strcmp(argv[i], "-stats") == 0){
            if(param.statsParam == 1){
                param.errorDouble = 1;
                break;
            }else{
                param.statsParam = 1;
            }
        }else if(strcmp(argv[i], "-out") == 0){
            if(param.outParam == 1){
                param.errorDouble = 1;
                break;
            }
            param.outParam = 1;
        }else{
            param.errorNotValide = 1;
        }

        if(param.errorDouble || param.errorNotValide) {
            break;
        }
    }


    return param;
}

/* _atoi
* ma version de atoi par ce que j'adore réinventais la roue lol
*/
int _atoi(char *input){
    if (input == NULL || input[0] == '\0')
        return -32767;

    int sign = 1;
    int i = 0;

    // Gérer le signe négatif
    if (input[0] == '-') {
        sign = -1;
        i = 1;

        // Cas "-" seul => invalide
        if (input[1] == '\0')
            return -32767;
    }

    int value = 0;

    // Vérifier puis convertir en une seule passe
    for (; input[i] != '\0'; i++) {

        if (input[i] < '0' || input[i] > '9')
            return -32767;

        value = value * 10 + (input[i] - '0');
    }

    return sign * value;
}

String getExt(char* str){
    int lastDot = -1;

    for(int i=0; i<(int)strlen(str); i++){
        if(str[i] == '.') lastDot = i;
    }

    if(lastDot == -1){
        char* ext = calloc(1, sizeof(char));
        return ext;
    }

    char* ext = calloc(strlen(str) - lastDot + 1, sizeof(char));
    strcpy(ext, str + lastDot);
    return ext;
}

void addExt(char* ext){
    if(ext == NULL || ext[0] == '\0'){
        ext = ".";
    };
    for(int i=0;i<g_nbExt;i++){
        if(strcmp(g_extStats[i].ext,ext) == 0){
            g_extStats[i].count++;
            return;
        }
    }

    g_nbExt++;
    g_extStats = realloc(g_extStats, g_nbExt * sizeof(ExtStat));
    g_extStats[g_nbExt-1].ext = calloc(strlen(ext)+1, sizeof(char)); // ← toujours manquant
    strcpy(g_extStats[g_nbExt-1].ext, ext);
    g_extStats[g_nbExt-1].count = 1;
}

void triABulleExt(){
    for(int i = 0; i < g_nbExt - 1; i++){
        for(int y = 0; y < g_nbExt - i - 1; y++){
            if(g_extStats[y].count < g_extStats[y+1].count){
                ExtStat tmp = g_extStats[y];
                g_extStats[y] = g_extStats[y+1];
                g_extStats[y+1] = tmp;
            }
        }
    }
}

/* addToTop10
* Insère un élément dans le top 10 des fichiers les plus lourds
* O(1) par fichier, pas de stockage de tous les fichiers
*/
void addToTop10(Element element) {
    if(!g_topFilesInit){
        for(int i=0;i<10;i++) g_topFiles[i].size = 0;
        g_topFilesInit = 1;
    }
    if(element.size <= g_topFiles[9].size) return;
    
    // Dupliquer les strings avant insertion
    Element copy = element;
    copy.name    = strdup(element.name);
    copy.path    = strdup(element.path);
    copy.sizeStr = strdup(element.sizeStr);
    copy.ext     = strdup(element.ext);
    
    for(int i=0;i<10;i++){
        if(copy.size > g_topFiles[i].size){
            // Libérer l'élément éjecté s'il est valide
            if(g_topFiles[9].size > 0){
                free(g_topFiles[9].name);
                free(g_topFiles[9].path);
                free(g_topFiles[9].sizeStr);
                free(g_topFiles[9].ext);
            }
            for(int y=9;y>i;y--)
                g_topFiles[y] = g_topFiles[y-1];
            g_topFiles[i] = copy;
            return;
        }
    }
    // Si pas inséré, libérer la copie
    free(copy.name); free(copy.path);
    free(copy.sizeStr); free(copy.ext);
}

int nbDigit(int nb){
    if(nb == 0){
        return 1;
    }

    int count = 0;
    while(nb >0){
        nb/=10;
        count ++;
    }

    return count;
}

String intToString(int NbINT){
    long BufferINT = NbINT;
    int NbCar = 1;

    // Si négatif, réserver une place pour '-'
    if(BufferINT < 0){
        NbCar++;
        BufferINT = -BufferINT; // on travaille sur la valeur positive
    }

    long Temp = BufferINT;
    while(Temp/10 != 0){
        Temp /= 10;
        NbCar++;
    }

    String NbSTR = (String)calloc(NbCar+1, sizeof(char));
    NbSTR[NbCar] = '\0';

    int i = NbCar - 1;
    do{
        NbSTR[i] = (BufferINT % 10) + '0';
        BufferINT /= 10;
        i--;
    } while(BufferINT > 0);

    // Mettre le signe '-' si nécessaire
    if(NbINT < 0){
        NbSTR[0] = '-';
    }

    return NbSTR;
}

int strlenVis(const char* s){
    int count = 0;
    const unsigned char* p = (const unsigned char*)s;
    while(*p){
        if((*p & 0xC0) != 0x80) count++;
        p++;
    }
    return count;
}

int strpartcmp(char* src, char* str) {
    int srcLen = (int)strlen(src);
    int strLen = (int)strlen(str);

    for (int i = 0; i <= srcLen - strLen; i++) {
        if (src[i] == str[0]) {
            int match = 1;
            for (int y = 1; y < strLen; y++) {
                if (src[i + y] != str[y]) {
                    match = 0;
                    break;
                }
            }
            if (match) return 1;
        }
    }
    return 0;
}

int compareElements(const void *a, const void *b) {
    const Element *elemA = (const Element *)a;
    const Element *elemB = (const Element *)b;
    return strcmp(elemA->name, elemB->name);
}

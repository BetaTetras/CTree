#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>

typedef char* String;

void printf_RGB(int r, int g, int b, const char* format, ...);

// Type de donnée
typedef enum {
    T_FILE,
    T_DIR,
    T_UNKN
} ElementType;

// Type des fichiée
typedef struct{
    String name;
    ElementType type;
    String path;
}Element;

//Type des répertoire
typedef struct Directory Directory;
struct Directory{
    Element *elements;
    Directory *Directorys;
    String path;
    int depth;
    int nbDir;
    int nbFile;
};


Directory scanDirectory(Element element);
void echoDirectory(Directory directory,String prefix,int depth);
void freeDirectory(Directory* dir);

int depthCount(String path);


////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    // Si le nombre de param est < 2 ou > 3 => Erreur, pas valable
    if(argc < 2 || argc > 3){
        printf("[!] Error : Mauvaise utilisation de la commande\n");
        return 1;
    }

    // Vérification du paramètre d'affichage profonde (Taille des fichier WORK IN PROGRESSE)
    int BoolDeep = 0;
    if(strncmp(argv[2],"-deep",5) == 0){ // Si le deuxième param == "-deep" alors l'activée
        BoolDeep = 1;
    }

    // Création du pointeur vers le fichier
    DIR *dir;
    struct dirent *data; // Création 
    struct stat st;

    // Définire le 
    String PATH = (String)calloc(1024,sizeof(char));
    strcpy(PATH,argv[1]);

    dir = opendir(PATH);
    if(!dir){
        printf("%s: No such directory\n",argv[1]);
        return 1;
    }
    closedir(dir);

    Element parent;
    parent.name = PATH;
    parent.path = PATH;
    parent.type = T_DIR;

    Directory Dir = scanDirectory(parent);
    printf_RGB(0,255,0,"%s\n",parent.name);
    echoDirectory(Dir,"",0);

    freeDirectory(&Dir);
    free(PATH);
    return 0;
}



////////////////////////////////////////////////////////////////

Directory scanDirectory(Element element){
    DIR *dir;
    Directory NewDirectory;

    NewDirectory.path = calloc(1024,sizeof(char));
    strcpy(NewDirectory.path,element.path);
    
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
            NewDirectory.Directorys[IndexDire] = scanDirectory(NewDirectory.elements[i]);
            IndexDire ++;
        }else if(data->d_type == DT_REG){
            NewDirectory.elements[i].type = T_FILE;
        }else{
            NewDirectory.elements[i].type = T_UNKN;
        }
        i++;
    }
    closedir(dir);
    return NewDirectory;
}

void echoDirectory(Directory directory,String prefix,int depth){
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
            printf("%s\n",directory.elements[i].name);
        }else if(directory.elements[i].type == T_DIR){
            printf_RGB(0,255,0,"%s\n",directory.elements[i].name);
            String NewPrefix = (String)calloc(1024,sizeof(char));
            strcpy(NewPrefix,prefix);
            if(last){
                strcat(NewPrefix,"   ");
            }else{
                strcat(NewPrefix,"|  ");
            }
            echoDirectory(directory.Directorys[IndexDir],NewPrefix,depth + 1);
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


void printf_RGB(int r, int g, int b, const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("\033[38;2;%d;%d;%dm", r, g, b);

    vprintf(format, args);

    printf("\033[0m");

    va_end(args);
}
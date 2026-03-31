#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>

typedef char* String;

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

void printf_GREEN(char* Text,...);
void printf_RED(char* Text,...);

Directory scanDirectory(Element element);
void echoDirectory(Directory directory,String prefix,int depth);
void freeDirectory(Directory* dir);

int depthCount(String path);


////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    if(argc > 2 && argc != 1){
        printf("[!] Error : UN argument autoriser (PATH)\n");
    }

    DIR *dir;
    struct dirent *data;

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
    printf_GREEN("%s\n",parent.name);
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
            printf_GREEN("%s\n",directory.elements[i].name);
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
            printf_RED("%s\n",directory.elements[i].name);
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

void printf_GREEN(char* Text,...){
    va_list Args;
    va_start(Args,Text);

    for(int i=0;i<(int)strlen(Text);i++){
        if(Text[i] == '%'){
            if(Text[i+1] == 'd'){
                int _int = va_arg(Args,int);
                printf("\033[1;32m%d\033[0m",_int);
                i ++;
            }else if(Text[i+1] == 'f'){
                double _double = va_arg(Args,double);
                printf("\033[1;32m%f\033[0m",_double);
                i ++;
            }else if(Text[i+1] == 's'){
                char* _String = va_arg(Args,char*);
                printf("\033[1;32m%s\033[0m",_String);
                i ++;
            }else if(Text[i+1] == 'c'){
                int _char = va_arg(Args,int);
                printf("\033[1;32m%c\033[0m",_char);
                i++;
            }
        }else{
            printf("\033[1;32m%c\033[0m",Text[i]);
        }
    }

    va_end(Args);
}

void printf_RED(char* Text,...){
    va_list Args;
    va_start(Args,Text);

    for(int i=0;i<(int)strlen(Text);i++){
        if(Text[i] == '%'){
            if(Text[i+1] == 'd'){
                int _int = va_arg(Args,int);
                printf("\033[1;31m%d\033[0m",_int);
                i ++;
            }else if(Text[i+1] == 'f'){
                double _double = va_arg(Args,double);
                printf("\033[1;31m%f\033[0m",_double);
                i ++;
            }else if(Text[i+1] == 's'){
                char* _String = va_arg(Args,char*);
                printf("\033[1;31m%s\033[0m",_String);
                i ++;
            }else if(Text[i+1] == 'c'){
                int _char = va_arg(Args,int);
                printf("\033[1;31m%c\033[0m",_char);
                i++;
            }
        }else{
            printf("\033[1;31m%c\033[0m",Text[i]);
        }
    }

    va_end(Args);
}
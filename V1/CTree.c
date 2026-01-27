#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>

typedef char* String;

typedef struct{
    String name;
    int type;
    String path;
    int depth;
}FileEntry;

void printf_GREEN(char* Text,...);
void printDIR(FileEntry *Tree,int nbElement,String parentPath);


////////////////////////////////////////////////////////////////

int main(void){
    DIR *dir;
    const String PATH = "./";
    struct dirent *data;

    String bufferFileName = (String)calloc(256,sizeof(char));
    int nbFile = 0;
    int nbDir = 0;

    dir = opendir(PATH);

    while((data = readdir(dir)) != NULL){
        strcpy(bufferFileName,data->d_name);
        if(strcmp(bufferFileName,".") == 0 || strcmp(bufferFileName,"..") == 0){
            continue;
        }else{
            if(data->d_type == DT_DIR){
                nbDir++;
            }else if(data->d_type == DT_REG){
                nbFile ++;
            }
        }
    }
    rewinddir(dir);
    FileEntry *Tree = calloc(nbDir+nbFile,sizeof(FileEntry));

    int i = 0;
    while((data = readdir(dir)) != NULL){
        if(strcmp(data->d_name, ".") == 0 || strcmp(data->d_name, "..") == 0)
            continue;

        Tree[i].name = calloc(256,sizeof(char));
        strcpy(Tree[i].name, data->d_name);

        if(data->d_type == DT_DIR){
            Tree[i].type = 1;
        }
        else if(data->d_type == DT_REG){
            Tree[i].type = 0;
        }
            
        i++;
    }

    printDIR(Tree, nbDir+nbFile,PATH);

    return 0;
}


////////////////////////////////////////////////////////////////

void printDIR(FileEntry *Tree,int nbElement,String parentPath){

    printf_GREEN(parentPath);
    printf("\n");

    for(int i=0;i<nbElement;i++){
        if(Tree[i].type == 1){
            printf_GREEN("    %s\n",Tree[i].name);
        }else if(Tree[i].type == 0){
            printf("    %s\n",Tree[i].name);
        }
    }
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
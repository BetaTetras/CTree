#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "types.h"
#include "tools.h"
#include "output.h"

Directory scanDirectory(Element element,Param p);
void echoDirectory(Directory directory,String prefix,Param p,int depth);
void freeDirectory(Directory* dir);

void* animLoader(void* arg);

int g_nbDIR = 0;
int g_nbFILE = 0;
long long int g_globalSize = 0;
ExtStat* g_extStats = NULL;
int g_nbExt = 0;
Element g_topFiles[10];
int g_topFilesInit = 0;
Element* g_searchedElements = NULL;
int g_searchedElementsIndex = 0;

FILE* fichier = NULL;

int g_scanning = 1;

int main(int argc, char *argv[]) {
    clock_t start = clock();

    if(argc == 1 || strcmp(argv[1],"-info") == 0){
        printf_wave_utf8(178,0,255,0,38,255,"┌─ CTree project v5.1 ─────────────────────────────────────────┐\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ Coded with <3 by BetaTetras (https://github.com/BetaTetras)  │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ Tree folder visualizer made in C with specific parameters.   │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│                                                              │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ Usage: CTree <path> [options]                                │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ Ex   : CTree ./myFolder -deep -cut 3 10                      │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│                                                              │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ -path ......: Show path of each file                         │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ -size ......: Show size of directories and files             │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ -deep ......: Show both size and path                        │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ -cut X Y ...: Limit depth and/or length of the tree          │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│       => X -> Max depth  (0 = unlimited)                     │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│       => Y -> Max length (0 = unlimited)                     │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ -ban [...] .: Ban files from the tree                        │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ -stats .....: Debug mode (in case of error)                  │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"│ -debug ....: Debug mode (in case of error)                   │\n",1,NULL,NULL,NULL);
        printf_wave_utf8(178,0,255,0,38,255,"└──────────────────────────────────────────────────────────────┘\n",1,NULL,NULL,NULL);

        return 0;
    }

    // Si le nombre de param est < 2 pas valable
    if(argc < 2){
        printf_RGB(255,0,0,"[!] Error : Mauvaise utilisation de la commande\n");
        return 1;
    }

    // Struc des paramétre nommée "p"
    Param p = {0};  
    
    // Init tu tab des Info concenée cherchée 
    g_searchedElements = (Element*)calloc(2,sizeof(Element));

    // Si l'utilisateur a entrée des param alors on lance la fonction getParameter qui vas analisée les entrée des param et determinée les param
    if(argc > 2){
        p = getParameter(argc,argv);
    }

    // Détection des erreur...
    if(p.errorDouble == 1){
        printf_RGB(255,0,0,"[!] Error : duplicate detected !\n");
        return 1;
    }
    
    if(p.errorNotValide == 1 || p.errorCutEntry == 1){
        printf_RGB(255,0,0,"[!] error : Error inside of the parameter declaration  !\n");
        printf_RGB(255,0,0,"            -size => Show the size of a file or a directory\n");
        printf_RGB(255,0,0,"            -path => Show the path of a file or a directory\n");
        printf_RGB(255,0,0,"            -deep => Show the size & the path\n");
        printf_RGB(255,0,0,"            -cut  => cut a part of the tree\n");
        printf_RGB(255,0,0,"                -depthX  => max depth of the tree\n");
        printf_RGB(255,0,0,"                -lenghtY => max Lenght of the tree\n");
        printf_RGB(255,0,0,"            -ban   => Ban name from the tree \n");
        printf_RGB(255,0,0,"            -stats => Show directory datas\n");
        printf_RGB(255,0,0,"            -debug => Show the debug of the tree \n");
        return 1;
    }
    
    if(p.errorBanEntry == 1){
        printf_RGB(255,0,0,"[!] error : Error from the ban parameter  !\n");
        printf_RGB(255,0,0,"            Make sure to put at least one \n");
        return 1;
    }

    if(p.errorSearchEntry == 1){
        printf_RGB(255,0,0,"[!] error : Error from the search parameter !\n");
        printf_RGB(255,0,0,"            Make sure to put at least one \n");
        return 1;
    }
    // Création du pointeur vers le fichier
    DIR *dir;

    // Définire le chemin racine passer en paramétre
    String PATH = (String)calloc(4096,sizeof(char));
    strcpy(PATH,argv[1]);

    if(p.outParam == 1){
        // Retirer le slash final si présent
        int pathLen = strlen(PATH);
        if(PATH[pathLen - 1] == '/'){
            PATH[pathLen - 1] = '\0';
        }

        // Extraire le nom du dossier
        String folderName = strrchr(PATH, '/');
        if(folderName != NULL){
            folderName++;
        }else{
            folderName = PATH;
        }

        // Récupérer la date
        time_t t = time(NULL);
        struct tm* tm_info = localtime(&t);
        char date[20];
        strftime(date, 20, "%d-%m-%Y %H-%M-%S", tm_info);

        // Construire le nom : "[DD-MM-YYYY HH-MM-SS] - NomFichier.tree"
        String fileName = (String)calloc(strlen(folderName) + strlen(date) + 20, sizeof(char));
        strcpy(fileName, "[");
        strcat(fileName, date);
        strcat(fileName, "] - ");
        strcat(fileName, folderName);
        strcat(fileName, ".tree");

        fichier = fopen(fileName, "w");
        if(fichier == NULL){
            printf_RGB(255,0,0,"[!] Erreur : impossible de créer %s\n", fileName);
            return 1;
        }
        free(fileName);
    }

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

    pthread_t thread;
    g_scanning = 1;
    pthread_create(&thread, NULL, animLoader, NULL);

    /*
    * On fais appelle a la fonction scanDirecory qui me permet de remplire
    * le typdef pour l'exploiter juste apres
    */
    Directory Dir = scanDirectory(parent,p);

    g_scanning = 0;
    pthread_join(thread, NULL);

    printf_RGB(0,255,0,"%s\n",parent.name); // On affiche le path parent
    echoDirectory(Dir,"",p,0);

    printf("\n\n");


    if(p.statsParam == 1){
        triABulleExt();
        printf_Stat();
    }
    if(p.searchParam == 1){
        printf_searched(p);
    }
    clock_t end = clock();
    if(p.debug == 1){
        printf_debug(p,start,end);
    }

    freeDirectory(&Dir);
    free(PATH);
    return 0;
}

////////////////////////////////////////////////////////////////
/* SCANDIRECTORY
* Fonction qui prend en paramétre un élément (rappelle : ligne 62), avec les data de ce dernier il vas l'analysée en le prenant comme un
* répertoire et vas définire ces caractéristique (path, liste d'élement ...).
* Permettre de mettre en place la liste arborifique des fichier avec une liste d'élement et un appelle récursive de cette fonction en cas
* de rencontre d'un autre répertoire
*/
Directory scanDirectory(Element element,Param p){
    DIR *dir; // On définit un pointeur de type DIR
    Directory NewDirectory; // On instanci une struct de type directory nommée NewDirecory car c'est le directory qu'on vas analysée

    // Allocation d'un espace de stockage pour le path de ce dernier qui est égale au path de l'élement mis en paramétre 
    NewDirectory.path = calloc(4096,sizeof(char));
    strcpy(NewDirectory.path,element.path);
    
    // Depuis le path de l'élement parent on détermine sa profondeur (Le nombre de /)
    NewDirectory.depth = depthCount(NewDirectory.path);

    // Set up des valeur de base 
    NewDirectory.nbDir = 0;
    NewDirectory.nbFile = 0;

    // Création de la struct data qui est le cusrseur dans le fichier 
    struct dirent *data;

    // On ouvre le fichier qui se trouve dans le path de l'élément passée en param
    dir = opendir(NewDirectory.path);

    // On crée donc la liste d'élements qu'il y'a dans le répertoire
    // Allocation large pour éviter les problèmes de double passage readdir
    NewDirectory.elements = calloc(4096,sizeof(Element));
    // Pareil avec la liste de répertoire 
    NewDirectory.Directorys = calloc(4096,sizeof(Directory));

    // Index pour se repérée
    int i = 0;
    // Index des répertoire 
    int IndexDire = 0;
    // Tant qu'on peut lire dans le répertoire
    while((data = readdir(dir)) != NULL){
        // On saute "." (Répertoire courant) et ".." (Répertoire parent)
        if(strcmp(data->d_name, ".") == 0 || strcmp(data->d_name, "..") == 0 || data->d_type == DT_UNKNOWN ){
            continue;
        }
        // On renconte un nouvelle élément !
        // On crée un espace de stockage pour sont nom et on affecte le nom effectife de ce fichier avec d_name
        NewDirectory.elements[i].name = calloc(4096,sizeof(char));
        strcpy(NewDirectory.elements[i].name, data->d_name);
    
        // On crée un espace pour sont extention (aussi)
        char* _ext = getExt(NewDirectory.elements[i].name);
        NewDirectory.elements[i].ext = _ext;  // on garde directement le pointeur alloué par getExt
        
        // On détermine sont path grace au path de l'élément mis en paramétre + son nom effectife
        NewDirectory.elements[i].path = calloc(4096,sizeof(char));
        strcpy(NewDirectory.elements[i].path,NewDirectory.path);
        // Assurer qu'il n'y a pas de double slash
        if (NewDirectory.path[strlen(NewDirectory.path)-1] != '/'){
            strcat(NewDirectory.elements[i].path, "/");
        }
        strcat(NewDirectory.elements[i].path,NewDirectory.elements[i].name);
        // Si l'élement rencontrée est un répertoire alors ...
        if(data->d_type == DT_DIR){
            // On définis sont type dans la strcuture en définisant cette élement comme une répertoire
            NewDirectory.elements[i].type = T_DIR;

            // Scanner le sous-répertoire
            Directory subDir = scanDirectory(NewDirectory.elements[i],p);
            // On affecte se sous répertoire a la liste des répertoire du répertoire parent
            NewDirectory.Directorys[IndexDire] = subDir;
            IndexDire++;

            // Calculer la taille réelle du sous-répertoire
            double dirSize = getDirectorySize(subDir);
            // On définit sa taille et sa représentation en string
            NewDirectory.elements[i].size = (long long)dirSize;
            NewDirectory.elements[i].sizeStr = getSizeStr((long long)dirSize);
        }else if(data->d_type == DT_REG){
            // On définis sont type dans la strcuture en définisant cette élement comme un fichier
            NewDirectory.elements[i].type = T_FILE;

            addExt(NewDirectory.elements[i].ext);

            // On récupère la taille du fichier
            struct stat st;
            stat(NewDirectory.elements[i].path, &st);
            long long buffer = st.st_size;

            // On définit sa taille et sa représentation en string
            NewDirectory.elements[i].size = buffer;
            NewDirectory.elements[i].sizeStr = getSizeStr(buffer);

            // On ajoute la taille au compteur global
            g_globalSize += buffer;

            // Si le mode stats est actif on compare et stock le fichier
            if(p.statsParam == 1){
                addToTop10(NewDirectory.elements[i]);
            }
        }else{
            NewDirectory.elements[i].type = T_UNKN;
            NewDirectory.elements[i].sizeStr = NULL;
            NewDirectory.elements[i].size = 0;
        }

        // On récupére les Element recherchée pour l'afficher dans les info 
        if(p.searchParam == 1){
            for(int y = 0; y < p.nbSearchParam; y++){
                if(strcmp(NewDirectory.elements[i].name, p.searchNameParam[y]) == 0){
                    g_searchedElements = realloc(g_searchedElements, (g_searchedElementsIndex + 1) * sizeof(Element));
                    g_searchedElements[g_searchedElementsIndex] = NewDirectory.elements[i];
                    g_searchedElementsIndex++;
                }
            }
        }
        i++;
    }

    // On met à jour les compteurs avec les valeurs réelles du passage unique
    NewDirectory.nbDir  = IndexDire;
    NewDirectory.nbFile = i - IndexDire;
    g_nbDIR  += IndexDire;
    g_nbFILE += i - IndexDire;

    // On réduit les allocations à la taille réelle
    if(i > 0){
        NewDirectory.elements = realloc(NewDirectory.elements, i * sizeof(Element));
    }else{
        NewDirectory.elements = realloc(NewDirectory.elements, 1 * sizeof(Element));
    }

    if(IndexDire > 0){
        NewDirectory.Directorys = realloc(NewDirectory.Directorys, IndexDire * sizeof(Directory));
    }else{
        NewDirectory.Directorys = realloc(NewDirectory.Directorys, 1 * sizeof(Directory));
    }

    closedir(dir);
    return NewDirectory;
}

/* echoDirectory
* Fonction qui prend en paramétre un directory a traitée ainsi que le "prefix" (L'arborésence actuelle du directory)
* ainsi que la profondeur du directory.
* Permet d'afficher l'arborésence aisni que traiter les paramétre entrée par l'utilisateur.
*/
void echoDirectory(Directory directory, String prefix, Param p, int depth){
    int nbElement = directory.nbDir + directory.nbFile;

    int indexLastVisible = nbElement;
    if(p.banParam == 1){
        for(int i=nbElement-1;i>=0;i--){
            int banned = 0;
            for(int y=0;y<p.nbBanParam;y++){
                if(strcmp(directory.elements[i].name,p.banNameParam[y]) == 0){
                    banned = 1;
                    break;
                }
            }
            if(!banned){
                indexLastVisible = i;
                break;
            }
        }
    }

    int indexLenght = 0;
    int IndexDir = 0;

    for(int i=0;i<nbElement;i++){
        if(directory.elements[i].type == T_FILE){
            if(p.cutParam == 1 && indexLenght >= p.cutLenght){
                if(i == nbElement - 1){
                    printf("%s", prefix);
                    printf("└── ");
                    printf_RGB(0,0,255," And %d other file ... \n", directory.nbFile-p.cutLenght);
                    if(fichier != NULL){
                        fprintf(fichier, "%s", prefix);
                        fprintf(fichier, "└──  And %d other file ... \n", directory.nbFile-p.cutLenght);
                    }
                }
                continue;
            }

            int found = 0;
            if(p.banParam == 1){
                for(int y=0;y<p.nbBanParam;y++){
                    if(strcmp(directory.elements[i].name,p.banNameParam[y]) == 0){
                        found = 1;
                        break;
                    }
                }
            }
            if(found == 1){
                continue;
            }

            printf("%s", prefix);
            if(fichier != NULL){
                fprintf(fichier, "%s", prefix);
            }

            if(i == nbElement - 1 || i == indexLastVisible){
                printf("└── ");
                if(fichier != NULL){
                    fprintf(fichier, "└── ");
                }
            }else{
                printf("├── ");
                if(fichier != NULL){
                    fprintf(fichier, "├── ");
                }
            }

            int isSearched = 0;
            if(p.searchParam == 1){
                for(int y=0; y<p.nbSearchParam; y++){
                    if(strcmp(p.searchNameParam[y], directory.elements[i].name) == 0){
                        isSearched = 1;
                        printf_RGB_BG(0,0,0,255,255,255," %s ",directory.elements[i].name);
                    }
                }
                if(!isSearched){
                    printf("%s ", directory.elements[i].name);
                }
            }else{
                printf("%s ", directory.elements[i].name);
            }
            if(fichier != NULL){
                fprintf(fichier, "%s ", directory.elements[i].name);
            }

            if(p.pathParam == 1){
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,0,128,255," %s ",directory.elements[i].path);
                }else{
                    printf_RGB(0,128,255,"%s ",directory.elements[i].path);
                }
                if(fichier != NULL){
                    fprintf(fichier, "%s ", directory.elements[i].path);
                }
            }

            if(p.sizeParam == 1){
                int r,g,b;
                sizeToGradientColor(directory.elements[i].size, &r, &g, &b);
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,r,g,b," %s ",directory.elements[i].sizeStr);
                }else{
                    printf_RGB(r,g,b,"%s ",directory.elements[i].sizeStr);
                }
                if(fichier != NULL){
                    fprintf(fichier, "%s ", directory.elements[i].sizeStr);
                }
            }

            printf("\n");
            if(fichier != NULL){
                fprintf(fichier, "\n");
            }
            indexLenght++;

        }else if(directory.elements[i].type == T_DIR){
            int found = 0;
            if(p.banParam == 1){
                for(int y=0;y<p.nbBanParam;y++){
                    if(strcmp(directory.elements[i].name,p.banNameParam[y]) == 0){
                        found = 1;
                        break;
                    }
                }
            }
            if(found == 1){
                IndexDir++;
                continue;
            }

            printf("%s", prefix);
            if(fichier != NULL){
                fprintf(fichier, "%s", prefix);
            }

            if(i == nbElement - 1 || i == indexLastVisible){
                printf("└── ");
                if(fichier != NULL){
                    fprintf(fichier, "└── ");
                }
            }else{
                printf("├── ");
                if(fichier != NULL){
                    fprintf(fichier, "├── ");
                }
            }

            int isSearched = 0;
            if(p.searchParam == 1){
                for(int y=0;y<p.nbSearchParam;y++){
                    if(strcmp(p.searchNameParam[y],directory.elements[i].name) == 0){
                        isSearched = 1;
                        printf_RGB_BG(0,0,0,0,255,0," %s ",directory.elements[i].name);
                    }
                }
                if(!isSearched){
                    printf_RGB(0,255,0,"%s  ",directory.elements[i].name);
                }
            }else{
                printf_RGB(0,255,0,"%s  ",directory.elements[i].name);
            }
            if(fichier != NULL){
                fprintf(fichier, "%s  ", directory.elements[i].name);
            }

            if(p.cutParam && depth >= p.cutDepth){
                printf_RGB(0,0,255,"[Cuted]  ");
                if(fichier != NULL){
                    fprintf(fichier, "[Cuted]  ");
                }
            }

            if(p.pathParam == 1){
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,255,128,0," %s ",directory.elements[i].path);
                }else{
                    printf_RGB(255,128,0,"%s ",directory.elements[i].path);
                }
                if(fichier != NULL){
                    fprintf(fichier, "%s ", directory.elements[i].path);
                }
            }

            if(p.sizeParam == 1){
                int r,g,b;
                sizeToGradientColor(directory.elements[i].size, &r, &g, &b);
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,r,g,b," %s ",directory.elements[i].sizeStr);
                }else{
                    printf_RGB(r,g,b,"%s ",directory.elements[i].sizeStr);
                }
                if(fichier != NULL){
                    fprintf(fichier, "%s ", directory.elements[i].sizeStr);
                }
            }

            printf("\n");
            if(fichier != NULL){
                fprintf(fichier, "\n");
            }

            String NewPrefix = (String)calloc(4096,sizeof(char));
            strcpy(NewPrefix,prefix);
            if(i == nbElement - 1 || i == indexLastVisible){
                strcat(NewPrefix,"   ");
            }else{
                strcat(NewPrefix,"│  ");
            }

            if(!(p.cutParam && depth >= p.cutDepth)){
                echoDirectory(directory.Directorys[IndexDir],NewPrefix,p,depth + 1);
            }
            free(NewPrefix);
            IndexDir++;

        }else if(directory.elements[i].type == T_UNKN){
            printf_RGB(255,0,0,"%s\n",directory.elements[i].name);
            if(fichier != NULL){
                fprintf(fichier, "%s\n", directory.elements[i].name);
            }
        }
    }
}

/* freeDirectory
* Fonction qui libére tout les allocation d'un répertoire passée en paramétre, a utlisée a la fin
*/
void freeDirectory(Directory* dir) {
    for(int i=0;i<dir->nbDir;i++)
        freeDirectory(&dir->Directorys[i]);
    for(int i=0;i<dir->nbDir + dir->nbFile;i++) {
        free(dir->elements[i].name);
        free(dir->elements[i].path);
        free(dir->elements[i].ext);
        if(dir->elements[i].sizeStr != NULL)
            free(dir->elements[i].sizeStr);
    }
    free(dir->elements);
    free(dir->Directorys);
    free(dir->path);
}

void* animLoader(void* arg){
    (void)arg;
    char* frames[] = {"⠋","⠙","⠹","⠸","⠼","⠴","⠦","⠧","⠇","⠏"};
    int i = 0;
    while(g_scanning){
        printf("\r ");
        printf_RGB(255,200,0,"%s", frames[i % 10]);
        printf(" Scanning... %d FILE | %d DIRS", g_nbFILE, g_nbDIR);
        fflush(stdout);
        usleep(80000);
        i++;
    }
    printf("\r                                        \r");
    fflush(stdout);
    return NULL;
}
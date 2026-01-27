#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>

typedef char* String;

/* UPDATE 
 * - Ajout du paramétre search
*/

/* TYPE DE DONNEE
 * Pour savoir qu'elle type de fichier on vas avoir en face je préfère faire mon propre type pour
 * prévoire les cas envisagable
*/
typedef enum {
    T_FILE, // Fichier normal (txt,mp4 ...)
    T_DIR,  // Fichier repertoire 
    T_UNKN  // Fichier non standare ou non reconnue (cas d'echec principalement)
} ElementType;

/* TAILLE DES FICHIER 
 * Enum résument les différente taille disponibre 
 * Octet ; KiloOctet ; MegaOctet ; GigaOctet ; TeraOctete
*/
typedef enum {
    O,
    KO,
    MO,
    GO,
    TO
} ElementSizeCategory;

/* STRUCTURE DES PARAMETRE
 * Résumé des différent paramétre d'entrée pour une utilisation booleain dans les fonction utile 
 * Presque tout les valeur son des int "booleain" (0 ou 1) sauf pour deph et lenght qui sont de
 * entier standare
*/
typedef struct{
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

    int errorDouble;    // Erreur si l'utilisateur entre 2x le même param
    int errorNotValide; // Erreur si l'utilisateur entre un param invalide
    int errorCutEntry;  // Erreur si l'utilisateur fais un choix non valide du param Cut (deph = -1000 ou lenght 0) 
    int errorBanEntry;  // Erreur si la liste de param ne prend pas en compte des nom de fichier (-ban puis plus rien comme param)
    int errorSearchEntry;

    int debug;
}Param;

int g_depthIndex = 0;

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


// Fonction primaire 
Directory scanDirectory(Element element);
void echoDirectory(Directory directory,String prefix,Param p,int depth);
void freeDirectory(Directory* dir);

// Fonction secondaire
Param getParameter(int argc, char *argv[]);
ElementSizeCategory getSizeCategory(long long size);
double getDirectorySize(Directory dir);
int depthCount(String path);
int _atoi(char *input);

// Affichage
void printf_RGB(int r, int g, int b, const char* format, ...);
void printf_wave_utf8(int r1, int g1, int b1,int r2, int g2, int b2,const char* text,int step);
void sizeToGradientColor(long long size, int* r, int* g, int* b);
void printf_RGB_BG(int fr, int fg, int fb, int br, int bg, int bb, const char* format, ...);

////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

    if(argc == 1 || strcmp(argv[1],"-info") == 0){
        printf_wave_utf8(178,0,255,0,38,255,"┌─ CTree project v10 ─────────────────────────────────────────┐\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ Coded with <3 by BetaTetras (https://github.com/BetaTetras) │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ Tree folder visualizer made in C with specific parameters.  │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│                                                             │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ Usage: CTree <path> [options]                               │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ Ex   : CTree ./myFolder -deep -cut 3 10                     │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│                                                             │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ -path .....: Show path of each file                         │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ -size .....: Show size of directories and files             │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ -deep .....: Show both size and path                        │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ -cut X Y ..: Limit depth and/or length of the tree          │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│       => X -> Max depth  (0 = unlimited)                    │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│       => Y -> Max length (0 = unlimited)                    │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ -ban [...] .: Ban files from the tree                       │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"│ -debug ....: Debug mode (in case of error)                  │\n",1);
        printf_wave_utf8(178,0,255,0,38,255,"└─────────────────────────────────────────────────────────────┘\n",1);

        return 0;
    }

    // Si le nombre de param est < 2 pas valable
    if(argc < 2){
        printf_RGB(255,0,0,"[!] Error : Mauvaise utilisation de la commande\n");
        return 1;
    }

    // Booleain de vérification des erreur paramétrique 
    int _break = 0;
    // Struc des paramétre nommée "p"
    Param p = {0};    

    // Si l'utilisateur a entrée des param alors on lance la fonction getParameter qui vas analisée les entrée des param et determinée les param
    if(argc > 2){
        p = getParameter(argc,argv);
    }

    if(p.debug){
        printf_RGB(0,255,0,"-path = %d -size = %d -cut = %d Depth = %d Lenght = %d -ban = %d\n",p.pathParam,p.sizeParam,p.cutParam,p.cutDepth,p.cutLenght,p.banParam);
        printf_RGB(0,0,255,"Ban list : [");
        for(int i = 0; i < p.nbBanParam; i++){
            if(i == p.nbBanParam - 1){
                printf_RGB(0,0,255, "%s", p.banNameParam[i]);   // dernier : sans virgule
            } else {
                printf_RGB(0,0,255, "%s, ", p.banNameParam[i]); // autres : avec virgule
            }
        }
        printf_RGB(0,0,255,"]\n");
        printf_RGB(255,0,255,"Search : [");
        for(int i = 0; i < p.nbSearchParam; i++){
            if(i == p.nbSearchParam - 1){
                printf_RGB(0,0,255, "%s", p.searchNameParam[i]);   // dernier : sans virgule
            } else {
                printf_RGB(0,0,255, "%s, ", p.searchNameParam[i]); // autres : avec virgule
            }
        }
        printf_RGB(255,0,255,"]\n");
        printf_RGB(255,0,0,"errorDouble = %d errorNotValide = %d errorCutEntry = %d\n\n",p.errorDouble,p.errorNotValide,p.errorCutEntry);
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
        printf_RGB(255,0,0,"            -ban => Ban name from the tree \n");
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
    struct dirent *data; // Création 
    struct stat st;

    // Définire le chemin racine passer en paramétre
    String PATH = (String)calloc(4096,sizeof(char));
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
/* SCANDIRECTORY
 * Fonction qui prend en paramétre un élément (rappelle : ligne 62), avec les data de ce dernier il vas l'analysée en le prenant comme un
 * répertoire et vas définire ces caractéristique (path, liste d'élement ...).
 * Permettre de mettre en place la liste arborifique des fichier avec une liste d'élement et un appelle récursive de cette fonction en cas
 * de rencontre d'un autre répertoire
*/
Directory scanDirectory(Element element){
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

    // Tant qu'on peut lire dans le répertoire 
    while((data = readdir(dir)) != NULL){
        // On saute "." (Répertoire courant) et ".." (Répertoire parent)
        if(strcmp(data->d_name, ".") == 0 || strcmp(data->d_name, "..") == 0){
            continue;
        }else{
            
            if(data->d_type == DT_DIR){             // Si le type rencontrée est un répertoire on augmenre le compteur associer
                NewDirectory.nbDir ++;
            }else if(data->d_type == DT_REG){       // Si le type rencontrée est un fichier on augmenre le compteur associer
                NewDirectory.nbFile ++;
            }else if(data->d_type == DT_UNKNOWN){   // Si le type rencontrée n'est pas reconnue alors on le saute (Eviter les erreur)
                continue;
            }
        }
    }
    // On réinitilalise le curseur
    rewinddir(dir);
    // On crée donc la liste d'élements qu'il y'a dans le répertoire
    NewDirectory.elements = calloc(NewDirectory.nbDir + NewDirectory.nbFile,sizeof(Element));
    // Pareil avec la liste de répertoire 
    NewDirectory.Directorys = calloc(NewDirectory.nbDir,sizeof(Directory));

    // Index pour se repérée
    int i = 0;
    // Index des répertoire 
    int IndexDire = 0;
    // Tant qu'on peut lire dans le répertoire
    while((data = readdir(dir)) != NULL){
        // On saute "." (Répertoire courant) et ".." (Répertoire parent)
        if(strcmp(data->d_name, ".") == 0 || strcmp(data->d_name, "..") == 0){
            continue;
        }
        // On renconte un nouvelle élément !
        // On crée un espace de stockage pour sont nom et on affecte le nom effectife de ce fichier avec d_name
        NewDirectory.elements[i].name = calloc(4096,sizeof(char));
        strcpy(NewDirectory.elements[i].name, data->d_name);
        
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
            Directory subDir = scanDirectory(NewDirectory.elements[i]);
            // On affecte se sous répertoire a la liste des répertoire du répertoire parent
            NewDirectory.Directorys[IndexDire] = subDir;
            IndexDire++;

            // Calculer la taille réelle du sous-répertoire
            double dirSize = getDirectorySize(subDir);
            // On définit sa taille 
            NewDirectory.elements[i].size = dirSize;
            // On définit sa taille adaptée (O,KO,GO,TB...)
            NewDirectory.elements[i].sizeCategory = getSizeCategory((long long)dirSize);

            // Selon le resultat de la taille choisi on converti les octet brute en valeur correspondant
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

/* echoDirectory
 * Fonction qui prend en paramétre un directory a traitée ainsi que le "prefix" (L'arborésence actuelle du directory)
 * ainsi que la profondeur du directory.
 * Permet d'afficher l'arborésence aisni que traiter les paramétre entrée par l'utilisateur.
*/
void echoDirectory(Directory directory,String prefix,Param p,int depth){
    int nbElement = directory.nbDir + directory.nbFile;
    int nbDir = directory.nbDir;

    // Gestion du dernier élément ban
    int indexLastVisible = nbElement;
    int _break = 0;
    if(p.banParam == 1){
        for(int i=nbElement-1;i>=0;i--){
            int banned = 0;
            for(int y=0;y<p.nbBanParam;y++){
                if(strcmp(directory.elements[i].name,p.banNameParam[y]) == 0){
                    banned = 1;
                    break;
                }
            }
            if(!banned){ // non-banni = c'est le dernier visible
                indexLastVisible = i;
                break;
            }
        }
    }

    int indexLenght = 0;
    int IndexDir = 0;

    for(int i=0;i<nbElement;i++){
        if(directory.elements[i].type == T_FILE){
            // Gestion de lenght
            if(p.cutParam == 1 && indexLenght >= p.cutLenght){
                if( i == nbElement - 1){
                    printf("%s",prefix);
                    printf("└── ");
                    printf_RGB(0,0,255," And %d other file ... \n",directory.nbFile-p.cutLenght);
                }
                continue;
            }

            // Gestion des bans
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

            // Gestion des prefixe passée
            printf("%s",prefix);
            // Gestion de la logique des prefixe actuelle
            if(i == nbElement - 1){
                printf("└── ");
            }else if(i == indexLastVisible ){
                printf("└── ");
            }else{
                printf("├── ");
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
                    printf("%s ",directory.elements[i].name);
                }
            }else{
                printf("%s ",directory.elements[i].name);
            }
            if(p.pathParam == 1){
                printf(" ");
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,0,128,255," %s ",directory.elements[i].path);
                }else{
                    printf_RGB(0,128,255,"%s ",directory.elements[i].path);
                }
            }if(p.sizeParam == 1){
                printf(" ");
                int r,g,b;
                sizeToGradientColor(directory.elements[i].size, &r, &g, &b);
                switch(directory.elements[i].sizeCategory){
                    case O:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"O ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"O   ");
                    break;
                    case KO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"KO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"KO   ");
                    break;
                    case MO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"MO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"MO   ");
                    break;
                    case GO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"GO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"GO   ");
                    break;
                    case TO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"TO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"TO   ");
                    break;
                    default:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"Error   ");
                }
            }
            printf("\n");
            indexLenght++;
        }else if(directory.elements[i].type == T_DIR){
            // Gestion des bans
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
                IndexDir ++;
                continue;
            }

            printf("%s",prefix);
            if(i == nbElement - 1){
                printf("└── ");
            }else if(i == indexLastVisible ){
                printf("└── ");
            }else{
                printf("├── ");
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

            if(p.cutParam && depth >= p.cutDepth){
                printf_RGB(0,0,255,"[Cuted]  ");
            }
            if(p.pathParam == 1){
                printf(" ");
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,255,128,0," %s ",directory.elements[i].path);
                }else{
                    printf_RGB(255,128,0,"%s ",directory.elements[i].path);
                }
            }if(p.sizeParam == 1){
                printf(" ");
                int r,g,b;
                sizeToGradientColor(directory.elements[i].size, &r, &g, &b);
                switch(directory.elements[i].sizeCategory){
                    case O:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"O ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"O   ");
                    break;
                    case KO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b, "%.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"KO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"KO   ");
                    break;
                    case MO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"MO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b," %.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"MO ");
                    break;
                    case GO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"GO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"GO   ");
                    break;
                    case TO:
                        if(isSearched == 1){
                            printf_RGB_BG(0,0,0,r,g,b," %.2f ",directory.elements[i].sizeConverted);
                            printf_RGB_BG(0,0,0,r,g,b,"TO ");
                            printf(" ");
                            break;
                        }
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"TO   ");
                    break;
                    default:
                        printf_RGB(r,g,b,"%.2f ",directory.elements[i].sizeConverted);
                        printf_RGB(r,g,b,"Error   ");
                }
            }
            printf("\n");
            String NewPrefix = (String)calloc(4096,sizeof(char));
            strcpy(NewPrefix,prefix);
            if(i == nbElement - 1 || i == indexLastVisible){
                strcat(NewPrefix,"   ");
            }else{
                strcat(NewPrefix,"│  ");
            }

            if(p.cutParam && depth >= p.cutDepth){
                continue;
            }
            echoDirectory(directory.Directorys[IndexDir],NewPrefix,p,depth + 1);
            free(NewPrefix);
            IndexDir++;
        }else if(directory.elements[i].type == DT_UNKNOWN){
            printf_RGB(255,0,0,"%s\n",directory.elements[i].name);
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
    }
    free(dir->elements);
    free(dir->Directorys);
    free(dir->path);
}

/* depthCount
 * Fonction qui donne la profondeur d'un fichier depuis son path
*/
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
ElementSizeCategory getSizeCategory(long long size){
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
                i = indexParamSearch - 1;  // ← MANQUANT, sauter les args consommés
            }
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


///////////////////////////////////////////////////////////////


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

void printf_RGB_BG(int fr, int fg, int fb, int br, int bg, int bb, const char* format, ...) {
    va_list args;
    va_start(args, format);
    printf("\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm", fr, fg, fb, br, bg, bb);
    vprintf(format, args);
    printf("\033[0m");
    va_end(args);
}
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <dirent.h>
    #include <stdarg.h>
    #include <sys/stat.h>

    typedef char* String;

    /* TYPE DE DONNEE
    * Pour savoir qu'elle type de fichier on vas avoir en face je préfère faire mon propre type pour
    * prévoire les cas envisagable
    */
    typedef enum {
        T_FILE, // Fichier normal (txt,mp4 ...)
        T_DIR,  // Fichier repertoire 
        T_UNKN  // Fichier non standare ou non reconnue (cas d'echec principalement)
    } ElementType;

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

        int statsParam;

        int errorDouble;    // Erreur si l'utilisateur entre 2x le même param
        int errorNotValide; // Erreur si l'utilisateur entre un param invalide
        int errorCutEntry;  // Erreur si l'utilisateur fais un choix non valide du param Cut (deph = -1000 ou lenght 0) 
        int errorBanEntry;  // Erreur si la liste de param ne prend pas en compte des nom de fichier (-ban puis plus rien comme param)
        int errorSearchEntry;

        int debug;
    }Param;

    // Index le profondeur pour repérage
    int g_depthIndex = 0;

    /* TYPE DE FICHIER
    * chaque elemnt (fichier ou repetoire)) on leur propre data comme leur nom , le path ainsi que leur
    * propre type. Utile pour la récursivitée 
    */
    typedef struct{
        String name;
        ElementType type;
        String path;
        String ext;
        long long size;
        double sizeConverted;
        String sizeStr;
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

    /* NOMBRE DE FOIS PAR EXTENTION
     * Permet de faire les stat sur les extention ...
    */
    typedef struct {
        char* ext;
        int count;
    } ExtStat;

    // Valeur statistique
    int g_nbDIR = 0;
    int g_nbFILE = 0;
    long long int g_globalSize = 0;

    // Tableau des info des extention
    ExtStat* g_extStats = NULL;
    int g_nbExt = 0;
    
    // Tableau des éléments les plus lourd
    Element g_topFiles[10];
    int g_topFilesInit = 0;

    // Tableau des élement recherche non-afficher 
    Element* g_searchedElements = NULL;
    int g_searchedElementsIndex = 0;

    // Fonction primaire 
    Directory scanDirectory(Element element,Param p);
    void echoDirectory(Directory directory,String prefix,Param p,int depth);
    void freeDirectory(Directory* dir);

    // Fonction secondaire
    Param getParameter(int argc, char *argv[]);
    String getSizeStr(long long size);
    double getDirectorySize(Directory dir);
    int depthCount(String path);
    int _atoi(char *input);
    String getExt(char* str);
    void addExt(char* ext);
    void triABulleExt();
    void addToTop10(Element element);
    int strlenVis(const char* s);
    int strpartcmp(char* src, char* str);

    // Affichage
    void printf_RGB(int r, int g, int b, const char* format, ...);
    void printf_wave_utf8(int r1, int g1, int b1,int r2, int g2, int b2,const char* text,int step,int* out_r, int* out_g, int* out_b);
    void sizeToGradientColor(long long size, int* r, int* g, int* b);
    void printf_RGB_BG(int fr, int fg, int fb, int br, int bg, int bb, const char* format, ...);
    void printf_Stat();
    void printf_debug(Param p);
    void printf_searched(Param p);
    int* print_rainbow(int* rgb, float brightness, const char* string, int pas, int sens, int max);
    void printBarre(int _Size, int value);

    ////////////////////////////////////////////////////////////////

    int main(int argc, char *argv[]) {

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

        // Booleain de vérification des erreur paramétrique 
        int _break = 0;
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

        Directory Dir = scanDirectory(parent,p);
        if(p.debug == 1){
            printf_debug(p);
        }

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
    void echoDirectory(Directory directory,String prefix,Param p,int depth){
    int nbElement = directory.nbDir + directory.nbFile;
    int nbDir = directory.nbDir;

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
                    printf("%s",prefix);
                    printf("└── ");
                    printf_RGB(0,0,255," And %d other file ... \n",directory.nbFile-p.cutLenght);
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
            if(found == 1) continue;

            printf("%s",prefix);
            if(i == nbElement - 1 || i == indexLastVisible){
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
                if(!isSearched) printf("%s ",directory.elements[i].name);
            }else{
                printf("%s ",directory.elements[i].name);
            }

            if(p.pathParam == 1){
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,0,128,255," %s ",directory.elements[i].path);
                }else{
                    printf_RGB(0,128,255,"%s ",directory.elements[i].path);
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
            }

            printf("\n");
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

            printf("%s",prefix);
            if(i == nbElement - 1 || i == indexLastVisible){
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
                if(!isSearched) printf_RGB(0,255,0,"%s  ",directory.elements[i].name);
            }else{
                printf_RGB(0,255,0,"%s  ",directory.elements[i].name);
            }

            if(p.cutParam && depth >= p.cutDepth){
                printf_RGB(0,0,255,"[Cuted]  ");
            }

            if(p.pathParam == 1){
                if(isSearched == 1){
                    printf_RGB_BG(0,0,0,255,128,0," %s ",directory.elements[i].path);
                }else{
                    printf_RGB(255,128,0,"%s ",directory.elements[i].path);
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
            }

            printf("\n");
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
                    i = indexParamSearch - 1;  // ← MANQUANT, sauter les args consommés
                }
            }else if(strcmp(argv[i], "-stats") == 0){
                if(param.statsParam == 1){
                    param.errorDouble = 1;
                    break;
                }else{
                    param.statsParam = 1;
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
                        int step,
                        int* out_r, int* out_g, int* out_b) {
        const char *p = text;
        
        int char_count = 0;
        while (*p) {
            unsigned char c = (unsigned char)*p;
            if ((c & 0xC0) != 0x80) char_count++;
            p++;
        }

        p = text;
        int index = 0;
        int r = r1, g = g1, b = b1; // valeurs initialisées au cas où text est vide
        while (*p) {
            unsigned char c = (unsigned char)*p;
            int bytes = 1;
            if (c >= 0xF0) bytes = 4;
            else if (c >= 0xE0) bytes = 3;
            else if (c >= 0xC0) bytes = 2;

            // gradient
            r = (char_count > 1) ? r1 + (r2 - r1) * index / (char_count - 1) : r1;
            g = (char_count > 1) ? g1 + (g2 - g1) * index / (char_count - 1) : g1;
            b = (char_count > 1) ? b1 + (b2 - b1) * index / (char_count - 1) : b1;

            // step
            if (r1 < r2) r += step * index; else r -= step * index;
            if (g1 < g2) g += step * index; else g -= step * index;
            if (b1 < b2) b += step * index; else b -= step * index;

            // clamp
            r = (r>255)?255:(r<0?0:r);
            g = (g>255)?255:(g<0?0:g);
            b = (b>255)?255:(b<0?0:b);

            printf("\033[38;2;%d;%d;%dm", r, g, b);
            printf("%.*s", bytes, p);
            p += bytes;
            index++;
        }

        // Retourner les dernières valeurs RGB si les pointeurs sont fournis
        if(out_r) *out_r = r;
        if(out_g) *out_g = g;
        if(out_b) *out_b = b;
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

    void printBarre(int _Size, int value) {
        char* on  = (char*)calloc(value * 3 + 1, 1);
        char* off = (char*)calloc((_Size - value) * 3 + 1, 1);

        for (int i = 0; i < value; i++)
            memcpy(on + i * 3, "█", 3);
        on[value * 3] = '\0';

        for (int i = 0; i < _Size - value; i++)
            memcpy(off + i * 3, "█", 3);
        off[(_Size - value) * 3] = '\0';

        int* start = (int*)calloc(3, sizeof(int));
        start[0] = 255; start[1] = 255; start[2] = 0;

        int* end = print_rainbow(start, 1.0f, on, 12, 0, 252);

        int* end_dark = (int*)calloc(3, sizeof(int));
        end_dark[0] = end[0] / 2;
        end_dark[1] = end[1] / 2;
        end_dark[2] = end[2] / 2;

        int* res = print_rainbow(end_dark, 0.5f, off, 6, 0, 126);

        free(on);
        free(off);
        free(start);
        free(end);
        free(end_dark);
        free(res);
    }

    int* print_rainbow(int* rgb, float brightness, const char* string, int pas, int sens, int max) {
        if (rgb[0] > max) rgb[0] = max;
        if (rgb[1] > max) rgb[1] = max;
        if (rgb[2] > max) rgb[2] = max;

        int _r = rgb[0];
        int _g = rgb[1];
        int _b = rgb[2];

        int i = 0;
        while (string[i] != '\0') {
            /* Détecter la taille du caractère UTF-8
            * 0xxxxxxx  → 1 octet  (ASCII classique, valeur < 0x80)
            * 110xxxxx  → 2 octets (valeur >= 0xC0)
            * 1110xxxx  → 3 octets (valeur >= 0xE0)
            * 11110xxx  → 4 octets (valeur >= 0xF0)
            */
            int char_len = 1;
            unsigned char c = (unsigned char)string[i];
            if      (c >= 0xF0) char_len = 4;
            else if (c >= 0xE0) char_len = 3;
            else if (c >= 0xC0) char_len = 2;

            printf("\033[38;2;%d;%d;%dm", (int)(_r * brightness),(int)(_g * brightness),(int)(_b * brightness));
            for (int j = 0; j < char_len; j++)
                putchar(string[i + j]);
            printf("\033[0m");

            i += char_len;

            if (sens == 0) {
                if (_r >= max && _g < max && _b == 0) {
                    _g += pas;
                    if (_g > max) {
                        _g = max;
                    }
                } else if (_r > 0 && _g >= max && _b == 0) {
                    _r -= pas;
                    if (_r < 0) {
                        _r = 0;
                    }
                } else if (_r == 0 && _g >= max && _b < max) {
                    _b += pas;
                    if (_b > max) {
                        _b = max;
                    }
                } else if (_r == 0 && _g > 0 && _b >= max) {
                    _g -= pas;
                    if (_g < 0) {
                        _g = 0;
                    }
                } else if (_r < max && _g == 0 && _b >= max) {
                    _r += pas;
                    if (_r > max) {
                        _r = max;
                    }
                } else if (_r >= max && _g == 0 && _b > 0) {
                    _b -= pas;
                    if (_b < 0) {
                        _b = 0;
                    }
                }
            } else {
                if (_r >= max && _g == 0 && _b < max) {
                    _b += pas;
                    if (_b > max) {
                        _b = max;
                    }
                } else if (_r > 0 && _g == 0 && _b >= max) {
                    _r -= pas;
                    if (_r < 0) {
                        _r = 0;
                    }
                } else if (_r == 0 && _g < max && _b >= max) {
                    _g += pas;
                    if (_g > max) {
                        _g = max;
                    }
                } else if (_r == 0 && _g >= max && _b > 0) {
                    _b -= pas;
                    if (_b < 0) {
                        _b = 0;
                    }
                } else if (_r < max && _g >= max && _b == 0) {
                    _r += pas;
                    if (_r > max) {
                        _r = max;
                    }
                } else if (_r >= max && _g > 0 && _b == 0) {
                    _g -= pas;
                    if (_g < 0) {
                        _g = 0;
                    }
                }
            }
        }

        int* res = (int*)calloc(3,sizeof(int));
        res[0] = _r;
        res[1] = _g;
        res[2] = _b;
        return res;
    }

    void printf_Stat(){
        int r,g,b;
        int _r,_g,_b;
        int r2,g2,b2;
        int buffeurChar = 0;
        int nbrMAX = 0;
        int extMAX = 0;
        int fileMAX = 0;
        int sizeMAX = 0;
        int pathMAX = 0;

        int topExt = g_nbExt;
        if(g_nbExt > 5) topExt = 5;

        char* globalSize = getSizeStr(g_globalSize);
        buffeurChar = (int)strlen(globalSize);
        if(nbDigit(g_nbDIR)  > buffeurChar) buffeurChar = nbDigit(g_nbDIR);
        if(nbDigit(g_nbFILE) > buffeurChar) buffeurChar = nbDigit(g_nbFILE);

        char* ligne1 = (char*)calloc((buffeurChar+1) * 3 + 4, sizeof(char));
        for(int i=0;i<buffeurChar+1;i++) strcat(ligne1, "─");

        for(int i=0;i<topExt;i++){
            if(strlenVis(g_extStats[i].ext) > extMAX) extMAX = strlenVis(g_extStats[i].ext);
            if(nbDigit(g_extStats[i].count) > nbrMAX) nbrMAX = nbDigit(g_extStats[i].count);
        }

        for(int i=0;i<10;i++){
            if(g_topFiles[i].size == 0) break;
            if(strlenVis(g_topFiles[i].name)    > fileMAX) fileMAX = strlenVis(g_topFiles[i].name);
            if(strlenVis(g_topFiles[i].sizeStr) > sizeMAX) sizeMAX = strlenVis(g_topFiles[i].sizeStr);
            if(strlenVis(g_topFiles[i].path)    > pathMAX) pathMAX = strlenVis(g_topFiles[i].path);
        }

        // ┌─ Information numérique ──────┐
        printf_wave_utf8(178,0,255,0,38,255,"┌─ Information numérique ──",1,&r,&g,&b);
        printf_wave_utf8(r,g,b,0,38,255,ligne1,1,&_r,&_g,&_b);
        printf_RGB(_r,_g,_b,"┐\n");

        printf_wave_utf8(178,0,255,0,38,255,"│ Nombre de repertoire ..: ",1,NULL,NULL,NULL);
        printf_RGB(255,255,255,"%d ",g_nbDIR);
        for(int i=0;i<buffeurChar-nbDigit(g_nbDIR);i++) printf(" ");
        printf_RGB(_r,_g,_b,"│\n");

        printf_wave_utf8(178,0,255,0,38,255,"│ Nombre de fichier .....: ",1,NULL,NULL,NULL);
        printf_RGB(255,255,255,"%d ",g_nbFILE);
        for(int i=0;i<buffeurChar-nbDigit(g_nbFILE);i++) printf(" ");
        printf_RGB(_r,_g,_b,"│\n");

        printf_wave_utf8(178,0,255,0,38,255,"│ Poids totale scanée ...: ",1,NULL,NULL,NULL);
        printf_RGB(255,255,255,"%s ",globalSize);
        for(int i=0;i<buffeurChar-strlenVis(globalSize);i++) printf(" ");
        printf_RGB(_r,_g,_b,"│\n");

        printf_wave_utf8(178,0,255,0,38,255,"└──────────────────────────",1,NULL,NULL,NULL);
        printf_wave_utf8(r,g,b,0,38,255,ligne1,1,&_r,&_g,&_b);
        printf_RGB(_r,_g,_b,"┘\n");

        // ┌─ Top 5 des extentions ───────┐
        if(g_nbExt == 0){
            printf_wave_utf8(178,0,255,0,38,255,"┌─ Top 5 des extention ────┐\n",1,&r,&g,&b);
            printf_wave_utf8(178,0,255,0,38,255,"│ Aucune extention trouvé  │\n",1,NULL,NULL,NULL);
            printf_wave_utf8(178,0,255,0,38,255,"└──────────────────────────┘\n",1,NULL,NULL,NULL);
        }else{
            printf_wave_utf8(255,255,0,0,255,128,"┌─ Top 5 des extention ───",1,&r,&g,&b);
            for(int i=0;i<(extMAX+nbrMAX+4);i++) printf_wave_utf8(r,g,b,0,255,128,"─",1,&r,&g,&b);
            printf_RGB(r,g,b,"┐\n");

            for(int i=0;i<topExt;i++){
                char* buffeur = (char*)calloc(50,sizeof(char));
                char* num = intToString(i+1);
                if(i == 0){ strcpy(buffeur,"│ "); strcat(buffeur,num); strcat(buffeur,"er   : "); }
                else       { strcpy(buffeur,"│ "); strcat(buffeur,num); strcat(buffeur,"eme  : "); }
                free(num);
                printf_wave_utf8(255,255,0,0,255,128,buffeur,1,&r,&g,&b);
                free(buffeur);
                printf_RGB(255,255,255,"%s",g_extStats[i].ext);
                for(int y=0;y<extMAX-strlenVis(g_extStats[i].ext);y++) printf(" ");
                printf("  ");
                printf_RGB(255,255,255,"%d",g_extStats[i].count);
                for(int y=0;y<nbrMAX-nbDigit(g_extStats[i].count);y++) printf(" ");
                printf("  ");
                printBarre(15,(int)((float)g_extStats[i].count * 15.0f / (float)g_nbFILE));
                printf_RGB(r,g,b," │\n");
            }

            printf_wave_utf8(255,255,0,0,255,128,"└─────────────────────────",1,&r,&g,&b);
            for(int i=0;i<(extMAX+nbrMAX+4);i++) printf_wave_utf8(r,g,b,0,255,128,"─",1,&r,&g,&b);
            printf_RGB(r,g,b,"┘\n");
        }

        // ┌─ Top 10 des fichiers ────────┐
        // prefix max = "│ 10eme : " = 10 chars visuels
        int innerWidth = 10 + fileMAX + 2 + pathMAX + 2 + sizeMAX + 1;
        int dashCount  = innerWidth - 22;
        if(dashCount < 0) dashCount = 0;

        printf_wave_utf8(255,0,128,255,128,0,"┌─ Top 10 des fichiers ──",1,&r2,&g2,&b2);
        for(int i=0;i<dashCount-3;i++) printf_wave_utf8(r2,g2,b2,255,128,0,"─",1,&r2,&g2,&b2);
        printf_wave_utf8(r2,g2,b2,255,128,0,"┐\n",1,&r2,&g2,&b2);

        for(int i=0;i<10;i++){
            if(g_topFiles[i].size == 0) break;
            char* num = intToString(i+1);
            char* buffeur = (char*)calloc(50,sizeof(char));
            strcpy(buffeur,"│ ");
            strcat(buffeur,num);
            if(i == 0)      strcat(buffeur,"er   : "); // "1er   : " → total prefix = 10
            else if(i == 9) strcat(buffeur,"eme : ");  // "10eme : " → total prefix = 10
            else            strcat(buffeur,"eme  : "); // "Xeme  : " → total prefix = 10
            free(num);
            printf_wave_utf8(255,0,128,255,128,0,buffeur,1,&r2,&g2,&b2);
            free(buffeur);
            printf_RGB(255,255,255,"%s",g_topFiles[i].name);
            for(int y=0;y<fileMAX-strlenVis(g_topFiles[i].name);y++) printf(" ");
            printf("  ");
            printf_RGB(255,255,255,"%s",g_topFiles[i].path);
            for(int y=0;y<pathMAX-strlenVis(g_topFiles[i].path);y++) printf(" ");
            printf("  ");
            printf_RGB(255,255,255,"%s",g_topFiles[i].sizeStr);
            for(int y=0;y<sizeMAX-strlenVis(g_topFiles[i].sizeStr);y++) printf(" ");
            printf_RGB(r2,g2,b2," │\n");
        }

        printf_wave_utf8(255,0,128,255,128,0,"└────────────────────────",1,&r2,&g2,&b2);
        for(int i=0;i<dashCount-3;i++) printf_wave_utf8(r2,g2,b2,255,128,0,"─",1,&r2,&g2,&b2);
        printf_wave_utf8(r2,g2,b2,255,128,0,"┘\n",1,&r2,&g2,&b2);

        free(ligne1);
        free(globalSize);
    }

    
    void printf_debug(Param p){
        if(p.debug){
            printf_RGB(0,255,0,"-path = %d -size = %d -cut = %d Depth = %d Lenght = %d -ban = %d\n",p.pathParam,p.sizeParam,p.cutParam,p.cutDepth,p.cutLenght,p.banParam);
            if(p.banParam == 1){
                printf_RGB(0,0,255,"Ban list : [");
                for(int i = 0; i < p.nbBanParam; i++){
                    if(i == p.nbBanParam - 1){
                        printf_RGB(0,0,255, "%s", p.banNameParam[i]);   // dernier : sans virgule
                    } else {
                        printf_RGB(0,0,255, "%s, ", p.banNameParam[i]); // autres : avec virgule
                    }
                }
                printf_RGB(0,0,255,"]\n");
            }
            if(p.searchParam == 1){
                printf_RGB(255,0,255,"Search : [");
                for(int i = 0; i < p.nbSearchParam; i++){
                    if(i == p.nbSearchParam - 1){
                        printf_RGB(255,0,255, "%s", p.searchNameParam[i]);   // dernier : sans virgule
                    } else {
                        printf_RGB(255,0,255, "%s, ", p.searchNameParam[i]); // autres : avec virgule
                    }
                }
                printf_RGB(255,0,255,"]\n");
            }
            printf_RGB(255,0,0,"errorDouble = %d errorNotValide = %d errorCutEntry = %d errorBanEntry = %d errorSearchEntry = %d\n\n",p.errorDouble,p.errorNotValide,p.errorCutEntry,p.errorBanEntry,p.errorSearchEntry);
        }
    }

    void printf_searched(Param p){
        int r,g,b;

        int nameMAX = 0;
        int pathMAX = 0;

        for(int i=0;i<g_searchedElementsIndex;i++){
            if(nameMAX < (int)strlen(g_searchedElements[i].name)){
                nameMAX = (int)strlen(g_searchedElements[i].name);
            }
            if(pathMAX < (int)strlen(g_searchedElements[i].path)){
                pathMAX = (int)strlen(g_searchedElements[i].path);
            }
        }

        int maxLigne = 2 + nameMAX + 2 + 4 + 2 + pathMAX + 2;

        // ┌─ Éléments recherchés ───────┐
        // │ CTree  FILE  ../CTree       │
        // │ CTree  FILE  ../CTree/CTree │
        // └─────────────────────────────┘
        // 2 + nameMAX + 2 + pathMAX + 2

        // ┌─ Information numérique ──────┐
        printf_wave_utf8(0,255,0,0,255,128,"┌─ Éléments recherchés ─",2,&r,&g,&b);
        for(int i=0;i<maxLigne - 25;i++){
            printf_wave_utf8(r,g,b,0,255,128,"─",2,&r,&g,&b);
        }
        printf_RGB(r,g,b,"┐\n");

        //│ CTree  FILE  ../CTree       │
        for(int i=0;i<g_searchedElementsIndex;i++){
            printf_RGB(0,255,0,"│ ");
            printf("%s",g_searchedElements[i].name);
            for(int y=0;y<nameMAX - (int)strlen(g_searchedElements[i].name);y++){
                printf(" ");
            }
            printf("  ");
            if(g_searchedElements[i].type == T_FILE){
                printf("FILE  ");
            }else{
                printf_RGB(0,255,0,"DIR   ");
            }

            if(g_searchedElements[i].type == T_DIR){
                printf("%s",g_searchedElements[i].path);
            }else{
                printf("%s",g_searchedElements[i].path);
            }
            for(int y=0;y<pathMAX - (int)strlen(g_searchedElements[i].path);y++){
                printf(" ");
            }
            printf_RGB(r,g,b," │\n");
        }

        // └─────────────────────────────┘
        r,g,b = 0;
        printf_wave_utf8(0,255,0,0,255,128,"└───────────────────────",2,&r,&g,&b);
        for(int i=0;i<maxLigne-25;i++){
            printf_wave_utf8(r,g,b,0,255,128,"─",2,&r,&g,&b);
        }
        printf_RGB(r,g,b,"┘\n");
    }
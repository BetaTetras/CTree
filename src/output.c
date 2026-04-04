#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "output.h"
#include "tools.h"

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


void printf_debug(Param p,clock_t start,clock_t end){
    if(p.debug){
        printf_RGB(0,255,0,"-path = %d -size = %d [-cut = %d {Depth = %d Lenght = %d}] -ban = %d -search = %d -stats = %d -out = %d\n",p.pathParam,p.sizeParam,p.cutParam,p.cutDepth,p.cutLenght,p.banParam,p.searchParam,p.statsParam,p.outParam);
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
        printf_RGB(255,0,0,"errorDouble = %d errorNotValide = %d errorCutEntry = %d errorBanEntry = %d errorSearchEntry = %d\n",p.errorDouble,p.errorNotValide,p.errorCutEntry,p.errorBanEntry,p.errorSearchEntry);
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
        printf_RGB(39,221,245,"Temps d'execution : ");
        printf("%.3f ms => %.2f s\n",elapsed,elapsed/1000);
    }
}

void printf_searched(){
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
    r = 0; g = 0; b = 0;
    printf_wave_utf8(0,255,0,0,255,128,"└───────────────────────",2,&r,&g,&b);
    for(int i=0;i<maxLigne-25;i++){
        printf_wave_utf8(r,g,b,0,255,128,"─",2,&r,&g,&b);
    }
    printf_RGB(r,g,b,"┘\n");
}
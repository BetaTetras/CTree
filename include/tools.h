#ifndef TOOLS_H
#define TOOLS_H

#include "types.h"

Param  getParameter(int argc, char *argv[]);
String getSizeStr(long long size);
double getDirectorySize(Directory dir);
int    depthCount(String path);
int    _atoi(char *input);
String getExt(char* str);
void   addExt(char* ext);
void   triABulleExt();
void   addToTop10(Element element);
int    strlenVis(const char* s);
int    strpartcmp(char* src, char* str);
int    nbDigit(int nb);
String intToString(int nb);

#endif
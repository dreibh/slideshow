#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <iostream>
#include <fstream>


using namespace std;


#define CHECK(cond) if(!(cond)) { std::cerr << "ERROR in " << __FILE__ << ", line " << __LINE__ << ": condition " << #cond << " is not satisfied!" << std::endl; exit(1); }


void webify(char* name);
const char* extractFileName(const char* name);
const char* getFileNameOneDirDown(const char* name);
unsigned long long getMicroTime();
string getTime(const unsigned long long microTime);
string getDate(const unsigned long long microTime);
void printTimeStamp(FILE* fd);
bool safestrcpy(char* dest, const char* src, const size_t size);
bool safestrcat(char* dest, const char* src, const size_t size);
char* strindex(char* string, const char character);
char* strrindex(char* string, const char character);
void makeDir(const char* a, const char* b = NULL, const char* c = NULL);
void copy(const char* input, const char* output);


#endif
/* --------------------------------------------------------------------------
 * - XHTML 1.1 image presentation and JavaScript-based slideshow generator  -
 * --------------------------------------------------------------------------
 *
 * Copyright (C) 2003-2022 by Thomas Dreibholz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: thomas.dreibholz@gmail.com
 */

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

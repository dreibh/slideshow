/* --------------------------------------------------------------------------
 * - XHTML 1.1 image presentation and JavaScript-based slideshow generator  -
 * --------------------------------------------------------------------------
 *
 * Copyright (C) 2003-2018 by Thomas Dreibholz
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
 * Contact: dreibh@iem.uni-due.de
 */

#include "tools.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>


// ###### Convert filename to web-usable one #################################
void webify(char* name)
{
   char* s1 = rindex(name, '/');
   char* s2 = rindex(name, '.');
   if(s2 > s1) {
      *s2 = 0x00;
   }

   const ssize_t l = strlen(name);
   for(ssize_t i = l - 1;i > 0;i--) {
      if(name[i] == '/') {
         return;
      }
      name[i] = tolower(name[i]);
      if( (!isalnum(name[i])) &&
          (name[i] != '.') &&
          (name[i] != '+') &&
          (name[i] != '-') ) {
         name[i] = '_';
      }
   }
}


const char* extractFileName(const char* name)
{
   const char* str = rindex(name, '/');
   if(str) {
      return((const char*)&str[1]);
   }
   return(name);
}


const char* getFileNameOneDirDown(const char* name)
{
   const char* str = index(name, '/');
   if(str) {
      return((const char*)&str[1]);
   }
   return(name);
}


/* ###### Get current timer ############################################## */
unsigned long long getMicroTime()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return(((unsigned long long)tv.tv_sec * (unsigned long long)1000000) + (unsigned long long)tv.tv_usec);
}


// ###### Convert timestamp to string ########################################
string getTime(const unsigned long long microTime)
{
   char         str[128];
   const time_t timeStamp = microTime / 1000000;
   struct tm    timestruct;
   localtime_r(&timeStamp, &timestruct);
   strftime((char*)&str,sizeof(str), "%H:%M:%S %Z", &timestruct);
   return(string(str));
}


// ###### Convert timestamp to string ########################################
string getDate(const unsigned long long microTime)
{
   char         str[128];
   const time_t timeStamp = microTime / 1000000;
   struct tm    timestruct;
   localtime_r(&timeStamp, &timestruct);
   strftime((char*)&str,sizeof(str), "%d-%b-%Y", &timestruct);
   return(string(str));
}


/* ###### Print time stamp ############################################### */
void printTimeStamp(FILE* fd)
{
   char str[64];
   const unsigned long long microTime = getMicroTime();
   const time_t timeStamp = microTime / 1000000;
   const struct tm *timeptr = localtime(&timeStamp);

   strftime((char*)&str,sizeof(str),"%d-%b-%Y %H:%M:%S",timeptr);
   fputs(str, fd);
   fprintf(fd,".%04d: ",(unsigned int)(microTime % 1000000) / 100);
}


/* ###### Length-checking strcpy() ###################################### */
bool safestrcpy(char* dest, const char* src, const size_t size)
{
   if(size > 0) {
      strncpy(dest,src,size);
      dest[size - 1] = 0x00;
      return(strlen(dest) < size);
   }
   return(false);
}


/* ###### Length-checking strcat() ###################################### */
bool safestrcat(char* dest, const char* src, const size_t size)
{
   const int l1  = strlen(dest);
   const int l2  = strlen(src);

   if(l1 + l2 < (int)size) {
      strcat(dest,src);
      return(true);
   }
   else if((int)size > l2) {
      strcat((char*)&dest[size - l2],src);
   }
   else {
      safestrcpy(dest,src,size);
   }
   return(false);
}


/* ###### Find first occurrence of character in string ################### */
char* strindex(char* string, const char character)
{
   if(string != NULL) {
      while(*string != character) {
         if(*string == 0x00) {
            return(NULL);
         }
         string++;
      }
      return(string);
   }
   return(NULL);
}


/* ###### Find last occurrence of character in string #################### */
char* strrindex(char* string, const char character)
{
   const char* original = string;

   if(original != NULL) {
      string = (char*)&string[strlen(string)];
      while(*string != character) {
         if(string == original) {
            return(NULL);
         }
         string--;
      }
      return(string);
   }
   return(NULL);
}


void makeDir(const char* a, const char* b, const char* c)
{
   char str[1024];
   safestrcpy((char*)&str, a, sizeof(str));
   if(b) {
      safestrcat((char*)&str, "/", sizeof(str));
      safestrcat((char*)&str, b, sizeof(str));
   }
   if(c) {
      safestrcat((char*)&str, "/", sizeof(str));
      safestrcat((char*)&str, c, sizeof(str));
   }
   const int result = mkdir(str, S_IRWXU|S_IXGRP|S_IRGRP|S_IXOTH|S_IROTH);
   if((result != 0) && (errno != EEXIST)) {
      cerr << "ERROR: Unable to create directory \"" << str << "\"!" << endl;
      exit(1);
   }
}


// ###### Copy a file #######################################################
void copy(const char* input, const char* output)
{
   char  buffer[8192];
   FILE* in  = fopen(input, "r");
   if(in != NULL) {
      FILE* out = fopen(output, "w");
      if(out != NULL) {
         size_t length = fread((char*)&buffer, 1, sizeof(buffer),  in);
         while(length > 0) {
            if(fwrite((char*)&buffer, 1, length, out) != length) {
               break;
            }
            length = fread((char*)&buffer, 1, sizeof(buffer),  in);
         }
         fclose(out);
      }
      else {
         cerr << "ERROR: Unable to create \"" << output << "\"" << endl;
         exit(1);
      }
      fclose(in);
   }
   else {
      cerr << "ERROR: Unable to read \"" << input << "\"" << endl;
      exit(1);
   }
}

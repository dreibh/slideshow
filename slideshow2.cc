/*
 * XHTML 1.1 image presentation and JavaScript-based slideshow generator
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>
#include <iostream>
#include <fstream>


using namespace std;


#define CHECK(cond) if(!(cond)) { std::cerr << "ERROR in " << __FILE__ << ", line " << __LINE__ << ": condition " << #cond << " is not satisfied!" << std::endl; exit(1); }

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



class Image;
class Block;



class Presentation
{
   public:
   Presentation(const char* presentationName, const char* title);

   public:
   char         Title[1024];
   char         PresentationName[1024];

   set<Block*>  BlockSet;
   unsigned int LastBlockID;
};

Presentation::Presentation(const char* presentationName, const char* title)
{
   snprintf((char*)&Title, sizeof(Title), "%s", title);
   snprintf((char*)&PresentationName, sizeof(PresentationName), "%s", presentationName);
   webify((char*)&PresentationName);
   LastBlockID = 0;
}



class Block
{
   public:
   Block(Presentation* presentation, const char* blockTitle);

   char         Title[1024];
   unsigned int ID;
   char         DirectoryName[1024];

   set<Image*>  ImageSet;
   unsigned int LastImageID;
};

Block::Block(Presentation* presentation, const char* blockTitle)
{
   ID = ++presentation->LastBlockID;
   snprintf((char*)&Title, sizeof(Title), "%s", title);
   snprintf((char*)&DirectoryName, sizeof(DirectoryName), "%s", name);
   LastImageID = 0;
}




class Image
{
   public:
   Image(Block* block, const char* imageTitle, const char* sourceName);

   public:
   Block*       OwnerBlock;
   char         Title[1024];
   unsigned int ID;
   char         SourceName[1024];
   char         OriginalName[1024];
   char         FullsizeName[1024];
   char         PreviewName[1024];
   unsigned int OriginalWidth;
   unsigned int OriginalHeight;
   unsigned int FullsizeWidth;
   unsigned int FullsizeHeight;
   unsigned int PreviewWidth;
   unsigned int PreviewHeight;
};


Image::Image(Block* block, const char* imageTitle, const char* sourceName)
{
   OwnerBlock = block;
   ID         = ++block->LastImageID;
   snprintf((char*)&Title, sizeof(Title), "%s", title);

   snprintf((char*)&SourceName,   sizeof(SourceName),   "%s", name);
   snprintf((char*)&OriginalName, sizeof(OriginalName), "%s", name);
   webify((char*)&OriginalName);
   snprintf((char*)&FullsizeName, sizeof(FullsizeName), "%s", name);
   webify((char*)&FullsizeName);
   snprintf((char*)&PreviewName,  sizeof(PreviewName),  "%s", name);
   webify((char*)&PreviewName);
   OriginalWidth  = 0;
   OriginalHeight = 0;
   FullsizeWidth  = 0;
   FullsizeHeight = 0;
   PreviewWidth   = 0;
   PreviewHeight  = 0;
   block->ImageSet.insert(image);
}



void getDefaults(int argc, char** argv)
{
   for(int i = 1;i < argc;i++) {
   }
}

void createImageTable(int argc, char** argv)
{
   Block* currentBlock = NULL;
   char*  blockTitle   = "";

   for(int i = 1;i < argc;i++) {
      if(!(strncmp(argv[i], "--", 2))) {

         Image* image = createImage(argv[i]);
      }
      else if(!(strncmp(argv[i], "--block=", 8))) {
         blockTitle = (char*)&argv[i][8];
      }
   }
}


int main(int argc, char** argv)
{
   getDefaults(argc, argv);
   createImageTable(argc, argv);
}

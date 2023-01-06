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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pwd.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <set>

#include "tools.h"
#include "converter.h"


using namespace std;


#define DEFAULT_SLIDESHOW_INFRASTRUCTURE1 "/usr/local/share/slideshow/infrastructure"
#define DEFAULT_SLIDESHOW_INFRASTRUCTURE2 "/usr/share/slideshow/infrastructure"


// ###### Replace special patterns in string ################################
string modifyLine(const char* line)
{
   bool control        = false;
   string result       = "";
   const size_t length = strlen(line);

   static const unsigned long long now = getMicroTime();
   for(size_t i = 0;i < length;i++) {
      if(line[i] == '$') {
         control = true;
      }
      else {
         if(control == false) {
            result += line[i];
         }
         else {
            control = false;
            switch(line[i]) {
               case '$':
                  result += string("$");
                break;
               case 'T':
                  result += getTime(now);
                break;
               case 'D':
                  result += getDate(now);
                break;
               default:
                  cerr << "ERROR: Bad control sequence $+" << line[i] << "!" << endl;
                  exit(1);
                break;
            }
         }
      }
   }
   return(result);
}


// ###### Dump a file to an output stream ####################################
bool cat(ostream& os, const char* input)
{
   char str[8192];

   ifstream in(input);
   if(in.good()) {
      while(!in.eof()) {
         in.getline(str, sizeof(str), '\n');
         os << modifyLine(str) << endl;
      }
      return(true);
   }
   return(false);
}


class Image;
class Block;


class Presentation
{
   public:
   Presentation();
   void dump();
   void createDirectories();
   void createInfrastructureFiles();
   void createRedirectPage();
   void createMainPage();
   void createViewPages(const bool showOriginal,
                        const bool forSlideshow);
   void createImages();
   void createSlideshow();

   public:
   bool         Enumerate;
   bool         Index;
   bool         Slideshow;
   bool         IncludeOriginal;
   bool         SkipImageConversion;

   size_t       Columns;
   size_t       PreviewWidth;
   size_t       PreviewHeight;
   size_t       PreviewQuality;
   size_t       FullsizeWidth;
   size_t       FullsizeHeight;
   size_t       FullsizeQuality;

   char         InfrastructureDirectory[1024];
   char         MainTitle[1024];
   char         MainDescription[1024];
   char         DirectoryName[1024];
   char         PresentationName[1024];
   char         Stylesheet[1024];
   char         Background[1024];
   char         ShortcutIcon[1024];
   char         Author[1024];
   char         Head[1024];
   char         Tail[1024];
   char         OneLeftArrowImage[1024];
   char         TwoLeftArrowImage[1024];
   char         OneRightArrowImage[1024];
   char         TwoRightArrowImage[1024];
   char         UpArrowImage[1024];
   char         PauseImage[1024];
   char         PlayImage[1024];
   char         ShuffleImage[1024];
   char         SlideshowFilelist[1024];
   char         SlideshowFrameset[1024];
   char         SlideshowControl[1024];
   char         SlideshowControlBody[1024];
   char         ImageViewerScript[1024];
   char         SlideshowScript[1024];

   set<Block*>  BlockSet;
   size_t       LastBlockID;

   private:
   void createSlideshowFrameset(const char* filelistName,
                                const char* framesetName,
                                const char* controlName);
};

class Block
{
   public:
   Block(Presentation*      presentation,
         const char*        blockTitle,
         const char*        blockDescription,
         const size_t           columns);
   void dump();
   void createImages();
   void createDirectories();
   void createViewPages(const Block* prevBlock,
                        const Block* nextBlock,
                        const bool   showOriginal,
                        const bool   forSlideshow);

   public:
   Presentation* OwnerPresentation;
   char          Title[1024];
   size_t        ID;
   size_t        Columns;
   char          Description[1024];
   char          DirectoryName[1024];
   char          OriginalDirectory[1024];
   char          FullsizeDirectory[1024];
   char          PreviewDirectory[1024];
   char          SlideshowFilelist[1024];
   char          SlideshowFrameset[1024];
   char          SlideshowControl[1024];

   set<Image*>   ImageSet;
   size_t        LastImageID;
};

class Image
{
   public:
   Image(Block* block, const char* imageTitle, const char* sourceName);
   void dump();
   void createImage();
   void createViewPage(const Block* prevBlock, const Block* nextBlock,
                       const Image* prevImage, const Image* nextImage,
                       const bool   showOriginal,
                       const bool   forSlideshow);

   public:
   Block*       OwnerBlock;
   char         Title[1024];
   size_t       ID;
   char         SourceName[1024];
   char         OriginalName[1024];
   char         FullsizeName[1024];
   char         PreviewName[1024];
   char         ViewName[1024];
   char         SlideshowName[1024];
   size_t       OriginalWidth;
   size_t       OriginalHeight;
   size_t       FullsizeWidth;
   size_t       FullsizeHeight;
   size_t       PreviewWidth;
   size_t       PreviewHeight;
};



// ###### Constructor #######################################################
Presentation::Presentation()
{
   Enumerate           = true;
   Index               = true;
   Columns             = 5;
   LastBlockID         = 0;
   PreviewWidth        = 128;
   PreviewHeight       = 96;
   PreviewQuality      = 50;
   FullsizeWidth       = 640;
   FullsizeHeight      = 480;
   FullsizeQuality     = 75;
   SkipImageConversion = false;

   struct stat iStatus;
   if( (stat("infrastructure", &iStatus) == 0) && (S_ISDIR(iStatus.st_mode)) ) {
      safestrcpy((char*)&InfrastructureDirectory, "infrastructure/", sizeof(InfrastructureDirectory));
   }
   else if( (stat(DEFAULT_SLIDESHOW_INFRASTRUCTURE1, &iStatus) == 0) && (S_ISDIR(iStatus.st_mode)) ) {
      safestrcpy((char*)&InfrastructureDirectory, DEFAULT_SLIDESHOW_INFRASTRUCTURE1, sizeof(InfrastructureDirectory));
   }
   else if( (stat(DEFAULT_SLIDESHOW_INFRASTRUCTURE2, &iStatus) == 0) && (S_ISDIR(iStatus.st_mode)) ) {
      safestrcpy((char*)&InfrastructureDirectory, DEFAULT_SLIDESHOW_INFRASTRUCTURE2, sizeof(InfrastructureDirectory));
   }
   else {
      safestrcpy((char*)&InfrastructureDirectory, "./", sizeof(InfrastructureDirectory));
   }
   safestrcpy((char*)&MainTitle, "Slideshow Image Archive", sizeof(MainTitle));
   safestrcpy((char*)&MainDescription, "", sizeof(MainDescription));
   safestrcpy((char*)&DirectoryName, "test-dir", sizeof(DirectoryName));
   safestrcpy((char*)&PresentationName, "index.html", sizeof(PresentationName));
   safestrcpy((char*)&Stylesheet, "", sizeof(Stylesheet));
   safestrcpy((char*)&Background, "", sizeof(Background));
   safestrcpy((char*)&ShortcutIcon, "", sizeof(ShortcutIcon));
   safestrcpy((char*)&Head, "", sizeof(Head));
   safestrcpy((char*)&Tail, "", sizeof(Tail));

   passwd* pw = getpwuid(getuid());
   if(pw) {
      safestrcpy((char*)&Author, pw->pw_gecos, sizeof(Author));
   }
   else {
      safestrcpy((char*)&Author, "", sizeof(Author));
   }
}


// ###### Dump information ##################################################
void Presentation::dump()
{
   cout << "   - Presentation \"" << MainTitle << "\"" << endl
        << "      + InfrastructureDirectory=" << InfrastructureDirectory << endl
        << "      + SkipImageConversion=" << ((SkipImageConversion == true) ? "yes" : "no") << endl
        << "      + MainTitle=" << MainTitle << endl
        << "      + MainDescription=" << MainDescription << endl
        << "      + DirectoryName=" << DirectoryName << endl
        << "      + PresentationName=" << PresentationName << endl
        << "      + Stylesheet=" << Stylesheet << endl
        << "      + ShortcutIcon=" << ShortcutIcon << endl
        << "      + SlideshowFilelist=" << SlideshowFilelist << endl
        << "      + SlideshowFrameset=" << SlideshowFrameset << endl
        << "      + SlideshowControl=" << SlideshowControl << endl
        << "      + PreviewWidth=" << PreviewWidth << endl
        << "      + PreviewHeight=" << PreviewHeight << endl
        << "      + PreviewQuality=" << PreviewQuality << " [%]" << endl
        << "      + FullsizeWidth=" << FullsizeWidth << endl
        << "      + FullsizeHeight=" << FullsizeHeight << endl
        << "      + FullsizeQuality=" << FullsizeQuality << " [%]" << endl
        << "      + ImageViewerScript=" << ImageViewerScript << endl
        << "      + SlideshowScript=" << SlideshowScript << endl
        << "      + SlideshowControlBody=" << SlideshowControlBody << endl;
   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      (*blockIterator)->dump();
      blockIterator++;
   }
}


// ###### Create directories ################################################
void Presentation::createDirectories()
{
   makeDir(DirectoryName);
   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      (*blockIterator)->createDirectories();
      blockIterator++;
   }
}


// ###### Create images #####################################################
void Presentation::createImages()
{
   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      (*blockIterator)->createImages();
      blockIterator++;
   }
}


// ###### Create frameset ###################################################
void Presentation::createSlideshowFrameset(const char* filelistName,
                                           const char* framesetName,
                                           const char* controlName)
{
   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s/%s", DirectoryName, framesetName);
   ofstream ssframeset(str);
   if(!ssframeset.good()) {
      cerr << "ERROR: Unable to create output file \"" << str << "\"!" << endl;
      exit(1);
   }

   ssframeset << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
              << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">" << endl
              << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl
              << "<head>" << endl;
   ssframeset << "<title>" << MainTitle << "</title>" << endl;
   ssframeset << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />" << endl;
   if(Author[0] != 0x00) {
      ssframeset  << "<meta name=\"author\" content=\"" << Author << "\" />" << endl;
   }
   ssframeset << "<meta name=\"description\" content=\"" << MainTitle << "\" />" << endl;
   ssframeset << "<meta name=\"keywords\" content=\"Slideshow, " << MainTitle;
   if(Author[0] != 0x00) {
      ssframeset << ", " << Author;
   }
   ssframeset << "\" />" << endl;
   ssframeset << "<meta name=\"classification\" content=\"Slideshow\" />" << endl;
   ssframeset << "</head>" << endl;
   ssframeset << "<frameset rows=\"50px, 80%\">" << endl
          << "   <frame src=\"" << controlName << "\" frameborder=\"1\" noresize=\"noresize\" scrolling=\"no\" />" << endl
          << "   <frame frameborder=\"1\" />" << endl
          << "</frameset>" << endl;
   ssframeset << "</html>" << endl;


   snprintf((char*)&str, sizeof(str), "%s/%s", DirectoryName, controlName);
   ofstream sscontrol(str);
   if(!sscontrol.good()) {
      cerr << "ERROR: Unable to create output file \"" << str << "\"!" << endl;
      exit(1);
   }
   sscontrol << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
             << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl
             << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl
             << "<head>" << endl;
   if(Stylesheet[0] != 0x00) {
      sscontrol << "<link rel=\"stylesheet\" href=\"" << Stylesheet << "\" type=\"text/css\" />" << endl;
   }
   if(ShortcutIcon[0] != 0x00) {
      sscontrol << "<link rel=\"shortcut icon\" href=\"" << ShortcutIcon << "\" type=\"image/png\" />" << endl;
   }
   sscontrol << "<title>" << MainTitle << "</title>" << endl;
   sscontrol << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />" << endl;
   if(Author[0] != 0x00) {
      sscontrol  << "<meta name=\"author\" content=\"" << Author << "\" />" << endl;
   }
   sscontrol << "<meta name=\"description\" content=\"" << MainTitle << "\" />" << endl;
   sscontrol << "<meta name=\"keywords\" content=\"Slideshow, " << MainTitle;
   if(Author[0] != 0x00) {
      sscontrol << ", " << Author;
   }
   sscontrol << "\" />" << endl;
   sscontrol << "<meta name=\"classification\" content=\"Slideshow\" />" << endl;
   sscontrol << "<script type=\"text/javascript\" src=\"" << SlideshowScript << "\"></script>" << endl;
   sscontrol << "<script type=\"text/javascript\" src=\"" << filelistName << "\"></script>" << endl;
   sscontrol << "</head>" << endl << endl;
   if(!cat(sscontrol, SlideshowControlBody)) {
      cerr << "ERROR: Unable to copy slideshow control body from \"" << SlideshowControlBody << "\"!" << endl;
      exit(1);
   }
   sscontrol << "</html>" << endl;
}


// ###### Create slideshow ##################################################
void Presentation::createSlideshow()
{
   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s/slideshow-filelist-allblocks.js", DirectoryName);
   ofstream ssallblocks(str);
   if(!ssallblocks.good()) {
      cerr << "ERROR: Unable to create file \"" << str << "\"!" << endl;
      exit(1);
   }
   ssallblocks << "mainPage = \"" << PresentationName << "\";" << endl;
   ssallblocks << "presentationName = \"" << PresentationName << "\";" << endl;
   createSlideshowFrameset(SlideshowFilelist, SlideshowFrameset, SlideshowControl);

   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      const Block* block = *blockIterator;
      snprintf((char*)&str, sizeof(str), "%s/slideshow-filelist-%s.js", DirectoryName, block->DirectoryName);
      ofstream ssblock(str);
      if(!ssblock.good()) {
         cerr << "ERROR: Unable to create file \"" << str << "\"!" << endl;
         exit(1);
      }
      ssblock << "mainPage = \"" << PresentationName << "\";" << endl;
      ssblock << "presentationName = \"" << PresentationName << "\";" << endl;
      createSlideshowFrameset(block->SlideshowFilelist, block->SlideshowFrameset, block->SlideshowControl);

      set<Image*>::iterator imageIterator = block->ImageSet.begin();
      while(imageIterator != block->ImageSet.end()) {
         const Image* image = *imageIterator;
         ssallblocks << "imageArray[images++]=\"" << image->SlideshowName << "\";" << endl;
         ssblock << "imageArray[images++]=\"" << image->SlideshowName << "\";" << endl;
         imageIterator++;
      }

      blockIterator++;
   }
}


// ###### Create infrastructure files #######################################
void Presentation::createInfrastructureFiles()
{
   int i = strlen(InfrastructureDirectory);
   while(--i >= 0) {
      if(InfrastructureDirectory[i] == '/') {
         InfrastructureDirectory[i] = 0x00;
      }
      else {
         break;
      }
   }
   snprintf((char*)&OneLeftArrowImage, sizeof(OneLeftArrowImage), "%s/%s", InfrastructureDirectory, "1leftarrow.png");
   snprintf((char*)&TwoLeftArrowImage, sizeof(TwoLeftArrowImage), "%s/%s", InfrastructureDirectory, "2leftarrow.png");
   snprintf((char*)&OneRightArrowImage, sizeof(OneRightArrowImage), "%s/%s", InfrastructureDirectory, "1rightarrow.png");
   snprintf((char*)&TwoRightArrowImage, sizeof(TwoRightArrowImage), "%s/%s", InfrastructureDirectory, "2rightarrow.png");
   snprintf((char*)&UpArrowImage, sizeof(UpArrowImage), "%s/%s", InfrastructureDirectory, "1uparrow.png");
   snprintf((char*)&PlayImage, sizeof(PlayImage), "%s/%s", InfrastructureDirectory, "player_play.png");
   snprintf((char*)&PauseImage, sizeof(PauseImage), "%s/%s", InfrastructureDirectory, "player_pause.png");
   snprintf((char*)&ShuffleImage, sizeof(ShuffleImage), "%s/%s", InfrastructureDirectory, "rotate.png");

   snprintf((char*)&ImageViewerScript, sizeof(ImageViewerScript), "%s/%s", InfrastructureDirectory, "imageviewer.js");
   snprintf((char*)&SlideshowScript, sizeof(SlideshowScript), "%s/%s", InfrastructureDirectory, "slideshow.js");
   snprintf((char*)&SlideshowControlBody, sizeof(SlideshowControlBody), "%s/%s", InfrastructureDirectory, "slideshowcontrol.html");

   safestrcpy((char*)&SlideshowFilelist, "slideshow-filelist-allblocks.js", sizeof(SlideshowFilelist));
   safestrcpy((char*)&SlideshowFrameset, "slideshow-frameset-allblocks.html", sizeof(SlideshowFrameset));
   safestrcpy((char*)&SlideshowControl, "slideshow-control-allblocks.html", sizeof(SlideshowControl));

   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s/infrastructure", DirectoryName);
   makeDir(str);

   if(Stylesheet[0] != 0x00) {
      snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(Stylesheet));
      copy(Stylesheet, str);
      safestrcpy(Stylesheet, getFileNameOneDirDown(str), sizeof(Stylesheet));
   }
   if(Background[0] != 0x00) {
      snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(Background));
      copy(Background, str);
      safestrcpy(Background, getFileNameOneDirDown(str), sizeof(Background));
   }
   if(ShortcutIcon[0] != 0x00) {
      snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(ShortcutIcon));
      copy(ShortcutIcon, str);
      safestrcpy(ShortcutIcon, getFileNameOneDirDown(str), sizeof(ShortcutIcon));
   }

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(OneLeftArrowImage));
   copy(OneLeftArrowImage, str);
   safestrcpy(OneLeftArrowImage, getFileNameOneDirDown(str), sizeof(OneLeftArrowImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(TwoLeftArrowImage));
   copy(TwoLeftArrowImage, str);
   safestrcpy(TwoLeftArrowImage, getFileNameOneDirDown(str), sizeof(TwoLeftArrowImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(OneRightArrowImage));
   copy(OneRightArrowImage, str);
   safestrcpy(OneRightArrowImage, getFileNameOneDirDown(str), sizeof(OneRightArrowImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(TwoRightArrowImage));
   copy(TwoRightArrowImage, str);
   safestrcpy(TwoRightArrowImage, getFileNameOneDirDown(str), sizeof(TwoRightArrowImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(UpArrowImage));
   copy(UpArrowImage, str);
   safestrcpy(UpArrowImage, getFileNameOneDirDown(str), sizeof(UpArrowImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(PlayImage));
   copy(PlayImage, str);
   safestrcpy(PlayImage, getFileNameOneDirDown(str), sizeof(PlayImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(PauseImage));
   copy(PauseImage, str);
   safestrcpy(PauseImage, getFileNameOneDirDown(str), sizeof(PauseImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(ShuffleImage));
   copy(ShuffleImage, str);
   safestrcpy(ShuffleImage, getFileNameOneDirDown(str), sizeof(ShuffleImage));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(ImageViewerScript));
   copy(ImageViewerScript, str);
   safestrcpy(ImageViewerScript, getFileNameOneDirDown(str), sizeof(ImageViewerScript));

   snprintf((char*)&str, sizeof(str), "%s/infrastructure/%s", DirectoryName, extractFileName(SlideshowScript));
   copy(SlideshowScript, str);
   safestrcpy(SlideshowScript, getFileNameOneDirDown(str), sizeof(SlideshowScript));
}


// ###### Create redirect page ##############################################
void Presentation::createRedirectPage()
{
   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s-index.html", DirectoryName);
   ofstream os(str);
   if(os.good()) {

      // ====== Header =======================================================
      os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
         << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl
         << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl
         << "<head>" << endl;
      if(Stylesheet[0] != 0x00) {
         os << "<link rel=\"stylesheet\" href=\"" << Stylesheet << "\" type=\"text/css\" />" << endl;
      }
      if(ShortcutIcon[0] != 0x00) {
         os << "<link rel=\"shortcut icon\" href=\"" << ShortcutIcon << "\" type=\"image/png\" />" << endl;
      }
      os << "<title>" << MainTitle << "</title>" << endl;
      os << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />" << endl;
      os << "<meta http-equiv=\"refresh\" content=\"0; url=" << DirectoryName << "/index.html\" />" << endl;
      os << "</head>" << endl << endl;

      os << "<body>" << endl
         << "<h1>Redirect</h1>" << endl
         << "<p>You will be automatically forwarded to the new location of this page. If forwarding does not work, click  <a href=\"" << DirectoryName << "/index.html\">here</a>!" << endl
         << "</p>" << endl
         << "<h1>Weiterleitung</h1>" << endl
         << "<p>Sie werden automatisch an den neuen Speicherort dieser Seite weitergeleitet. Wenn die Weiterleitung nicht funktioniert, klicken Sie <a href=\"" << DirectoryName << "/index.html\">hier</a>!" << endl
         << "</p>" << endl
         << "</body>" << endl << endl;

      os << "</html>" << endl;
   }
}


// ###### Create main page ##################################################
void Presentation::createMainPage()
{
   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s/%s", DirectoryName, PresentationName);
   ofstream os(str);
   if(os.good()) {

      // ====== Header =======================================================
      os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
         << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl
         << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl
         << "<head>" << endl;
      if(Stylesheet[0] != 0x00) {
         os << "<link rel=\"stylesheet\" href=\"" << Stylesheet << "\" type=\"text/css\" />" << endl;
      }
      if(ShortcutIcon[0] != 0x00) {
         os << "<link rel=\"shortcut icon\" href=\"" << ShortcutIcon << "\" type=\"image/png\" />" << endl;
      }
      os << "<title>" << MainTitle << "</title>" << endl;
      os << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />" << endl;
      os  << "<meta name=\"author\" content=\"" << Author << "\" />" << endl;
      os << "<meta name=\"description\" content=\"" << MainTitle << "\" />" << endl;
      os << "<meta name=\"keywords\" content=\"Slideshow" << MainTitle;
      if(Author[0] != 0x00) {
         os << ", " << Author;
      }
      os << "\" />" << endl;
      os << "<meta name=\"classification\" content=\"Slideshow\" />" << endl;
      os << "</head>" << endl << endl;


      // ====== Body =========================================================
      os << "<body>" << endl;
      if(Head[0] != 0x00) {
         if(!cat(os, Head)) {
            cerr << "ERROR: Unable to copy head from file \"" << Head << "\"!" << endl;
            exit(1);
         }
      }
      os << "<h1>" << MainTitle << "</h1>" << endl;
      if(MainDescription[0] != 0x00) {
         if(!cat(os, MainDescription)) {
            cerr << "ERROR: Unable to copy main description from file \"" << MainDescription << "\"!" << endl;
            exit(1);
         }
      }
      if(Index) {
         os << "<h2 id=\"index\">Index</h2>" << endl
            << "<ul>" << endl;
         set<Block*>::iterator blockIterator = BlockSet.begin();
         while(blockIterator != BlockSet.end()) {
            Block* block = *blockIterator;
            os << "   <li><a href=\"#" << block->DirectoryName << "\">"
               << block->Title << "</a></li>" << endl;
            blockIterator++;
         }
         if(Slideshow) {
            os << "   <li><strong><a href=\"" << SlideshowFrameset << "\">"
               << "View Slideshow" << "</a></strong></li>" << endl;
         }
         os << "</ul>" << endl
            << "<hr />"  << endl
            << endl;
      }


      // ====== Preview Tables ===============================================
      set<Block*>::iterator blockIterator = BlockSet.begin();
      while(blockIterator != BlockSet.end()) {
         Block* block = *blockIterator;
         os << "<h1 id=\"" << block->DirectoryName << "\">" << block->Title << "</h1>" << endl;
         if(Slideshow) {
            os << "<p class=\"center\">" << endl
               << "<strong><a href=\"" << block->SlideshowFrameset << "\">"
               << "View Slideshow" << "</a></strong>" << endl
               << "</p>" << endl;
         }
         if(block->Description[0] != 0x00) {
            if(!cat(os, block->Description)) {
               cerr << "ERROR: Unable to copy description from file \"" << block->Description << "\"!" << endl;
               exit(1);
            }
         }
         os << "<table class=\"previewtable\">" << endl;

         size_t           row       = 0;
         size_t           column    = ~0;
         bool         rowOpened = false;
         set<Image*>::iterator imageIterator = block->ImageSet.begin();
         while(imageIterator != block->ImageSet.end()) {
            Image* image = *imageIterator;
            if(column > block->Columns) {
               if(rowOpened) {
                  os << "   </tr>" << endl;
               }
               os << "   <tr>" << endl;
               rowOpened = true;
               column    = 0;
               row++;
            }

            os << "      <td class=\"previewtable\">"
               << "<a href=\"" << image->ViewName << "\">"
               << "<img "
               << "width=\""  << image->PreviewWidth  << "\" "
               << "height=\"" << image->PreviewHeight << "\" "
               << "alt=\""    << image->Title         << "\" "
               << "src=\""    << image->OwnerBlock->DirectoryName << "/" << image->PreviewName   << "\""
               << " /><br />"
               << image->Title
               << "</a>"
               << "</td>"
               << endl;

            column++;
            imageIterator++;
         }
         if(rowOpened) {
            os << "   </tr>" << endl;
         }
         os << "</table>" << endl << endl;

         blockIterator++;
      }

      // ====== Tail =========================================================
      if(Tail[0] != 0x00) {
         if(!cat(os, Tail)) {
            cerr << "ERROR: Unable to copy tail from file \"" << Tail << "\"!" << endl;
            exit(1);
         }
      }
      os << "</body>" << endl
         << "</html>" << endl;
   }
   else {
      cerr << "ERROR: Unable to create file \"" << str << "\"!" << endl;
      exit(1);
   }
}


// ###### Create views pages ################################################
void Presentation::createViewPages(const bool showOriginal,
                                   const bool forSlideshow)
{
   set<Block*>::iterator blockIterator = BlockSet.begin();
   Block* prevBlock = NULL;
   Block* nextBlock = NULL;
   while(blockIterator != BlockSet.end()) {
      set<Block*>::iterator nextBlockIterator = blockIterator;
      nextBlockIterator++;
      if(nextBlockIterator != BlockSet.end()) {
         nextBlock = *nextBlockIterator;
      }
      else {
         nextBlock = NULL;
      }
      (*blockIterator)->createViewPages(prevBlock, nextBlock, showOriginal, forSlideshow);
      prevBlock = *blockIterator;
      blockIterator++;
   }
}



// ###### Constructor #######################################################
Block::Block(Presentation*      presentation,
             const char*        blockTitle,
             const char*        blockDescription,
             const size_t           columns)
{
   OwnerPresentation = presentation;
   ID = ++OwnerPresentation->LastBlockID;
   snprintf((char*)&Title, sizeof(Title), "%s", blockTitle);
   safestrcpy((char*)&Description, blockDescription, sizeof(Description));
   snprintf((char*)&DirectoryName, sizeof(DirectoryName), "block-%04u", (unsigned int)ID);
   safestrcpy((char*)&OriginalDirectory, "original", sizeof(OriginalDirectory));
   safestrcpy((char*)&FullsizeDirectory, "fullsize", sizeof(FullsizeDirectory));
   safestrcpy((char*)&PreviewDirectory, "preview", sizeof(PreviewDirectory));
   snprintf((char*)&SlideshowFilelist, sizeof(SlideshowFilelist), "slideshow-filelist-%s.js", DirectoryName);
   snprintf((char*)&SlideshowFrameset, sizeof(SlideshowFrameset), "slideshow-frameset-%s.html", DirectoryName);
   snprintf((char*)&SlideshowControl, sizeof(SlideshowControl), "slideshow-control-%s.html", DirectoryName);
   Columns     = columns;
   LastImageID = 0;
   OwnerPresentation->BlockSet.insert(this);
}


// ###### Dump block information ############################################
void Block::dump()
{
   cout << "      + Block #" << ID << " \"" << Title << "\"" << endl
        << "         % DirectoryName=" << DirectoryName << endl
        << "         % SlideshowFilelist=" << SlideshowFilelist << endl
        << "         % SlideshowFrameset=" << SlideshowFrameset << endl
        << "         % SlideshowControl=" << SlideshowControl << endl;
   set<Image*>::iterator imageIterator = ImageSet.begin();
   while(imageIterator != ImageSet.end()) {
      (*imageIterator)->dump();
      imageIterator++;
   }
}


// ###### Create block directories ##########################################
void Block::createDirectories()
{
   makeDir(OwnerPresentation->DirectoryName, DirectoryName);
   makeDir(OwnerPresentation->DirectoryName, DirectoryName, OriginalDirectory);
   makeDir(OwnerPresentation->DirectoryName, DirectoryName, FullsizeDirectory);
   makeDir(OwnerPresentation->DirectoryName, DirectoryName, PreviewDirectory);
}


// ###### Create block images ###############################################
void Block::createImages()
{
   set<Image*>::iterator imageIterator = ImageSet.begin();
   while(imageIterator != ImageSet.end()) {
      (*imageIterator)->createImage();
      imageIterator++;
   }
}


// ###### Create block view pages ###########################################
void Block::createViewPages(const Block* prevBlock,
                            const Block* nextBlock,
                            const bool   showOriginal,
                            const bool   forSlideshow)
{
   set<Image*>::iterator ImageIterator = ImageSet.begin();
   Image* prevImage = NULL;
   Image* nextImage = NULL;
   while(ImageIterator != ImageSet.end()) {
      set<Image*>::iterator nextImageIterator = ImageIterator;
      nextImageIterator++;
      if(nextImageIterator != ImageSet.end()) {
         nextImage = *nextImageIterator;
      }
      else {
         nextImage = NULL;
      }
      (*ImageIterator)->createViewPage(prevBlock, nextBlock, prevImage, nextImage, showOriginal, forSlideshow);
      prevImage = *ImageIterator;
      ImageIterator++;
   }
}



// ###### Constructor #######################################################
Image::Image(Block* block, const char* imageTitle, const char* sourceName)
{
   OwnerBlock = block;
   ID         = ++OwnerBlock->LastImageID;

   char baseName[1024];
   snprintf((char*)&baseName, sizeof(baseName), "image-%04u", (unsigned int)ID);

   if(Title[0] == 0x00) {
      if(OwnerBlock->OwnerPresentation->Enumerate) {
         snprintf((char*)&Title, sizeof(Title), "Image %u", (unsigned int)ID);
      }
      else {
         safestrcpy((char*)&Title, extractFileName(sourceName), sizeof(Title));
      }
   }
   else {
      safestrcpy((char*)&Title, imageTitle, sizeof(Title));
   }

   snprintf((char*)&SourceName, sizeof(SourceName), "%s", sourceName);

   char        extension[1024];
   const char* extPtr = rindex(SourceName, '.');
   if(extPtr) {
      if(index(extPtr, '/')) {
         extPtr = "";
      }
   }
   else {
      extPtr = "";
   }
   safestrcpy((char*)&extension, extPtr, sizeof(extension));
   webify((char*)extension);
   if(strcmp(extension, ".jpg")) {
      safestrcpy((char*)&extension, ".jpeg", sizeof(extension));
   }

   snprintf((char*)&OriginalName, sizeof(OriginalName), "%s/%s%s", OwnerBlock->OriginalDirectory, baseName, extension);
   snprintf((char*)&FullsizeName, sizeof(FullsizeName), "%s/%s.jpeg", OwnerBlock->FullsizeDirectory, baseName);
   snprintf((char*)&PreviewName, sizeof(PreviewName), "%s/%s.jpeg", OwnerBlock->PreviewDirectory, baseName);
   snprintf((char*)&ViewName, sizeof(ViewName), "view-block%04u-%s.html", (unsigned int)block->ID, baseName);
   snprintf((char*)&SlideshowName, sizeof(SlideshowName), "slideshow-block%04u-%s.html", (unsigned int)block->ID, baseName);
   OriginalWidth  = 0;
   OriginalHeight = 0;
   FullsizeWidth  = 0;
   FullsizeHeight = 0;
   PreviewWidth   = 0;
   PreviewHeight  = 0;
   OwnerBlock->ImageSet.insert(this);
}


// ###### Dump image information ############################################
void Image::dump()
{
   cout << "         % Image #" << ID << " \"" << Title << "\"" << endl
        << "            ~ SourceName=" << SourceName << endl
        << "            ~ OriginalName=" << OriginalName << " (" << OriginalWidth << "x" << OriginalHeight << ")" << endl
        << "            ~ FullsizeName=" << FullsizeName << " (" << FullsizeWidth << "x" << FullsizeHeight << ")" << endl
        << "            ~ PreviewName=" << PreviewName << " (" << PreviewWidth << "x" << PreviewHeight << ")" << endl
        << "            ~ ViewName=" << ViewName << endl
        << "            ~ SlideshowName=" << SlideshowName << endl;
}


// ###### Dump image view page ##############################################
void Image::createViewPage(const Block* prevBlock, const Block* nextBlock,
                           const Image* prevImage, const Image* nextImage,
                           const bool   showOriginal,
                           const bool   forSlideshow)
{
   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s/%s",
            OwnerBlock->OwnerPresentation->DirectoryName,
            (forSlideshow == true) ? SlideshowName : ViewName);
   ofstream os(str);
   if(os.good()) {

      // ====== Header =======================================================
      os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
         << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl
         << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl
         << "<head>" << endl;
      if(OwnerBlock->OwnerPresentation->Stylesheet[0] != 0x00) {
         os << "<link rel=\"stylesheet\" href=\"" << OwnerBlock->OwnerPresentation->Stylesheet << "\" type=\"text/css\" />" << endl;
      }
      if(OwnerBlock->OwnerPresentation->ShortcutIcon[0] != 0x00) {
         os << "<link rel=\"shortcut icon\" href=\"" << OwnerBlock->OwnerPresentation->ShortcutIcon << "\" type=\"image/png\" />" << endl;
      }
      os << "<title>" << OwnerBlock->OwnerPresentation->MainTitle << " - " << Title << "</title>" << endl
         << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />" << endl
         << "<meta name=\"author\" content=\"" << OwnerBlock->OwnerPresentation->Author << "\" />" << endl
         << "<meta name=\"description\" content=\"" << OwnerBlock->OwnerPresentation->MainTitle << ", " << Title << "\" />" << endl
         << "<meta name=\"keywords\" content=\"Slideshow" << OwnerBlock->OwnerPresentation->MainTitle << ", " << Title;
      if(OwnerBlock->OwnerPresentation->Author[0] != 0x00) {
         os << ", " << OwnerBlock->OwnerPresentation->Author;
      }
      os << "\" />" << endl
         << "<meta name=\"classification\" content=\"Image, " << Title << "\" />" << endl
         << "<script type=\"text/javascript\" src=\"" << OwnerBlock->OwnerPresentation->ImageViewerScript << "\"></script>" << endl;
      if(prevImage) {
         os << "<link rel=\"next\" href=\"" << ((forSlideshow == true) ? prevImage->SlideshowName : prevImage->ViewName) << "\" />" << endl
            << "<link rel=\"next\" href=\"" << prevImage->OwnerBlock->DirectoryName << "/" << prevImage->FullsizeName << "\" />" << endl;
      }
      if(nextImage) {
         os << "<link rel=\"next\" href=\"" << ((forSlideshow == true) ? nextImage->SlideshowName : nextImage->ViewName) << "\" />" << endl
            << "<link rel=\"next\" href=\"" << nextImage->OwnerBlock->DirectoryName << "/" << nextImage->FullsizeName << "\" />" << endl;
      }
      os << "</head>" << endl << endl;


      // ====== Body =========================================================
      os << "<body>" << endl;
      if(!forSlideshow) {
         if(OwnerBlock->OwnerPresentation->Head[0] != 0x00) {
            if(!cat(os, OwnerBlock->OwnerPresentation->Head)) {
               cerr << "ERROR: Unable to copy head from file \"" << OwnerBlock->OwnerPresentation->Head << "\"!" << endl;
               exit(1);
            }
         }
      }
      os << "<h1>" << Title << "</h1>" << endl;

      os << "<script type=\"text/javascript\">" << endl
         << "<!--" << endl
         << "   show( \"" << OwnerBlock->DirectoryName << "/" << FullsizeName << "\", " << FullsizeWidth << ", " << FullsizeHeight << ");" << endl
         << "-->" << endl
         << "</script>" << endl;
      os << "<noscript>" << endl
         << "   <p class=\"center\">" << endl
         << "   <img width=\"" << FullsizeWidth << "\" height=\"" << FullsizeHeight << "\" alt=\"" << Title << "\" src=\"" << OwnerBlock->DirectoryName << "/" << FullsizeName << "\" />" << endl
         << "   </p>" << endl
         << "</noscript>" << endl;

      os << "<p class=\"center\">" << endl;
      if(!forSlideshow) {
         os << "<br />Full view of <em>" << Title << "</em>" << endl
            << "<br /><a href=\"" << OwnerBlock->DirectoryName << "/" << OriginalName << "\">Get the original file</a><br />" << endl;

         os << "<a href=\"" << OwnerBlock->OwnerPresentation->PresentationName << "#" << OwnerBlock->DirectoryName << "\"><img alt=\"Preview Page\" src=\"" << OwnerBlock->OwnerPresentation->UpArrowImage << "\" /></a> ";
         if(prevBlock) {
            os << "<a href=\"" << prevBlock->OwnerPresentation->PresentationName << "#" << prevBlock->DirectoryName << "\">";
         }
         os << "<img alt=\"Previous Block\" src=\"" << OwnerBlock->OwnerPresentation->TwoLeftArrowImage << "\" />";
         if(prevBlock) {
            os << "</a>";
         }
         os << " ";

         if(prevImage) {
            os << "<a href=\"" << prevImage->ViewName << "\">";
         }
         os << "<img alt=\"Previous Image\" src=\"" << OwnerBlock->OwnerPresentation->OneLeftArrowImage << "\" />";
         if(prevImage) {
            os << "</a>";
         }
         os << " ";

         if(nextImage) {
            os << "<a href=\"" << nextImage->ViewName << "\">";
         }
         os << "<img alt=\"Next Image\" src=\"" << OwnerBlock->OwnerPresentation->OneRightArrowImage << "\" />";
         if(nextImage) {
            os << "</a>";
         }
         os << " ";

         if(nextBlock) {
            os << "<a href=\"" << nextBlock->OwnerPresentation->PresentationName << "#" << nextBlock->DirectoryName << "\">";
         }
         os << "<img alt=\"Next Block\" src=\"" << OwnerBlock->OwnerPresentation->TwoRightArrowImage << "\" />";
         if(nextBlock) {
            os << "</a>";
         }

         os << " ";
      }

      os << "</p>" << endl;
      if(!forSlideshow) {
         if(OwnerBlock->OwnerPresentation->Tail[0] != 0x00) {
            if(!cat(os, OwnerBlock->OwnerPresentation->Tail)) {
               cerr << "ERROR: Unable to copy tail from file \"" << OwnerBlock->OwnerPresentation->Tail << "\"!" << endl;
               exit(1);
            }
         }
      }
      os << "</body>" << endl
         << "</html>" << endl;
   }
   else {
      cerr << "ERROR: Unable to create file \"" << str << "\"!" << endl;
      exit(1);
   }
}



// ###### Create presentation ###############################################
Presentation* createPresentation(int argc, char** argv)
{
   Presentation* presentation = new Presentation;
   CHECK(presentation);

   for(int i = 1;i < argc;i++) {
      if(!(strncmp(argv[i], "--infrastructure=", 17))) {
         safestrcpy(presentation->InfrastructureDirectory, (char*)&argv[i][17], sizeof(presentation->InfrastructureDirectory));
      }
      else if(!(strncmp(argv[i], "--maintitle=", 12))) {
         safestrcpy(presentation->MainTitle, (char*)&argv[i][12], sizeof(presentation->MainTitle));
      }
      else if(!(strncmp(argv[i], "--maindescription=", 18))) {
         safestrcpy(presentation->MainDescription, (char*)&argv[i][18], sizeof(presentation->MainDescription));
      }
      else if(!(strncmp(argv[i], "--stylesheet=", 13))) {
         safestrcpy(presentation->Stylesheet, (char*)&argv[i][13], sizeof(presentation->Stylesheet));
      }
      else if(!(strncmp(argv[i], "--background=", 13))) {
         safestrcpy(presentation->Background, (char*)&argv[i][13], sizeof(presentation->Background));
      }
      else if(!(strncmp(argv[i], "--shortcuticon=", 15))) {
         safestrcpy(presentation->ShortcutIcon, (char*)&argv[i][15], sizeof(presentation->ShortcutIcon));
      }
      else if(!(strncmp(argv[i], "--directory=", 12))) {
         safestrcpy(presentation->DirectoryName, (char*)&argv[i][12], sizeof(presentation->DirectoryName));
      }
      else if(!(strncmp(argv[i], "--head=", 7))) {
         safestrcpy(presentation->Head, (char*)&argv[i][7], sizeof(presentation->Head));
      }
      else if(!(strncmp(argv[i], "--tail=", 7))) {
         safestrcpy(presentation->Tail, (char*)&argv[i][7], sizeof(presentation->Tail));
      }
      else if(!(strcmp(argv[i], "--enumerate"))) {
         presentation->Enumerate = true;
      }
      else if(!(strcmp(argv[i], "--noenumerate"))) {
         presentation->Enumerate = false;
      }
      else if(!(strcmp(argv[i], "--index"))) {
         presentation->Index = true;
      }
      else if(!(strcmp(argv[i], "--noindex"))) {
         presentation->Index = false;
      }
      else if(!(strcmp(argv[i], "--slideshow"))) {
         presentation->Slideshow = true;
      }
      else if(!(strcmp(argv[i], "--noslideshow"))) {
         presentation->Slideshow = false;
      }
   }

   safestrcpy((char*)&presentation->PresentationName, "index.html", sizeof(presentation->PresentationName));
   return(presentation);
}


// ###### Create image table ################################################
void createImageTable(Presentation* presentation, int argc, char** argv)
{
   Block*      currentBlock     = NULL;
   const char* blockTitle       = "";
   const char* blockDescription = "";
   const char* imageTitle       = "";
   size_t      columns          = presentation->Columns;

   for(int i = 1;i < argc;i++) {
      if(strncmp(argv[i], "--", 2)) {
         if(currentBlock == NULL) {
            cout << "   + Creating block \"" << blockTitle << "\"..." << endl;
            currentBlock = new Block(presentation, blockTitle, blockDescription, columns);
            CHECK(currentBlock);
            blockTitle       = "";
            blockDescription = "";
         }

         cout << "      * Checking image \"" << argv[i] << "\"..." << endl;
         Image* currentImage = new Image(currentBlock, imageTitle, argv[i]);
         CHECK(currentImage);
         imageTitle = "";
      }
      else if(!(strncmp(argv[i], "--block=", 8))) {
         blockTitle   = (char*)&argv[i][8];
         currentBlock = NULL;
      }
      else if(!(strncmp(argv[i], "--blockdescription=", 19))) {
         blockDescription = (char*)&argv[i][19];
         currentBlock     = NULL;
      }
      else if(!(strncmp(argv[i], "--imagetitle=", 13))) {
         imageTitle   = (char*)&argv[i][13];
      }
      else if(!(strncmp(argv[i], "--columns=", 10))) {
         columns = atol((char*)&argv[i][10]);
         if(columns < 1) {
            columns = 1;
         }
      }
      else if(!(strncmp(argv[i], "--previewdith=", 14))) {
         presentation->PreviewWidth = atol((char*)&argv[i][14]);
         if(presentation->PreviewWidth < 16) {
            presentation->PreviewWidth = 16;
         }
      }
      else if(!(strncmp(argv[i], "--previewheight=", 16))) {
         presentation->PreviewHeight = atol((char*)&argv[i][16]);
         if(presentation->PreviewHeight < 16) {
            presentation->PreviewHeight = 16;
         }
      }
      else if(!(strncmp(argv[i], "--previewquality=", 17))) {
         presentation->PreviewQuality = atol((char*)&argv[i][17]);
         if(presentation->PreviewQuality < 1) {
            presentation->PreviewQuality = 1;
         }
         else if(presentation->PreviewQuality > 100) {
            presentation->PreviewQuality = 100;
         }
      }
      else if(!(strncmp(argv[i], "--fullsizewidth=", 16))) {
         presentation->FullsizeWidth = atol((char*)&argv[i][16]);
         if(presentation->FullsizeWidth < 16) {
            presentation->FullsizeWidth = 16;
         }
      }
      else if(!(strncmp(argv[i], "--fullsizeheight=", 17))) {
         presentation->FullsizeHeight = atol((char*)&argv[i][17]);
         if(presentation->FullsizeHeight < 16) {
            presentation->FullsizeHeight = 16;
         }
      }
      else if(!(strncmp(argv[i], "--fullsizequality=", 18))) {
         presentation->FullsizeQuality = atol((char*)&argv[i][18]);
         if(presentation->FullsizeQuality < 1) {
            presentation->FullsizeQuality = 1;
         }
         else if(presentation->FullsizeQuality > 100) {
            presentation->FullsizeQuality = 100;
         }
      }
      else if(!(strcmp(argv[i], "--htmlonly"))) {
         presentation->SkipImageConversion = true;
      }
   }
}


// ###### Create image ######################################################
void Image::createImage()
{
   char originalImageName[1024];
   char previewImageName[1024];
   char fullsizeImageName[1024];

   cout << "Preparing image \"" << SourceName << "..." << endl;
   snprintf((char*)&originalImageName, sizeof(originalImageName), "%s/%s/%s",
            OwnerBlock->OwnerPresentation->DirectoryName,
            OwnerBlock->DirectoryName,
            OriginalName);
   snprintf((char*)&previewImageName, sizeof(previewImageName), "%s/%s/%s",
            OwnerBlock->OwnerPresentation->DirectoryName,
            OwnerBlock->DirectoryName,
            PreviewName);
   snprintf((char*)&fullsizeImageName, sizeof(fullsizeImageName), "%s/%s/%s",
            OwnerBlock->OwnerPresentation->DirectoryName,
            OwnerBlock->DirectoryName,
            FullsizeName);

   OriginalWidth  = OwnerBlock->OwnerPresentation->FullsizeWidth;
   OriginalHeight = OwnerBlock->OwnerPresentation->FullsizeHeight;
   PreviewWidth   = OwnerBlock->OwnerPresentation->PreviewWidth;
   PreviewHeight  = OwnerBlock->OwnerPresentation->PreviewHeight;
   FullsizeWidth  = OwnerBlock->OwnerPresentation->FullsizeWidth;
   FullsizeHeight = OwnerBlock->OwnerPresentation->FullsizeHeight;

   if(!OwnerBlock->OwnerPresentation->SkipImageConversion) {
      // ====== Copy original file ==========================================
      copy(SourceName, originalImageName);


      // ====== Create preview and fullsize images ==========================
      imageConverter(SourceName, previewImageName, fullsizeImageName,
                     OriginalWidth, OriginalHeight,
                     PreviewWidth, PreviewHeight,
                     OwnerBlock->OwnerPresentation->PreviewQuality,
                     FullsizeWidth, FullsizeHeight,
                     OwnerBlock->OwnerPresentation->FullsizeQuality);
   }
   else {
      imageTester(SourceName, previewImageName, fullsizeImageName,
                  OriginalWidth, OriginalHeight,
                  PreviewWidth, PreviewHeight,
                  FullsizeWidth, FullsizeHeight);
   }
}



// ###### Main program ######################################################
int main(int argc, char** argv)
{
   cout << "- Creating Presentation..." << endl;
   Presentation* presentation = createPresentation(argc, argv);

   cout << "- Creating Image Table..." << endl;
   createImageTable(presentation, argc, argv);

   cout << "- Creating directories..." << endl;
   presentation->createDirectories();

   cout << "- Creating HTML infrastructure files..." << endl;
   presentation->createInfrastructureFiles();

   cout << "- Converting Images..." << endl;
   presentation->createImages();

   cout << "- Overview" << endl;
   presentation->dump();

   cout << "- Creating main page..." << endl;
   presentation->createRedirectPage();
   presentation->createMainPage();

   cout << "- Creating View HTMLs..." << endl;
   presentation->createViewPages(false, false);

   cout << "- Creating Slideshow HTMLs..." << endl;
   presentation->createViewPages(false, true);
   presentation->createSlideshow();

   delete presentation;
}

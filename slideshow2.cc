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
#include <sys/time.h>
#include <pwd.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <set>

#include <Magick++.h>


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
   fprintf(fd,str);
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


void makeDir(const char* a, const char* b = NULL, const char* c = NULL)
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


// ###### Copy a file to an output stream ####################################
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



class Image;
class Block;



class Presentation
{
   public:
   Presentation();
   void dump();
   void createDirectories();
   void createInfrastructureFiles();
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

   unsigned int Columns;
   unsigned int PreviewWidth;
   unsigned int PreviewHeight;
   unsigned int PreviewQuality;
   unsigned int FullsizeWidth;
   unsigned int FullsizeHeight;
   unsigned int FullsizeQuality;

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
   char         ImageViewerScript[1024];
   char         SlideshowScript[1024];

   set<Block*>  BlockSet;
   unsigned int LastBlockID;

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
         const unsigned int columns);
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
   unsigned int  ID;
   unsigned int  Columns;
   char          Description[1024];
   char          DirectoryName[1024];
   char          OriginalDirectory[1024];
   char          FullsizeDirectory[1024];
   char          PreviewDirectory[1024];
   char          SlideshowFilelist[1024];
   char          SlideshowFrameset[1024];
   char          SlideshowControl[1024];

   set<Image*>   ImageSet;
   unsigned int  LastImageID;
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
   unsigned int ID;
   char         SourceName[1024];
   char         OriginalName[1024];
   char         FullsizeName[1024];
   char         PreviewName[1024];
   char         ViewName[1024];
   char         SlideshowName[1024];
   unsigned int OriginalWidth;
   unsigned int OriginalHeight;
   unsigned int FullsizeWidth;
   unsigned int FullsizeHeight;
   unsigned int PreviewWidth;
   unsigned int PreviewHeight;
};




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

   safestrcpy((char*)&MainTitle, "My Photo Archive", sizeof(MainTitle));
   safestrcpy((char*)&MainDescription, "", sizeof(MainDescription));
   safestrcpy((char*)&DirectoryName, "test-dir", sizeof(DirectoryName));
   safestrcpy((char*)&PresentationName, "index.html", sizeof(PresentationName));
   safestrcpy((char*)&Stylesheet, "", sizeof(Stylesheet));
   safestrcpy((char*)&Background, "", sizeof(Background));
   safestrcpy((char*)&ShortcutIcon, "", sizeof(ShortcutIcon));
   safestrcpy((char*)&Head, "", sizeof(Head));
   safestrcpy((char*)&Tail, "", sizeof(Tail));
   safestrcpy((char*)&OneLeftArrowImage, "infrastructure/1leftarrow.png", sizeof(OneLeftArrowImage));
   safestrcpy((char*)&TwoLeftArrowImage, "infrastructure/2leftarrow.png", sizeof(TwoLeftArrowImage));
   safestrcpy((char*)&OneRightArrowImage, "infrastructure/1rightarrow.png", sizeof(OneRightArrowImage));
   safestrcpy((char*)&TwoRightArrowImage, "infrastructure/2rightarrow.png", sizeof(TwoRightArrowImage));
   safestrcpy((char*)&UpArrowImage, "infrastructure/1uparrow.png", sizeof(UpArrowImage));
   safestrcpy((char*)&PlayImage, "infrastructure/player_play.png", sizeof(PlayImage));
   safestrcpy((char*)&PauseImage, "infrastructure/player_pause.png", sizeof(PauseImage));
   safestrcpy((char*)&ShuffleImage, "infrastructure/rotate.png", sizeof(ShuffleImage));
   safestrcpy((char*)&SlideshowFilelist, "slideshow-filelist-allblocks.js", sizeof(SlideshowFilelist));
   safestrcpy((char*)&SlideshowFrameset, "slideshow-frameset-allblocks.html", sizeof(SlideshowFrameset));
   safestrcpy((char*)&SlideshowControl, "slideshow-control-allblocks.html", sizeof(SlideshowControl));
   safestrcpy((char*)&ImageViewerScript, "imageviewer.js", sizeof(ImageViewerScript));
   safestrcpy((char*)&SlideshowScript, "slideshow.js", sizeof(SlideshowScript));

   passwd* pw = getpwuid(getuid());
   if(pw) {
      safestrcpy((char*)&Author, pw->pw_gecos, sizeof(Author));
   }
   else {
      safestrcpy((char*)&Author, "", sizeof(Author));
   }
}

void Presentation::dump()
{
   cout << "   - Presentation \"" << MainTitle << "\"" << endl
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
        << "      + PreviewQuality=" << PreviewQuality << " [%%]" << endl
        << "      + FullsizeWidth=" << FullsizeWidth << endl
        << "      + FullsizeHeight=" << FullsizeHeight << endl
        << "      + FullsizeQuality=" << FullsizeQuality << " [%%]" << endl
        << "      + ImageViewerScript=" << ImageViewerScript << endl
        << "      + SlideshowScript=" << SlideshowScript << endl;
   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      (*blockIterator)->dump();
      blockIterator++;
   }
}

void Presentation::createDirectories()
{
   makeDir(DirectoryName);
   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      (*blockIterator)->createDirectories();
      blockIterator++;
   }
}


void Presentation::createImages()
{
   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      (*blockIterator)->createImages();
      blockIterator++;
   }
}


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

   ssframeset << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">" << endl
          << "<html>"  << endl
          << "<head>"  << endl;
   ssframeset << "<title>" << MainTitle << "</title>" << endl;
   ssframeset << "<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-15\" />" << endl;
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
   sscontrol << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl
             << "<html>"  << endl
             << "<head>"  << endl;
   if(Stylesheet[0] != 0x00) {
      sscontrol << "<link rel=\"stylesheet\" href=\"" << Stylesheet << "\" type=\"text/css\" />" << endl;
   }
   if(ShortcutIcon[0] != 0x00) {
      sscontrol << "<link rel=\"shortcut icon\" href=\"" << ShortcutIcon << "\" type=\"image/png\" />" << endl;
   }
   sscontrol << "<title>" << MainTitle << "</title>" << endl;
   sscontrol << "<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-15\" />" << endl;
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
   if(!cat(sscontrol, "slideshowcontrol.html")) {
      cerr << "ERROR: Unable to copy slideshow control body from \"slideshowcontrol.html\"!" << endl;
      exit(1);
   }
   sscontrol << "</html>" << endl;
}


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


void Presentation::createInfrastructureFiles()
{
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


void Presentation::createMainPage()
{
   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s/%s", DirectoryName, PresentationName);
   ofstream os(str);
   if(os.good()) {

      // ====== Header =======================================================
      os << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xos11/DTD/xos11.dtd\">" << endl
           << "<html>"  << endl
           << "<head>"  << endl;
      if(Stylesheet[0] != 0x00) {
         os << "<link rel=\"stylesheet\" href=\"" << Stylesheet << "\" type=\"text/css\" />" << endl;
      }
      if(ShortcutIcon[0] != 0x00) {
         os << "<link rel=\"shortcut icon\" href=\"" << ShortcutIcon << "\" type=\"image/png\" />" << endl;
      }
      os << "<title>" << MainTitle << "</title>" << endl;
      os << "<meta http-equiv=\"content-type\" content=\"text/os; charset=ISO-8859-15\" />" << endl;
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
            os << "   <p />"
               << "   <li><a href=\"" << SlideshowFrameset << "\">"
               << "View Slideshow" << "</a></li>" << endl;
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

         unsigned int row       = 0;
         unsigned int column    = ~0;
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


Block::Block(Presentation*      presentation,
             const char*        blockTitle,
             const char*        blockDescription,
             const unsigned int columns)
{
   OwnerPresentation = presentation;
   ID = ++OwnerPresentation->LastBlockID;
   snprintf((char*)&Title, sizeof(Title), "%s", blockTitle);
   safestrcpy((char*)&Description, blockDescription, sizeof(Description));
   snprintf((char*)&DirectoryName, sizeof(DirectoryName), "block-%04u", ID);
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


void Block::createDirectories()
{
   makeDir(OwnerPresentation->DirectoryName, DirectoryName);
   makeDir(OwnerPresentation->DirectoryName, DirectoryName, OriginalDirectory);
   makeDir(OwnerPresentation->DirectoryName, DirectoryName, FullsizeDirectory);
   makeDir(OwnerPresentation->DirectoryName, DirectoryName, PreviewDirectory);
}


void Block::createImages()
{
   set<Image*>::iterator imageIterator = ImageSet.begin();
   while(imageIterator != ImageSet.end()) {
      (*imageIterator)->createImage();
      imageIterator++;
   }
}


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



Image::Image(Block* block, const char* imageTitle, const char* sourceName)
{
   OwnerBlock = block;
   ID         = ++OwnerBlock->LastImageID;

   char baseName[1024];
   snprintf((char*)&baseName, sizeof(baseName), "image-%04u", ID);

   if(Title[0] == 0x00) {
      if(OwnerBlock->OwnerPresentation->Enumerate) {
         snprintf((char*)&Title, sizeof(Title), "Image %u", ID);
      }
      else {
      // ????? Ausschneiden...
         safestrcpy((char*)&Title, extractFileName(sourceName), sizeof(Title));
      }
   }
   else {
      safestrcpy((char*)&Title, imageTitle, sizeof(Title));
   }
// ???? ENDUNG ...
   snprintf((char*)&SourceName, sizeof(SourceName), "%s", sourceName);
// ???? ENDUNG ...
   snprintf((char*)&OriginalName, sizeof(OriginalName), "%s/%s", OwnerBlock->OriginalDirectory, baseName);
   snprintf((char*)&FullsizeName, sizeof(FullsizeName), "%s/%s.jpeg", OwnerBlock->FullsizeDirectory, baseName);
   snprintf((char*)&PreviewName, sizeof(PreviewName), "%s/%s.jpeg", OwnerBlock->PreviewDirectory, baseName);
   snprintf((char*)&ViewName, sizeof(ViewName), "view-block%04u-%s.html", block->ID, baseName);
   snprintf((char*)&SlideshowName, sizeof(SlideshowName), "slideshow-block%04u-%s.html", block->ID, baseName);
   OriginalWidth  = 0;
   OriginalHeight = 0;
   FullsizeWidth  = 0;
   FullsizeHeight = 0;
   PreviewWidth   = 0;
   PreviewHeight  = 0;
   OwnerBlock->ImageSet.insert(this);
}

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
      os << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xos11/DTD/xos11.dtd\">" << endl
         << "<html>"  << endl
         << "<head>"  << endl;
      if(OwnerBlock->OwnerPresentation->Stylesheet[0] != 0x00) {
         os << "<link rel=\"stylesheet\" href=\"" << OwnerBlock->OwnerPresentation->Stylesheet << "\" type=\"text/css\" />" << endl;
      }
      if(OwnerBlock->OwnerPresentation->ShortcutIcon[0] != 0x00) {
         os << "<link rel=\"shortcut icon\" href=\"" << OwnerBlock->OwnerPresentation->ShortcutIcon << "\" type=\"image/png\" />" << endl;
      }
      os << "<title>" << OwnerBlock->OwnerPresentation->MainTitle << " - " << Title << "</title>" << endl
         << "<meta http-equiv=\"content-type\" content=\"text/os; charset=ISO-8859-15\" />" << endl
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


Presentation* createPresentation(int argc, char** argv)
{
   Presentation* presentation = new Presentation;
   CHECK(presentation);

   for(int i = 1;i < argc;i++) {
      if(!(strncmp(argv[i], "--maintitle=", 12))) {
         safestrcpy(presentation->MainTitle, (char*)&argv[i][12], sizeof(presentation->MainTitle));
      }
      if(!(strncmp(argv[i], "--maindescription=", 18))) {
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


void createImageTable(Presentation* presentation, int argc, char** argv)
{
   Block*       currentBlock     = NULL;
   char*        blockTitle       = "";
   char*        blockDescription = "";
   char*        imageTitle       = "";
   unsigned int columns          = presentation->Columns;

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
      else if(!(strncmp(argv[i], "--previewheight=", 15))) {
         presentation->PreviewHeight = atol((char*)&argv[i][15]);
         if(presentation->PreviewHeight < 16) {
            presentation->PreviewHeight = 16;
         }
      }
      else if(!(strncmp(argv[i], "--previewquality=", 16))) {
         presentation->PreviewQuality = atol((char*)&argv[i][16]);
         if(presentation->PreviewQuality < 1) {
            presentation->PreviewQuality = 1;
         }
         else if(presentation->PreviewQuality > 100) {
            presentation->PreviewQuality = 100;
         }
      }
      else if(!(strncmp(argv[i], "--fullsizedith=", 15))) {
         presentation->FullsizeWidth = atol((char*)&argv[i][15]);
         if(presentation->FullsizeWidth < 16) {
            presentation->FullsizeWidth = 16;
         }
      }
      else if(!(strncmp(argv[i], "--fullsizeheight=", 16))) {
         presentation->FullsizeHeight = atol((char*)&argv[i][16]);
         if(presentation->FullsizeHeight < 16) {
            presentation->FullsizeHeight = 16;
         }
      }
      else if(!(strncmp(argv[i], "--fullsizequality=", 17))) {
         presentation->FullsizeQuality = atol((char*)&argv[i][17]);
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


void Image::createImage()
{
   char str[1024];
   cout << "Preparing image \"" << SourceName << "..." << endl;

   if(!OwnerBlock->OwnerPresentation->SkipImageConversion) {
      Magick::Image original;
      try {
         original.read(SourceName);
         OriginalWidth  = original.size().width();
         OriginalHeight = original.size().height();

         Magick::Image preview = original;
         preview.interlaceType(Magick::LineInterlace);
         preview.quality(OwnerBlock->OwnerPresentation->PreviewQuality);
         preview.sample(Magick::Geometry(OwnerBlock->OwnerPresentation->PreviewWidth, OwnerBlock->OwnerPresentation->PreviewHeight));
         PreviewWidth  = preview.size().width();
         PreviewHeight = preview.size().height();
         snprintf((char*)&str, sizeof(str), "%s/%s/%s",
                  OwnerBlock->OwnerPresentation->DirectoryName,
                  OwnerBlock->DirectoryName,
                  PreviewName);
         preview.write(str);

         Magick::Image fullsize = original;
         fullsize.interlaceType(Magick::LineInterlace);
         fullsize.quality(OwnerBlock->OwnerPresentation->FullsizeQuality);
         fullsize.sample(Magick::Geometry(OwnerBlock->OwnerPresentation->FullsizeWidth, OwnerBlock->OwnerPresentation->FullsizeHeight));
         FullsizeWidth  = fullsize.size().width();
         FullsizeHeight = fullsize.size().height();
         snprintf((char*)&str, sizeof(str), "%s/%s/%s",
                  OwnerBlock->OwnerPresentation->DirectoryName,
                  OwnerBlock->DirectoryName,
                  FullsizeName);
         fullsize.write(str);
      } catch(Magick::Exception& exception) {
         cerr << "ERROR: Image prepararion failed: " << exception.what() << endl;
      }
   }
   else {
      OriginalWidth  = OwnerBlock->OwnerPresentation->FullsizeWidth;
      OriginalHeight = OwnerBlock->OwnerPresentation->FullsizeHeight;
      PreviewWidth   = OwnerBlock->OwnerPresentation->PreviewWidth;
      PreviewHeight  = OwnerBlock->OwnerPresentation->PreviewHeight;
      FullsizeWidth  = OwnerBlock->OwnerPresentation->FullsizeWidth;
      FullsizeHeight = OwnerBlock->OwnerPresentation->FullsizeHeight;
      try {
         Magick::Image original;
         original.ping(SourceName);
         OriginalWidth  = original.size().width();
         OriginalHeight = original.size().height();
      } catch(Magick::Exception& exception) {
         cerr << "WARNING: Image check failed: " << exception.what() << endl;
      }

      try {
         Magick::Image preview;
         snprintf((char*)&str, sizeof(str), "%s/%s/%s",
                  OwnerBlock->OwnerPresentation->DirectoryName,
                  OwnerBlock->DirectoryName,
                  PreviewName);
         preview.ping(str);
         PreviewWidth  = preview.size().width();
         PreviewHeight = preview.size().height();
      } catch(Magick::Exception& exception) {
         cerr << "WARNING: Image check failed: " << exception.what() << endl;
      }

      try {
         Magick::Image fullsize;
         snprintf((char*)&str, sizeof(str), "%s/%s/%s",
                  OwnerBlock->OwnerPresentation->DirectoryName,
                  OwnerBlock->DirectoryName,
                  FullsizeName);
         fullsize.ping(str);
         FullsizeWidth  = fullsize.size().width();
         FullsizeHeight = fullsize.size().height();
      } catch(Magick::Exception& exception) {
         cerr << "WARNING: Image check failed: " << exception.what() << endl;
      }
   }
}



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
   presentation->createMainPage();

   cout << "- Creating View HTMLs..." << endl;
   presentation->createViewPages(false, false);

   cout << "- Creating Slideshow HTMLs..." << endl;
   presentation->createViewPages(false, true);
   presentation->createSlideshow();

   delete presentation;
}

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

/* ###### Get current timer ############################################## */
unsigned long long getMicroTime()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return(((unsigned long long)tv.tv_sec * (unsigned long long)1000000) + (unsigned long long)tv.tv_usec);
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


// ###### Dump a file to an output stream ####################################
bool cat(ostream& os, const char* input)
{
   char str[8192];

   ifstream in(input);
   if(in.good()) {
      while(!in.eof()) {
         in.getline(str, sizeof(str), '\n');
         os << str << endl;
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
   void createMainPage();
   void createViewPages();

   public:
   bool         Enumerate;
   bool         Index;
   bool         Slideshow;

   unsigned int Columns;

   char         MainTitle[1024];
   char         MainDescription[1024];
   char         DirectoryName[1024];
   char         PresentationName[1024];
   char         Stylesheet[1024];
   char         ShortcutIcon[1024];
   char         Author[1024];
   char         Head[1024];
   char         Tail[1024];

   set<Block*>  BlockSet;
   unsigned int LastBlockID;
};

class Block
{
   public:
   Block(Presentation*      presentation,
         const char*        blockTitle,
         const char*        blockDescription,
         const unsigned int columns);
   void dump();
   void createDirectories();
   void createViewPages();

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

   set<Image*>   ImageSet;
   unsigned int  LastImageID;
};

class Image
{
   public:
   Image(Block* block, const char* imageTitle, const char* sourceName);
   void dump();
   void createViewPage();

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
   Enumerate = true;
   Index     = true;
   Columns   = 5;
   safestrcpy((char*)&MainTitle, "My Photo Archive", sizeof(MainTitle));
   safestrcpy((char*)&MainDescription, "", sizeof(MainDescription));
   safestrcpy((char*)&DirectoryName, "test-dir", sizeof(DirectoryName));
   safestrcpy((char*)&PresentationName, "index.html", sizeof(PresentationName));
   safestrcpy((char*)&Stylesheet, "", sizeof(Stylesheet));
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

void Presentation::dump()
{
   cout << "   - Presentation \"" << MainTitle << "\"" << endl
        << "      + MainTitle=" << MainTitle << endl
        << "      + MainDescription=" << MainDescription << endl
        << "      + DirectoryName=" << DirectoryName << endl
        << "      + PresentationName=" << PresentationName << endl
        << "      + Stylesheet=" << Stylesheet << endl
        << "      + ShortcutIcon=" << ShortcutIcon << endl;
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
         if(Slideshow) {
// ???            os << "   <li><strong><a href=\"" << ssmainname << "\">View slideshow</a></strong></li>" << endl;
         }
         set<Block*>::iterator blockIterator = BlockSet.begin();
         while(blockIterator != BlockSet.end()) {
            Block* block = *blockIterator;
            os << "   <li><a href=\"#" << block->DirectoryName << "\">"
               << block->Title << "</a></li>" << endl;
            blockIterator++;
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
/*
         html << "<p class=\"center\">" << endl
// ???              << "<strong><a href=\"" << subssmainname << "\">View slideshow</a></strong>" << endl
              << "</p>" << endl;
*/
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
               << "src=\""    << image->PreviewName   << "\""
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


void Presentation::createViewPages()
{
   set<Block*>::iterator blockIterator = BlockSet.begin();
   while(blockIterator != BlockSet.end()) {
      (*blockIterator)->createViewPages();
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
   Columns     = columns;
   LastImageID = 0;
   OwnerPresentation->BlockSet.insert(this);
}

void Block::dump()
{
   cout << "      + Block #" << ID << " \"" << Title << "\"" << endl
        << "         % DirectoryName=" << DirectoryName << endl;
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


void Block::createViewPages()
{
   set<Image*>::iterator imageIterator;
   while(imageIterator != ImageSet.end()) {
      (*imageIterator)->createViewPage();
      imageIterator++;
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
         safestrcpy((char*)&Title, sourceName, sizeof(Title));
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
   snprintf((char*)&ViewName, sizeof(ViewName), "view%s.html", baseName);
   snprintf((char*)&SlideshowName, sizeof(SlideshowName), "slideshow%s.html", baseName);
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


void Image::createViewPage()
{
   char str[1024];
   snprintf((char*)&str, sizeof(str), "%s", ViewName);
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
      os << "<title>" << OwnerBlock->OwnerPresentation->MainTitle << " - " << Title << "</title>" << endl;
      os << "<meta http-equiv=\"content-type\" content=\"text/os; charset=ISO-8859-15\" />" << endl;
      os  << "<meta name=\"author\" content=\"" << OwnerBlock->OwnerPresentation->Author << "\" />" << endl;
      os << "<meta name=\"description\" content=\"" << OwnerBlock->OwnerPresentation->Title << ", " << Title << "\" />" << endl;
      os << "<meta name=\"keywords\" content=\"Slideshow" << OwnerBlock->OwnerPresentation->Title << ", " << Title;
      if(OwnerBlock->OwnerPresentation->Author[0] != 0x00) {
         os << ", " << OwnerBlock->OwnerPresentation->Author;
      }
      os << "\" />" << endl;
      os << "<meta name=\"classification\" content=\"Image, " << Title << "\" />" << endl;
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
      else if(!(strncmp(argv[i], "--shortcuticon=", 14))) {
         safestrcpy(presentation->ShortcutIcon, (char*)&argv[i][14], sizeof(presentation->ShortcutIcon));
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
   }
}


int main(int argc, char** argv)
{
   cout << "- Creating Presentation..." << endl;
   Presentation* presentation = createPresentation(argc, argv);

   cout << "- Creating Image Table..." << endl;
   createImageTable(presentation, argc, argv);

   cout << "- Overview" << endl;
   presentation->dump();


   cout << "- Creating directories..." << endl;
   presentation->createDirectories();

   cout << "- Creating main page..." << endl;
   presentation->createMainPage();

   cout << "- Creating HTMLs..." << endl;
   presentation->createViewPages();

   delete presentation;
}

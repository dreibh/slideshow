/*
 *  $Id: slideshow.cc,v 1.2 2003/07/21 12:14:50 dreibh Exp $
 *
 * HTML 4.01 image presentation and JavaScript-based slideshow generator
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
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


// ###### Copy a file ########################################################
bool copy(const char* input,
          const char* output)
{
   bool  okay = false;
   char  buffer[8192];
   FILE* in  = fopen(input, "r");
   if(in != NULL) {
      FILE* out = fopen(output, "w");
      if(out != NULL) {
         okay = true;
         size_t length = fread((char*)&buffer, 1, sizeof(buffer),  in);
         while(length > 0) {
            if(fwrite((char*)&buffer, 1, length, out) != length) {
               okay= false;
               break;
            }
            length = fread((char*)&buffer, 1, sizeof(buffer),  in);
         }
         fclose(out);
      }
      else {
         cerr << "Unable to create \"" << output << "\"" << endl;
      }
      fclose(in);
   }
   else {
      cerr << "Unable to read \"" << input << "\"" << endl;
   }
   return(okay);
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


// ###### Create full-size image for presentation ############################
bool createFull(const char* input,
                const char* output,
                bool        htmlonly)
{
   cerr << "Full image for \"" << input << "\"..." << endl;

   char  cmd[1024];
   bool  result = false;
   snprintf((char*)&cmd, sizeof(cmd),"createfull \"%s\" \"%s\" %s",
            input, output,
            (htmlonly == true) ? "--htmlonly" : "");
   if(system(cmd) == 0) {
      result = true;
   }
   else {
      cerr << "ERROR: Unable to prepare full-size view for image \"" << input << "\"!" << endl;
   }

   return(htmlonly || result);
}


// ###### Create preview-size image for presentation #########################
bool createPreview(const char*   input,
                   const char*   output,
                   unsigned int& previewWidth,
                   unsigned int& previewHeight,
                   unsigned int& fullWidth,
                   unsigned int& fullHeight,
                   bool          htmlonly)
{
   cerr << "Preview for \"" << input << "\" in format "
        << previewWidth << "x" << previewHeight << "..." << endl;

   char  cmd[1024];
   char  sizes[512];
   bool  result = false;
   snprintf((char*)&sizes, sizeof(sizes), "%s.sizes", output);
   snprintf((char*)&cmd, sizeof(cmd),"createpreview \"%s\" \"%s\" %d %d %s %s",
            input, output, previewWidth, previewHeight, sizes,
            (htmlonly == true) ? "--htmlonly" : "");
  // cout << "bash# " << cmd << endl;

   fullWidth  = 2048;
   fullHeight = 1536;
   if(system(cmd) == 0) {
      FILE* in = fopen(sizes, "r");
      if(in != NULL) {
         if(fscanf(in, "W=%u H=%u\nW=%u H=%u\n",
            &fullWidth, &fullHeight, &previewWidth, &previewHeight) != 4) {
            cerr << "ERROR: Unable to obtain image dimensions for image \"" << input << "\"! Bad sizes file!" << endl;
         }
         else {
            result = true;
         }
         fclose(in);
      }
      else {
         cerr << "ERROR: Unable to obtain image dimensions for image \"" << input << "\"! No sizes file found!" << endl;
      }
   }
   else {
      cerr << "ERROR: Unable to prepare preview for image \"" << input << "\"!" << endl;
   }

   return(htmlonly || result);
}


// ###### Create slideshow control and mail HTML files #######################
bool createSlideshow(const char* ssmainname,
                     const char* sscontrolname,
                     ostream&    ssfiles,
                     const char* ssfilesname,
                     const char* mainPageName,
                     const char* presentationName,
                     const char* subdirName,
                     const char* stylesheet,
                     const char* maintitle,
                     passwd*     pw)
{
   ofstream ssmain(ssmainname);
   ofstream sscontrol(sscontrolname);
   if(!ssmain.good()) {
      cerr << "ERROR: Unable to create output file \"" << ssmainname << "\"!" << endl;
      exit(1);
   }
   if(!sscontrol.good()) {
      cerr << "ERROR: Unable to create output file \"" << sscontrolname << "\"!" << endl;
      exit(1);
   }

   ssfiles << "mainPage = \"" << mainPageName << "\";" << endl;
   ssfiles << "presentationName = \"" << presentationName << "-" << subdirName << "\";" << endl;

   ssmain << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\">" << endl
          << "<html lang=\"en\">"  << endl
          << "<head>"  << endl;
   ssmain << "<title>" << maintitle << "</title>" << endl;
   ssmain << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-15\">" << endl;
   if(pw) {
      ssmain  << "<meta name=\"Author\" content=\"" << pw->pw_gecos << "\">" << endl;
   }
   ssmain << "<meta name=\"Description\" content=\"" << maintitle << "\">" << endl;
   ssmain << "<meta name=\"Keywords\" content=\"Slideshow, " << maintitle;
   if(pw) {
      ssmain << ", " << pw->pw_gecos;
   }
   ssmain << "\">" << endl;
   ssmain << "<meta name=\"Classification\" content=\"Slideshow\">" << endl;
   ssmain << "</head>" << endl;
   ssmain << "<frameset rows=\"50px, 80%\">" << endl
          << "   <frame src=\"" << sscontrolname << "\" frameborder=\"1\" noresize scrolling=\"no\">" << endl
          << "   <frame frameborder=\"1\">" << endl
          << "</frameset>" << endl;
   ssmain << "</html>" << endl;

   sscontrol << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">" << endl
             << "<html lang=\"en\">"  << endl
             << "<head>"  << endl;
   if(stylesheet) {
      sscontrol << "<link rel=\"stylesheet\" href=\"" << stylesheet << "\" type=\"text/css\">" << endl;
   }
   sscontrol << "<title>" << maintitle << "</title>" << endl;
   sscontrol << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-15\">" << endl;
   if(pw) {
      sscontrol  << "<meta name=\"Author\" content=\"" << pw->pw_gecos << "\">" << endl;
   }
   sscontrol << "<meta name=\"Description\" content=\"" << maintitle << "\">" << endl;
   sscontrol << "<meta name=\"Keywords\" content=\"Slideshow, " << maintitle;
   if(pw) {
      sscontrol << ", " << pw->pw_gecos;
   }
   sscontrol << "\">" << endl;
   sscontrol << "<meta name=\"Classification\" content=\"Slideshow\">" << endl;
   sscontrol << "<script type=\"text/javascript\" src=\"slideshow.js\"></script>" << endl;
   sscontrol << "<script type=\"text/javascript\" src=\"" << ssfilesname << "\"></script>" << endl;
   sscontrol << "</head>" << endl << endl;
   if(!cat(sscontrol, "slideshowcontrol.html")) {
      cerr << "ERROR: Unable to copy slideshow control body from \"slideshowcontrol.html\"!" << endl;
      return(false);
   }
   sscontrol << "</html>" << endl;
   return(true);
}


// ###### Main program #######################################################
int main(int argc, char** argv)
{
   char         full[1024];
   char         original[1024];
   char         fullhtml[1048];
   char         preview[1024];
   char         webname[1024];
   char         ssmainname[1024];
   char         sscontrolname[1024];
   char         ssfilesname[1024];
   char         subsscontrolname[1024];
   char         subssmainname[1024];
   char         subssfilesname[1024];
   char         presentationname[1536];
   char         subdir[1536];
   char         tmp[1536];
   unsigned int cols   = 5;
   unsigned int width  = 128;
   unsigned int height = 96;
   unsigned int previewWidth;
   unsigned int previewHeight;
   unsigned int fullWidth;
   unsigned int fullHeight;
   unsigned int count           = 0;
   unsigned int r               = 0;
   unsigned int c               = 0;
   unsigned int s               = 0;
   unsigned int number          = 0;
   bool         createidx       = false;
   char*        maintitle       = "My Photo Archive";
   char*        title           = "Photo Archive";
   char*        stylesheet      = "stylesheet.css";
   char*        description     = NULL;
   char*        maindescription = NULL;
   bool         enumerate       = false;
   bool         htmlonly        = false;
   bool         sscreated       = false;
   ofstream     html;
   ofstream     ssmain;
   ofstream     ssfiles;
   ofstream     subssfiles;
   ofstream     sscontrol;

   if(argc < 2) {
      cerr << "Usage: " << argv[0] << " [Output file] {--htmlonly} {--enumerate} {--cols=Columns} {--width=Width} {-height=Height} {-stylesheet=CSS file} {--maintitle=Title} {--maindescription=HTML file} {--title=Title} {--subdir} {--description=File} {JPEG file 1} ..." << endl;
      exit(1);
   }
   if(argv[1][0] == '-') {
      cerr << "ERROR: First argument must be output HTML file name! This should not begin with '-'." << endl;
      exit(1);
   }

   passwd* pw = getpwuid(getuid());
   html.open(argv[1]);
   if(!html.good()) {
      cerr << "ERROR: Unable to create output file \"" << argv[1] << "\"!" << endl;
      exit(1);
   }
   snprintf((char*)&presentationname, sizeof(presentationname), "%s", argv[1]);
   webify(presentationname);
   char* a = rindex(presentationname, '/');
   char* b = rindex(presentationname, '.');
   if(a < b) {
      *b = 0x00;
   }
   snprintf((char*)&ssmainname, sizeof(ssmainname), "%s-slideshow.html", presentationname);
   snprintf((char*)&sscontrolname, sizeof(sscontrolname), "%s-sscontrol.html", presentationname);
   snprintf((char*)&ssfilesname, sizeof(ssfilesname), "%s-ssfiles.js", presentationname);
   ssfiles.open(ssfilesname);
   if(!ssfiles.good()) {
      cerr << "ERROR: Unable to create output file \"" << ssfilesname << "\"!" << endl;
      exit(1);
   }

   for(int i = 2;i < argc;i++) {
      if(!(strncmp(argv[i], "--subdir", 8))) {
         s++;
         if(s > 1) {
            createidx = true;
            break;
         }
      }
   }

   s = 0;
   snprintf((char*)&subdir, sizeof(subdir), "%s-block001", presentationname);
   for(int i = 2;i < argc;i++) {
      if(!(strncmp(argv[i], "--cols=", 7))) {
         if(count == 0) {
            cols = atol((char*)&argv[i][7]);
         }
      }
      else if(!(strncmp(argv[i], "--width=", 8))) {
         if(count == 0) {
            width = atol((char*)&argv[i][8]);
            if(width < 32) {
               width = 32;
            }
         }
      }
      else if(!(strncmp(argv[i], "--stylesheet=", 13))) {
         if(count == 0) {
            stylesheet = (char*)&argv[i][13];
         }
      }
      else if(!(strncmp(argv[i], "--height=", 9))) {
         if(count == 0) {
            height = atol((char*)&argv[i][9]);
            if(height < 32) {
               height = 32;
            }
         }
      }
      else if(!(strcmp(argv[i], "--index"))) {
         createidx = true;
      }
      else if(!(strcmp(argv[i], "--noindex"))) {
         createidx = false;
      }
      else if(!(strcmp(argv[i], "--enumerate"))) {
         enumerate = true;
      }
      else if( (!(strcmp(argv[i], "--htmlonly"))) ||
               (!(strcmp(argv[i], "--testonly"))) ) {
         if(!htmlonly) {
            cerr << "NOTE: HTML-only mode -> Creating only HTML files!" << endl;
            htmlonly = true;
         }
      }
      else if(!(strncmp(argv[i], "--title=", 8))) {
         title = (char*)&argv[i][8];
      }
      else if(!(strncmp(argv[i], "--maintitle=", 12))) {
         maintitle = (char*)&argv[i][12];
      }
      else if(!(strncmp(argv[i], "--description=", 14))) {
         description = (char*)&argv[i][14];
      }
      else if(!(strncmp(argv[i], "--maindescription=", 18))) {
         maindescription = (char*)&argv[i][18];
      }
      else if(!(strncmp(argv[i], "--subdir", 8))) {
         if(c > 0) {
            html << "</tr>" << endl
                 << "</table>" << endl << endl;
            html.flush();
            c = 0; r = 0; s++;
            snprintf((char*)&subdir, sizeof(subdir), "%s-block%03d", presentationname, s + 1);
            number = 0;
         }
      }
      else if(strncmp(argv[i], "--", 2)) {
         char  numstr[64];
         char* name;
         if(!enumerate) {
            name = rindex(argv[i], '/');
            if(name == NULL) {
               name = argv[i];
            }
            else {
               name = (char*)&name[1];
            }
            snprintf((char*)&webname, sizeof(webname), "%s", name);
            webify((char*)&webname);
         }
         else {
            number++;
            name = (char*)&numstr;
            snprintf(name, sizeof(numstr), "Image #%d", number);
            snprintf(webname, sizeof(webname), "image-%04d", number);
         }
         snprintf((char*)&original, sizeof(original), "%s/original", subdir);
         snprintf((char*)&full, sizeof(full), "%s/full", subdir);
         snprintf((char*)&preview, sizeof(preview), "%s/preview", subdir);
         const int d1 = mkdir(subdir,S_IRWXU|S_IXGRP|S_IRGRP|S_IXOTH|S_IROTH);
         const int d2 = mkdir(original,S_IRWXU|S_IXGRP|S_IRGRP|S_IXOTH|S_IROTH);
         const int d3 = mkdir(full,S_IRWXU|S_IXGRP|S_IRGRP|S_IXOTH|S_IROTH);
         const int d4 = mkdir(preview,S_IRWXU|S_IXGRP|S_IRGRP|S_IXOTH|S_IROTH);
         if((c == 0) && (!htmlonly) &&
            ((d1 != 0) || (d2 != 0) || (d3 != 0) || (d4 != 0)) && (errno != EEXIST)) {
            cerr << "Unable to create directory \"" << subdir << "\"" << endl;
            exit(1);
         }
         snprintf((char*)&original, sizeof(original), "%s/original/%s.jpeg", subdir, webname);
         snprintf((char*)&full, sizeof(full), "%s/full/%s.jpeg", subdir, webname);
         snprintf((char*)&preview, sizeof(preview), "%s/preview/%s.jpeg", subdir, webname);
         snprintf((char*)&fullhtml, sizeof(fullhtml), "%s/show-%s.html", subdir, webname);
         if((createFull(argv[i], full, htmlonly) == false)) {
            cerr << "Creating full image for \"" << argv[i] << "\"" << " failed!" << endl;
            exit(1);
         }
         if((!htmlonly) && (copy(argv[i], original) == false)) {
            cerr << "Copying original image for \"" << argv[i] << "\"" << " failed!" << endl;
            exit(1);
         }
         previewWidth  = width;
         previewHeight = height;
         fullWidth     = 640;
         fullHeight    = 480;
         if(createPreview(argv[i], preview, previewWidth, previewHeight, fullWidth, fullHeight, htmlonly) == true) {
            if((r == 0) && (c == 0) && (s == 0)) {
               if(!sscreated) {
                  if(!createSlideshow(ssmainname, sscontrolname, ssfiles,
                                      ssfilesname, argv[1],
                                      presentationname, "main",
                                      stylesheet, maintitle, pw)) {
                     exit(1);
                  }
                  sscreated = true;
               }
               html << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">" << endl
                    << "<html lang=\"en\">"  << endl
                    << "<head>"  << endl;
               if(stylesheet) {
                  html << "<link rel=\"stylesheet\" href=\"" << stylesheet << "\" type=\"text/css\">" << endl;
               }
               html << "<title>" << maintitle << "</title>" << endl;
               html << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-15\">" << endl;
               if(pw) {
                  html  << "<meta name=\"Author\" content=\"" << pw->pw_gecos << "\">" << endl;
               }
               html << "<meta name=\"Description\" content=\"" << title << "\">" << endl;
               html << "<meta name=\"Keywords\" content=\"Slideshow, " << title;
               if(pw) {
                  html << ", " << pw->pw_gecos;
               }
               html << "\">" << endl;
               html << "<meta name=\"Classification\" content=\"Slideshow\">" << endl;
               html << "</head>" << endl << endl;
               html << "<body>" << endl;
               cat(html, "head.html");
               if(createidx) {
                  html << "<h1>" << maintitle << "</h1>" << endl;
                  if((maindescription) && (maindescription[0] != 0x00)) {
                     html << "<p class=\"description\">" << endl;
                     if(!cat(html, maindescription)) {
                        cerr << "ERROR: Unable to copy main description from file \"" << maindescription << "\"!" << endl;
                        exit(1);
                     }
                     html << "</p>" << endl;
                  }
                  html << "<h2><a name=\"index\"></a>Index</h2>" << endl
                       << "<ul>" << endl
                       << "<li><strong><a href=\"" << ssmainname << "\">View slideshow</a></strong>" << endl;
                  char*        btitle = title;
                  bool         change = false;
                  char         bsubdir[1536];
                  unsigned int bs = 0;
                  strcpy((char*)&bsubdir, subdir);
                  for(int j = 2;j < argc;j++) {
                     if(!(strncmp(argv[j], "--title=", 8))) {
                        btitle = (char*)&argv[j][8];
                     }
                     else if(!(strncmp(argv[j], "--subdir", 8))) {
                        snprintf((char*)&bsubdir, sizeof(bsubdir), "%s-block%03d", presentationname, bs + 1);
                        change = true;
                        bs++;
                     }
                     else if((change) && (strncmp(argv[i], "--", 2))) {
                        html << "   <li><a href=\"#" << bsubdir << "\">"
                             << btitle << "</a>" << endl;
                        change = false;
                     }
                  }
                  html << "</ul>" << endl
                       << "<hr>"  << endl;
               }
               html << endl;
            }
            if((r == 0) && (c == 0)) {
               if(s > 0) {
                  subssfiles.close();
               }
               snprintf((char*)&subssfilesname, sizeof(subssfilesname), "%s-slideshow-files.js", subdir);
               snprintf((char*)&subsscontrolname, sizeof(subsscontrolname), "%s-slideshow-control.html", subdir);
               snprintf((char*)&subssmainname, sizeof(subssmainname), "%s-slideshow.html", subdir);

               html << "<h1><a name=\"" << subdir << "\"></a>" << title << "</h1>" << endl;
               html << "<p class=\"center\">" << endl
                    << "<strong><a href=\"" << subssmainname << "\">View slideshow</a></strong>" << endl
                    << "</p>" << endl;
               if((description) && (description[0] != 0x00)) {
                  html << "<p class=\"description\">" << endl;
                  if(!cat(html, description)) {
                     cerr << "ERROR: Unable to copy description from file \"" << description << "\"!" << endl;
                     exit(1);
                  }
                  html << "</p>" << endl;
                  description = NULL;
               }
               html << "<table class=\"previewtable\">" << endl;

               subssfiles.open(subssfilesname);
               if(!subssfiles.good()) {
                  cerr << "ERROR: Unable to create output file \"" << subssfilesname << "\"!" << endl;
                  exit(1);
               }
               snprintf((char*)&tmp, sizeof(tmp), "%s#%s", argv[1], subdir);
               if(!createSlideshow(subssmainname, subsscontrolname, subssfiles,
                                   subssfilesname, tmp,
                                   presentationname, subdir,
                                   stylesheet, title, pw)) {
                  exit(1);
               }
               c++;
            }
            if(r >= cols) {
               html << "   </tr>" << endl;
               html.flush();
               r = 0;
            }
            if(r == 0) {
               html << "   <tr>" << endl;
               c++;
            }
            html << "      <td class=\"previewtable\">"
                 << "<a href=\"" << fullhtml << "\">"
                 << "<img "
                 << "width=\""  << previewWidth  << "\" "
                 << "height=\"" << previewHeight << "\" "
                 << "alt=\""  << name          << "\" "
                 << "src=\""  << preview       << "\""
                 << ">"
                 << "<br>" << name
                 << "</a>"
                 << "</td>"
                 << endl;
            html.flush();

            ssfiles << "imageArray[images++]=\"" << fullhtml << "\";" << endl;
            subssfiles << "imageArray[images++]=\"" << fullhtml << "\";" << endl;
            ofstream viewhtml(fullhtml);
            if(!viewhtml.good()) {
               cerr << "ERROR: Unable to create file \"" << fullhtml << "\"!" << endl;
               exit(1);
            }
            viewhtml << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">" << endl
                     << "<html lang=\"en\">"  << endl
                     << "<head>"  << endl;
            viewhtml << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-15\">" << endl;
            if(pw) {
               viewhtml << "<meta name=\"Author\" content=\"" << pw->pw_gecos << "\">" << endl;
            }
            viewhtml << "<meta name=\"Description\" content=\"" << title << "\">" << endl;
            viewhtml << "<meta name=\"Keywords\" content=\"Image, " << name << ", " << title;
            if(pw) {
               viewhtml << ", " << pw->pw_gecos;
            }
            viewhtml << "\">" << endl;
            viewhtml << "<meta name=\"Classification\" content=\"Image, " << name << "\">" << endl;
            viewhtml << "<title>Full view of " << name << "</title>" << endl;
            if(stylesheet) {
               viewhtml << "<link rel=\"stylesheet\" href=\"../" << stylesheet << "\" type=\"text/css\">" << endl;
            }
            viewhtml << "<script type=\"text/javascript\" src=\"../imageviewer.js\"></script>" << endl;
            viewhtml << "</head>" << endl << endl;
            viewhtml << "<body>" << endl;
            char* originalRef = index(original, '/');
            if(originalRef) {
               originalRef++;
            }
            else {
               originalRef = "";
            }
            viewhtml << "<script type=\"text/javascript\">" << endl
                     /*
                     << "   w=" << fullWidth << "; h=" << fullHeight << ";" << endl
                     << "   nw = document.body.clientWidth;" << endl
                     << "   nh = document.body.clientHeight;" << endl
                     << "   sw = nw * 0.90 / w;" << endl
                     << "   sh = nh * 0.80 / h;" << endl
                     << "   if(sw <= sh) scale = sw; else scale = sh;" << endl
                     << "   if(scale > 1) scale = 1;" << endl
                     << "   nw=w * scale;" << endl
                     << "   nh=h * scale;" << endl
                     << "   document.write( \"<object width=\" + nw + \" height=\" + nh + \" type=\\\"image/pjpeg\\\" data=\\\"" << (char*)&full[strlen(subdir) + 1] << "\\\" alt=\\\"" << name << "\\\">\" );" << endl
                     << "   document.write( \"   <object width=\" + nw + \" height=\" + nh + \" data=\\\"" << (char*)&original[strlen(subdir) + 1] << "\\\" alt=\\\"" << name << "\\\">\" );" << endl
                     << "   document.write( \"      <strong>Your browser has been unable to load or display the image!</strong>\" );" << endl
                     << "   document.write( \"   </object>\" );" << endl
                     << "   document.write( \"</object>\" );" << endl
                     */
                     << "<!--" << endl
                     << "   show( \"" << (char*)&full[strlen(subdir) + 1] << "\", \"" << (char*)&original[strlen(subdir) + 1] << "\", " << fullWidth << ", " << fullHeight << ");" << endl
                     << "-->" << endl
                     << "</script>" << endl;

            viewhtml << "<noscript>" << endl
                     << "   <p class=\"center\">" << endl
                     << "   <object width=\"90%\" type=\"image/pjpeg\" data=\"" << (char*)&full[strlen(subdir) + 1] << "\">" << endl
                     << "      <object width=\"90%\" type=\"image/jpeg\" data=\"" << (char*)&original[strlen(subdir) + 1] << "\">" << endl
                     << "         <strong>Your browser has been unable to load or display image!</strong>" << endl
                     << "      </object>" << endl
                     << "   </object>" << endl
                     << "   </p>" << endl
                     << "</noscript>" << endl;

            viewhtml << "<p class=\"center\">" << endl
                     << "<br>Full view of <em>" << name << "</em>" << endl
                     << "<br><a href=\"" << originalRef << "\">Get the original file</a>" << endl
                     << "</p>" << endl;
            cat(viewhtml, "tail.html");
            viewhtml << "<p class=\"description\">" << endl
                     << "<a href=\"http://validator.w3.org/check/referer\">" << endl
                     << "<object type=\"image/png\" data=\"../valid-html401.png\" height=\"31\" width=\"88\">" << endl
                     << "   <object data=\"http://www.w3.org/Icons/valid-html401\" height=\"31\" width=\"88\">" << endl
                     << "      Valid HTML 4.01!" << endl
                     << "   </object>" << endl
                     << "</object>" << endl
                     << "</a>" << endl
                     << "</p>" << endl;
            viewhtml << "</body>" << endl;
            viewhtml << "</html>" << endl;
            r++;
         }
         else {
            cerr << "Creating preview image for \"" << argv[i] << "\"" << " failed!" << endl;
            exit(1);
         }
      }
   }

   if(c > 0) {
      html << "   </tr>" << endl
           << "</table>" << endl << endl;
   }
   cat(html, "tail.html");
   html << "<p class=\"description\">" << endl
        << "<a href=\"http://validator.w3.org/check/referer\">" << endl
        << "<object type=\"image/png\" data=\"valid-html401.png\" height=\"31\" width=\"88\">" << endl
        << "   <object data=\"http://www.w3.org/Icons/valid-html401\" height=\"31\" width=\"88\">" << endl
        << "      Valid HTML 4.01!" << endl
        << "   </object>" << endl
        << "</object>" << endl
        << "</a>" << endl
        << "</p>" << endl;
   html << "</body>" << endl;
   html << "</html>" << endl;

   cout << endl << "Slideshow generation successfully completed!" << endl;
}

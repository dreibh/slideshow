/*
 *  $Id: getsize.cc,v 1.1 2003/07/17 09:45:09 dreibh Exp $
 *
 * Get size of a PNM file.
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
#include <iostream>


using namespace std;


// ###### Main program #######################################################
int main(int argc, char** argv)
{
   for(int i = 1;i < argc;i++) {
      FILE* in = fopen(argv[i], "r");
      if(in == NULL) {
         cerr << "ERROR: Unable to open file \"" << argv[i] << "\"!" << endl;
         exit(1);
      }

      unsigned int dummy, w, h;
      if(fscanf(in, "P%d\n%u %u\n", &dummy, &w, &h) != 3) {
         cerr << "ERROR: Unable to get width/height from file \"" << argv[i] << "\"!" << endl;
         exit(1);
      }

      cout << "W=" << w << " " << "H=" << h << endl;

      fclose(in);
   }
}

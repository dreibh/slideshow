/* --------------------------------------------------------------------------
 * - XHTML 1.1 image presentation and JavaScript-based slideshow generator  -
 * --------------------------------------------------------------------------
 *
 * Copyright (C) 2003-2024 by Thomas Dreibholz
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


// ###### Window has been resized -> Update page from cache ##################
function resizeEvent()
{
   history.go(0);
}


// ###### Show image #########################################################
function show(fullname, w, h) {
   window.onresize = resizeEvent;

   nw = document.documentElement.clientWidth;
   nh = document.documentElement.clientHeight;

   // document.write("w=" + w + " h=" + h + "<br />");
   // document.write("nw=" + nw + " nh=" + nh + "<br />");

   if((nw > 0) && (nh > 0)) {
      sw = nw * 0.75 / w;
      sh = nh * 0.65 / h;
      if(sw <= sh) scale = sw; else scale = sh;
      if(scale > 1) scale = 1;
      nw=w * scale;
      nh=h * scale;
   }
   else {
      nw = w;
      nh = h;
   }
   document.write( "<p class=\"center\">" );
   document.write( "<img width=\"" + nw + "\" height=\"" + nh + "\" alt=\"Image " + fullname + "\" src=\"" + fullname + "\" />" );
   document.write( "</p>" );
}

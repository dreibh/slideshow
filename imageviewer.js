/*
 *  $Id: imageviewer.js,v 1.7 2004/07/01 08:31:07 dreibh Exp $
 *
 * JavaScript Image Viewer
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

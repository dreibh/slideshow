/*
 *  $Id: imageviewer.js,v 1.1 2003/07/17 09:45:09 dreibh Exp $
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
function show(fullname, originalname, w, h) {
   window.onresize = resizeEvent;

   nw = document.body.clientWidth;
   nh = document.body.clientHeight;
   sw = nw * 0.90 / w;
   sh = nh * 0.80 / h;
   if(sw <= sh) scale = sw; else scale = sh;
   if(scale > 1) scale = 1;
   nw=w * scale;
   nh=h * scale;
   document.write( "<p class=\"center\">" );
   document.write( "<object width=" + nw + " height=" + nh + " type=\"image/pjpeg\" data=\"" + fullname + "\">" );
   document.write( "   <object width=" + nw + " height=" + nh + " data=\"" + originalname + "\">" );
   document.write( "      <strong>Your browser has been unable to load or display image!</strong>" );
   document.write( "   </object>" );
   document.write( "</object>" );
   document.write( "</p>" );
}

/*
 *  $Id: slideshow.js,v 1.1 2003/07/17 09:45:09 dreibh Exp $
 *
 * JavaScript Slideshow
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


images      = 0;
current     = -1;
imageArray  = new Array();
pause       = false;
showTimer   = 0;
changeDelay = 30;


// To use slideshow, "imageArray" has to be filled with URLs to display and
// "images" has to be set to the amount of its entries!
// mainPage has to be set to the URL of the page for the "Up" button.


// ##### Get previous URL ####################################################
function getPrev() {
   inputObjects = document.getElementsByTagName("INPUT");
   if(inputObjects[1].checked) {
      current = Math.floor(Math.random() * images);
   }
   else {
      current--;
      if(current < 0) {
         current = images - 1;
      }
   }
   return(imageArray[current]);
}


// ##### Get next URL ########################################################
function getNext() {
   inputObjects = document.getElementsByTagName("INPUT");
   if(inputObjects[1].checked) {
      current = Math.floor(Math.random() * images);
   }
   else {
      current++;
      if(current >= images) {
         current = 0;
      }
   }
   return(imageArray[current]);
}


// ##### Show current URL ####################################################
function show() {
   clearTimeout(showTimer);
   try {
      inputObjects = document.getElementsByTagName("INPUT");
      inputObjects[2].value = current;
   } catch(e) { }
   if(current < images) {
      top.frames[1].location = imageArray[current];
   }
   else {
      alert("slideshow.js:\nThere are no images to display?!");
   }
   if(!pause) {
      showTimer = setTimeout("next()", 1000 * changeDelay);
   }
}


// ##### Handle play/pause button  ###########################################
function playpause() {
   clearTimeout(showTimer);
   if(pause) {
      pause = false;
      document["play"].src="controls/player_play.png";
      getNext();
      show();
   }
   else {
      pause = true;
      document["play"].src="controls/player_pause.png";
   }
}


// ##### Handle prev button  #################################################
function prev() {
   document["goprev"].style.backgroundColor="#aaaaaa";
   getPrev();
   show();
   document["goprev"].src="controls/1leftarrow.png";
}


// ##### Handle next button  #################################################
function next() {
   getNext();
   show();
}


// ##### Handle up button  ###################################################
function up() {
   top.location = mainPage;
}


// ###### Handle changes of the delay ########################################
function newChangeDelay() {
   inputObjects = document.getElementsByTagName("INPUT");
   input  = inputObjects[0];

   try {
      value = new Number(input.value);
   }
   catch(e) {
      value = 60;
   }

   if((value >= 1) && (value <= 999)) {
   }
   else if(value < 1) {
      value = 1;
   }
   else if(value > 999) {
      value = 999;
   }
   else {
      value = 88;
   }
   input.value = value;

   changeDelay = value;
   if(!pause) {
      clearTimeout(showTimer);
      showTimer = setTimeout("next()", 1000 * changeDelay);
   }
}


// ######## Mouse over/out effect ############################################
function goupMouseOver(name) {
   document["goup"].style.backgroundColor="#aaaaaa";
}


// ######## Mouse over/out effect ############################################
function goupMouseOut(name) {
   document["goup"].style.backgroundColor="";
}


// ######## Mouse over/out effect ############################################
function goprevMouseOver(name) {
   document["goprev"].style.backgroundColor="#aaaaaa";
}


// ######## Mouse over/out effect ############################################
function goprevMouseOut(name) {
   document["goprev"].style.backgroundColor="";
}


// ######## Mouse over/out effect ############################################
function gonextMouseOver(name) {
   document["gonext"].style.backgroundColor="#aaaaaa";
}


// ######## Mouse over/out effect ############################################
function gonextMouseOut(name) {
   document["gonext"].style.backgroundColor="";
}


// ######## Mouse over/out effect ############################################
function playpauseMouseOver(name) {
   document["play"].style.backgroundColor="#ccccee";
}


// ######## Mouse over/out effect ############################################
function playpauseMouseOut(name) {
   document["play"].style.backgroundColor="";
}


// ######## Initialization ###################################################
function onLoad() {
   document["goup"].onmouseover=goupMouseOver;
   document["goup"].onmouseout=goupMouseOut;
   document["goprev"].onmouseover=goprevMouseOver;
   document["goprev"].onmouseout=goprevMouseOut;
   document["gonext"].onmouseover=gonextMouseOver;
   document["gonext"].onmouseout=gonextMouseOut;
   document["play"].onmouseover=playpauseMouseOver;
   document["play"].onmouseout=playpauseMouseOut;
   next();
}

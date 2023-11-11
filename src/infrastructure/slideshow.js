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


var images      = 0;
var current     = -1;
var imageArray  = new Array();
var pause       = false;
var showTimer   = 0;



// To use slideshow, "imageArray" has to be filled with URLs to display and
// "images" has to be set to the amount of its entries!
// mainPage has to be set to the URL of the page for the "Up" button.
// presentationName has to be set to this presentation's name (cookie-string capable!)
var mainPage         = "Unknown"
var presentationName = "Unknown"
var changeDelay      = 999;
var randomMode       = 0;



// ##### Get cookie #########################################################
function getCookie (name) {
   myentry = name + "=";
   i = 0;
   while(i < document.cookie.length) {
      j = i + myentry.length;
      if(document.cookie.substring(i, j) == myentry) {
         end = document.cookie.indexOf(";", j);
         if(end == -1) {
            end = document.cookie.length;
         }
         return(unescape(document.cookie.substring(j, end)));
      }
      i = document.cookie.indexOf(" ", i) + 1;
      if(i == 0) {
         break;
      }
   }
   return(null);
}


// ##### Set cookie #########################################################
function setCookie (name, value, expires) {
   document.cookie = name + "=" + escape(value) + "; " +
                     "expires=" + expires.toGMTString();
}


// ##### Get random but different image #####################################
function chooseRandomImage() {
   old = current
   if(images > 1) {   // Otherwise, there is not much to select ...
      while(current == old) {
         current = Math.floor(Math.random() * images);
      }
   }
   return(current);
}


// ##### Get previous URL ###################################################
function getPrev() {
   if(randomMode == 1) {
      current = chooseRandomImage();
   }
   else {
      current--;
      if(current < 0) {
         current = images - 1;
      }
   }
   return(imageArray[current]);
}


// ##### Get next URL #######################################################
function getNext() {
   if(randomMode == 1) {
      current = chooseRandomImage();
   }
   else {
      current++;
      if(current >= images) {
         current = 0;
      }
   }
   return(imageArray[current]);
}


// ##### Show current URL ###################################################
function show() {
   clearTimeout(showTimer);
   try {
      inputObjects = document.getElementsByTagName("INPUT");
      inputObjects[1].value = current;
   } catch(e) { }
   if(current >= images) {
      current = 0;
   }
   if(current < images) {
      top.frames[1].location = imageArray[current];
   }
   else {
      alert("slideshow.js:\nThere are no images to display?!");
   }
   if(!pause) {
      showTimer = setTimeout("next()", 1000 * changeDelay);
   }
   inputObjects = document.getElementsByTagName("INPUT");
   expires = new Date();
   expires.setDate(expires.getTime() + 7 * 24 * 60 * 60 * 1000);
   setCookie("CurrentImageNumber-" + presentationName, current, expires);
}


// ##### Handle play/pause button  ##########################################
function playpause() {
   clearTimeout(showTimer);
   if(pause) {
      pause = false;
      document["play"].src="infrastructure/player_play.png";
      document["play"].alt="Play";
      getNext();
      show();
   }
   else {
      pause = true;
      document["play"].src="infrastructure/player_pause.png";
      document["play"].alt="Pause";
   }
}


// ##### Handle prev button  ################################################
function prev() {
   document["goprev"].style.backgroundColor="#aaaaaa";
   getPrev();
   show();
   document["goprev"].src="infrastructure/1leftarrow.png";
}


// ##### Handle next button  ################################################
function next() {
   getNext();
   show();
}


// ##### Handle up button  ##################################################
function up() {
   top.location = mainPage;
}


// ###### Toggle random mode ################################################
function toggleRandomMode() {
   if(randomMode == 1) {
      randomMode = 0;
   }
   else {
      randomMode = 1;
   }
   if(randomMode == 1) {
      document["randommode"].style.backgroundColor="#aaaaaa";
      document["randommode"].alt = "Random Mode ON";
   }
   else {
      document["randommode"].style.backgroundColor="";
      document["randommode"].alt = "Random Mode OFF";
   }
   expires.setDate(expires.getTime() + 7 * 24 * 60 * 60 * 1000);
   setCookie("RandomImageView-" + presentationName, randomMode, expires);
}


// ###### Handle changes of the delay #######################################
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

   expires.setDate(expires.getTime() + 7 * 24 * 60 * 60 * 1000);
   setCookie("CurrentChangeDelay-" + presentationName, changeDelay, expires);
}


// ######## Mouse over/out effect ###########################################
function goupMouseOver(name) {
   document["goup"].style.backgroundColor="#aaaaaa";
}


// ######## Mouse over/out effect ###########################################
function goupMouseOut(name) {
   document["goup"].style.backgroundColor="";
}


// ######## Mouse over/out effect ###########################################
function goprevMouseOver(name) {
   document["goprev"].style.backgroundColor="#aaaaaa";
}


// ######## Mouse over/out effect ###########################################
function goprevMouseOut(name) {
   document["goprev"].style.backgroundColor="";
}


// ######## Mouse over/out effect ###########################################
function gonextMouseOver(name) {
   document["gonext"].style.backgroundColor="#aaaaaa";
}


// ######## Mouse over/out effect ###########################################
function gonextMouseOut(name) {
   document["gonext"].style.backgroundColor="";
}


// ######## Mouse over/out effect ###########################################
function playpauseMouseOver(name) {
   document["play"].style.backgroundColor="#ccccee";
}


// ######## Mouse over/out effect ###########################################
function playpauseMouseOut(name) {
   document["play"].style.backgroundColor="";
}


// ######## Initialization ##################################################
function onLoad() {
   document["goup"].onmouseover=goupMouseOver;
   document["goup"].onmouseout=goupMouseOut;
   document["goprev"].onmouseover=goprevMouseOver;
   document["goprev"].onmouseout=goprevMouseOut;
   document["gonext"].onmouseover=gonextMouseOver;
   document["gonext"].onmouseout=gonextMouseOut;
   document["play"].onmouseover=playpauseMouseOver;
   document["play"].onmouseout=playpauseMouseOut;
   document["play"].alt="Play";

   inputObjects = document.getElementsByTagName("INPUT");
   changeDelay = getCookie("CurrentChangeDelay-" + presentationName);
   if(changeDelay == null) {
      changeDelay = 30;
   }
   inputObjects[0].value = changeDelay;

   randomMode = getCookie("RandomImageView-" + presentationName);
   if(randomMode == 1) {
      document["randommode"].style.backgroundColor="#aaaaaa";
      document["randommode"].alt = "Random Mode ON";
   }
   else {
      document["randommode"].style.backgroundColor="";
      document["randommode"].alt = "Random Mode OFF";
   }

   current = getCookie("CurrentImageNumber-" + presentationName);
   if(current == null) {
      next();
   }
   show();
}

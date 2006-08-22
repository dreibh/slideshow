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
 * $Id$
 */

#ifndef CONVERTER_H
#define CONVERTER_H


int imageConverter(const char*   originalImageName,
                   const char*   previewImageName,
                   const char*   fullsizeImageName,
                   unsigned int& originalWidth,
                   unsigned int& originalHeight,
                   unsigned int& previewWidth,
                   unsigned int& previewHeight,
                   unsigned int  previewQuality,
                   unsigned int& fullsizeWidth,
                   unsigned int& fullsizeHeight,
                   unsigned int  fullsizeQuality);

int imageTester(const char*   originalImageName,
                const char*   previewImageName,
                const char*   fullsizeImageName,
                unsigned int& originalWidth,
                unsigned int& originalHeight,
                unsigned int& previewWidth,
                unsigned int& previewHeight,
                unsigned int& fullsizeWidth,
                unsigned int& fullsizeHeight);


#endif

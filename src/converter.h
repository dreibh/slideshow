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

#ifndef CONVERTER_H
#define CONVERTER_H


int imageConverter(const char* originalImageName,
                   const char* previewImageName,
                   const char* fullsizeImageName,
                   size_t&     originalWidth,
                   size_t&     originalHeight,
                   size_t&     previewWidth,
                   size_t&     previewHeight,
                   size_t      previewQuality,
                   size_t&     fullsizeWidth,
                   size_t&     fullsizeHeight,
                   size_t      fullsizeQuality);

int imageTester(const char* originalImageName,
                const char* previewImageName,
                const char* fullsizeImageName,
                size_t&     originalWidth,
                size_t&     originalHeight,
                size_t&     previewWidth,
                size_t&     previewHeight,
                size_t&     fullsizeWidth,
                size_t&     fullsizeHeight);


#endif

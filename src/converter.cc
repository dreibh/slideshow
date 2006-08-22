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

#include "tools.h"
#include <magick/api.h>


#define MIN(a,b) ((a < b) ? (a) : (b))
#define MAX(a,b) ((a > b) ? (a) : (b))

// ###### Calculate scaled image size #######################################
void scaleImageSize(size_t  originalWidth,
                    size_t  originalHeight,
                    size_t& scaledWidth,
                    size_t& scaledHeight)
{
   const double ratio = (double)originalWidth / (double)originalHeight;

   scaledWidth   = MIN(originalWidth, scaledWidth);
   scaledHeight  = MIN(originalHeight, scaledHeight);

   size_t width  = scaledWidth;
   size_t height = (size_t)rint(scaledWidth / ratio);
   if(height > scaledHeight) {
      width  = (size_t)rint(scaledHeight * ratio);
      height = scaledHeight;
   }

   const double ratioNew = (double)width / (double)height;
   if(fabs(ratio - ratioNew) > 0.1) {
      std::cerr << "INTERNAL ERROR: Scaling calculation failed!" << std::endl;
      std::cerr << "ow=" << originalWidth << std::endl
                << "oh=" << originalHeight << std::endl
                << "sw=" << scaledWidth << std::endl
                << "sw=" << scaledHeight << std::endl
                << "r1=" << ratio << std::endl
                << "r2=" << ratioNew << std::endl;
      exit(1);
   }

   scaledWidth  = width;
   scaledHeight = height;
   // printf("w=%d h=%d\n", scaledWidth, scaledHeight);
}


// ###### Convert image to fullsize and preview versions ####################
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
                   unsigned int  fullsizeQuality)
{
   ExceptionInfo exception;
   ImageInfo*    originalImageInfo;
   ImageInfo*    previewImageInfo;
   ImageInfo*    fullsizeImageInfo;
   Image*        originalImage;
   Image*        previewImage;
   Image*        fullsizeImage;

   InitializeMagick(NULL);
   GetExceptionInfo(&exception);


   // ====== Read original image ============================================
   originalImageInfo = CloneImageInfo((ImageInfo*)NULL);
   snprintf((char*)&originalImageInfo->filename,
            sizeof(originalImageInfo->filename),
            "%s",
            originalImageName);
   originalImage = ReadImage(originalImageInfo, &exception);
   if(exception.severity != UndefinedException) {
      CatchException(&exception);
   }
   if(originalImage == NULL) {
      DestroyImageInfo(originalImageInfo);
      return(0);
   }
   originalWidth  = originalImage->columns;
   originalHeight = originalImage->rows;


   // ====== Calculate scaled sizes =========================================
   scaleImageSize(originalWidth, originalHeight, previewWidth, previewHeight);
   scaleImageSize(originalWidth, originalHeight, fullsizeWidth, fullsizeHeight);


   // ====== Create fullsize image ==========================================
   fullsizeImage = ResizeImage(originalImage, fullsizeWidth, fullsizeHeight, LanczosFilter, 1.0, &exception);
   if(fullsizeImage == NULL) {
      DestroyImage(originalImage);
      DestroyImageInfo(originalImageInfo);
      return(0);
   }
   fullsizeImageInfo = CloneImageInfo(originalImageInfo);
   if(fullsizeImageInfo == NULL) {
      DestroyImage(originalImage);
      DestroyImageInfo(originalImageInfo);
      DestroyImage(fullsizeImage);
      return(0);
   }

   safestrcpy(fullsizeImage->filename, fullsizeImageName, sizeof(fullsizeImage->filename));
   fullsizeImageInfo->interlace = LineInterlace;
   fullsizeImageInfo->quality   = fullsizeQuality;
   WriteImage(fullsizeImageInfo, fullsizeImage);

   DestroyImage(fullsizeImage);
   DestroyImageInfo(fullsizeImageInfo);


   // ====== Create preview image ===========================================
   previewImage = ResizeImage(originalImage, previewWidth, previewHeight, LanczosFilter, 1.0, &exception);
   if(previewImage == NULL) {
      DestroyImage(originalImage);
      DestroyImageInfo(originalImageInfo);
      return(0);
   }
   previewImageInfo = CloneImageInfo(originalImageInfo);
   if(previewImageInfo == NULL) {
      DestroyImage(originalImage);
      DestroyImageInfo(originalImageInfo);
      DestroyImage(previewImage);
      return(0);
   }

   safestrcpy(previewImage->filename, previewImageName, sizeof(previewImage->filename));
   previewImageInfo->interlace = LineInterlace;
   previewImageInfo->quality   = previewQuality;
   WriteImage(previewImageInfo, previewImage);

   DestroyImage(previewImage);
   DestroyImageInfo(previewImageInfo);


   // ====== Create fullsize image ==========================================
   DestroyImage(originalImage);
   DestroyImageInfo(originalImageInfo);
   DestroyMagick();
   return(1);
}


// ###### Test original, fullsize and preview images ########################
int imageTester(const char*   originalImageName,
                const char*   previewImageName,
                const char*   fullsizeImageName,
                unsigned int& originalWidth,
                unsigned int& originalHeight,
                unsigned int& previewWidth,
                unsigned int& previewHeight,
                unsigned int& fullsizeWidth,
                unsigned int& fullsizeHeight)
{
   ExceptionInfo exception;
   ImageInfo*    originalImageInfo;
   ImageInfo*    previewImageInfo;
   ImageInfo*    fullsizeImageInfo;
   Image*        originalImage;
   Image*        previewImage;
   Image*        fullsizeImage;

   InitializeMagick(NULL);
   GetExceptionInfo(&exception);


   // ====== Read original image ============================================
   originalImageInfo = CloneImageInfo((ImageInfo*)NULL);
   snprintf((char*)&originalImageInfo->filename,
            sizeof(originalImageInfo->filename),
            "%s",
            originalImageName);
   originalImage = PingImage(originalImageInfo, &exception);
   if(exception.severity != UndefinedException) {
      CatchException(&exception);
   }
   if(originalImage != NULL) {
      originalWidth  = originalImage->columns;
      originalHeight = originalImage->rows;
      DestroyImage(originalImage);
      originalImage = NULL;
   }


   // ====== Read preview image =============================================
   previewImageInfo = CloneImageInfo((ImageInfo*)NULL);
   snprintf((char*)&previewImageInfo->filename,
            sizeof(previewImageInfo->filename),
            "%s",
            previewImageName);
   previewImage = PingImage(previewImageInfo, &exception);
   if(exception.severity != UndefinedException) {
      CatchException(&exception);
   }
   if(previewImage != NULL) {
      previewWidth  = previewImage->columns;
      previewHeight = previewImage->rows;
      DestroyImage(previewImage);
      previewImage = NULL;
   }


   // ====== Read fullsize image ============================================
   fullsizeImageInfo = CloneImageInfo((ImageInfo*)NULL);
   snprintf((char*)&fullsizeImageInfo->filename,
            sizeof(fullsizeImageInfo->filename),
            "%s",
            fullsizeImageName);
   fullsizeImage = PingImage(fullsizeImageInfo, &exception);
   if(exception.severity != UndefinedException) {
      CatchException(&exception);
   }
   if(fullsizeImage != NULL) {
      fullsizeWidth  = fullsizeImage->columns;
      fullsizeHeight = fullsizeImage->rows;
      DestroyImage(fullsizeImage);
      fullsizeImage = NULL;
   }


   DestroyMagick();
   return(1);
}

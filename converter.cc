#include "tools.h"
#include <magick/api.h>


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


   // ====== Create fullsize image ==========================================
   fullsizeImage = SampleImage(originalImage, fullsizeWidth, fullsizeHeight, &exception);
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
   previewImage = SampleImage(originalImage, previewWidth, previewHeight, &exception);
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

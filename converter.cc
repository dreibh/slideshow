#include <stdio.h>
#include <stdlib.h>
#include <magick/api.h>


int imageConverter(const char*   originalImageName,
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
      return(0);
   }
   originalWidth  = originalImage->columns;
   originalHeight = originalImage->rows;
printf("W=%u H=%u\n",originalWidth,originalHeight);

/*
   // ====== Create preview image ===========================================
   previewImage = CloneImage(originalImage);
   if(previewImage == NULL) {
      return(0);
   }




   // ====== Create fullsize image ==========================================
         preview.interlaceType(Magick::LineInterlace);
         preview.quality(OwnerBlock->OwnerPresentation->PreviewQuality);
         preview.sample(Magick::Geometry(OwnerBlock->OwnerPresentation->PreviewWidth, OwnerBlock->OwnerPresentation->PreviewHeight));
         PreviewWidth  = preview.size().width();
         PreviewHeight = preview.size().height();



   DestroyImage(originalImage);
   */


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

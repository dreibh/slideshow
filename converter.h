#ifndef CONVERTER_H
#define CONVERTER_H


int imageConverter(const char*   originalImageName,
                   const char*   previewImageName,
                   const char*   fullsizeImageName,
                   unsigned int& originalWidth,
                   unsigned int& originalHeight,
                   unsigned int& previewWidth,
                   unsigned int& previewHeight,
                   unsigned int& fullsizeWidth,
                   unsigned int& fullsizeHeight);

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

#ifndef DELTA_SOARX_FRAMEWORK
#define DELTA_SOARX_FRAMEWORK

#include "soarx_framework.h"

namespace dtSOARX
{
   //______________________________________________________________
   class Image :
   public IImage
   {
   private:
	   IImage::Dimensions m_dimensions;
	   int m_data_size;
	   char* m_image;
   public:
	   Image();
	   ~Image();
	   int Create(IImage::Dimensions d);
	   int ProcessImage(ImageProc fp, void* ptr);
	   int ProcessLine(int line, ImageProc fp, void* ptr);
	   int CreateGradient(IImage* src_image, float scale);
	   int Load(const char* path);
	   int LoadInfo(const char* path);
	   int Save(const char* path);
	   Dimensions GetDimensions();
	   char* GetData();
	   int GetPixel(int c, int x, int y = 0, int z = 0);
	   int SetPixel(int v, int c, int x, int y = 0, int z = 0);

   private:
	   int Load(const char* path, bool info);

	   int Load_BMP(FILE* f, bool info);
	   int Load_TGA(FILE* f, bool info);
	   int Load_PNG(FILE* f, bool info);
	   int Load_JPEG(FILE* f, bool info);

	   int Save_BMP(FILE* f);
	   int Save_TGA(FILE* f);
	   int Save_PNG(FILE* f);
	   int Save_JPEG(FILE* f);
   };
};

#endif // DELTA_SOARX_FRAMEWORK

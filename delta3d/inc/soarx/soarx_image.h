/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_SOARX_IMAGE
#define DELTA_SOARX_IMAGE

#include "soarx/soarx_framework.h"

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

#endif // DELTA_SOARX_IMAGE

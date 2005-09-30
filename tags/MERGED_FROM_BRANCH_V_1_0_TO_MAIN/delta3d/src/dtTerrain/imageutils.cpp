/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Teague Coonan
*/

#include <osgDB/ImageOptions>
#include <osg/Vec3>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include "dtTerrain/imageutils.h"
#include "dtTerrain/mathutils.h"
#include "dtTerrain/fixedpointnoise.h"

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   unsigned short ImageUtils::CalculateDetailNoise(int x, int y)
   {
      static int a[] = { 0, 0, 0, 0, 2048, 1024, 512, 256, 96, 58 };
      FixedPointNoise noise;
      
      int d = 0;
      int tx = x << 2;
      int ty = y << 2;
      
      for (int o=4; o<10; o++) 
      {
         int c = 1024 << (12-(10-o));
         int dx = x << (12-(10-o));
         int dy = y << (12-(10-o));
         int p = noise(dx,dy);
         int px = noise(dx-c,dy);
         int py = noise(dx,dy-c);
         int pxy = noise(dx-c,dy-c);
      
         d += MathUtils::IMul(MathUtils::ILerp(ty, 
               MathUtils::ILerp(tx,p,px),MathUtils::ILerp(tx,py,pxy)),a[o]);
         d = osg::clampTo(d,-4095,4095);
      }
      
      d <<= 3;
      d += 32767;
      return static_cast<unsigned short>(d);
   }

   //////////////////////////////////////////////////////////////////////////   
   dtCore::RefPtr<osg::Image> ImageUtils::CreateDetailMap(unsigned int width, 
      unsigned int height)
   {
      dtCore::RefPtr<osg::Image> image = new osg::Image();      
      image->allocateImage(width,height,1,GL_LUMINANCE,GL_UNSIGNED_SHORT);
      unsigned short *data = (unsigned short *)image->data();
      
      for (unsigned int i=0; i<height; i++)
         for (unsigned int j=0; j<width; j++)
            *(data++) = CalculateDetailNoise(j,i);
      
      return image;
   }
   
   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::CreateBaseGradientMap(
      const osg::Image *srcImage, float scale)
   {
      if (srcImage == NULL)
         return NULL;
         
      //Make sure the source image is a 16-bit single channel 
      //image. 

      dtCore::RefPtr<osg::Image> image = new osg::Image();                  
      unsigned int width = srcImage->s();
      unsigned int height = srcImage->t();        

      image->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);
      unsigned short *srcData = (unsigned short *)srcImage->data();
      unsigned char *dstData = image->data();
      
      for (unsigned int i=0; i<height; i++)
      {
         for (unsigned int j=0; j<width; j++)
         {
            float o = (float)srcData[(i*width)+j];
            float h,v;
           
            h = (float)srcData[(i*width) + ((j+1)%width)];
            v = (float)srcData[(((i+1)%height) * width) + j];
            
            h = ((((h-o) * scale) + 32768.0f) / 65536.0f) * 255.0f;
            v = ((((v-o) * scale) + 32768.0f) / 65536.0f) * 255.0f;           
                 
            *(dstData)++ = 0;
            *(dstData)++ = (unsigned char)osg::clampTo(h,0.0f,255.0f);
            *(dstData)++ = (unsigned char)osg::clampTo(v,0.0f,255.0f);
         }
      }
      
      return image;
   }
   
   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::CreateDetailGradientMap(unsigned int width,
         unsigned int height, float scale)
   {
      if (width == 0 || height == 0)
      {
         LOG_ERROR("Cannot make detail gradient map.  An invalid width or height "
            "was specified.");
         return NULL;
      }
      
      dtCore::RefPtr<osg::Image> image = new osg::Image();
      image->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);
      unsigned char *imageData = image->data();
      
      for (unsigned int i=0; i<height; i++)
      {
         for (unsigned int j=0; j<width; j++)
         {
            unsigned short h = CalculateDetailNoise(j,i);
            float gx,gy;
            
            gx = (float)CalculateDetailNoise(j+1,i);
            gy = (float)CalculateDetailNoise(j,i+1);
            
            gx = ((((gx-h) * scale) + 32768.0f) / 65536.0f) * 255.0f;
            gy = ((((gy-h) * scale) + 32768.0f) / 65536.0f) * 255.0f;      
            
            *(imageData)++ = 0;
            *(imageData)++ = (unsigned char)osg::clampTo(gx,0.0f,255.0f);
            *(imageData)++ = (unsigned char)osg::clampTo(gy,0.0f,255.0f);
         }
      }
            
      return image;
   }
   
   //////////////////////////////////////////////////////////////////////////
   unsigned short ImageUtils::CalculateScaleMapNoise(int x, int y)
   {
      static int a[] = { 64, 128, 1024, 1024, 2048, 1024, 512, 5000, 4000, 4000 };
      FixedPointNoise noise;

      int d = 0;
      int tx = x<<2;
      int ty = y<<2;

      for (int o=8; o<10; o++) 
      {
         int c = 1024 << (12-(10-o));
         int dx = x << (12-(10-o));
         int dy = y << (12-(10-o));
         int p = noise(dx,dy);
         int px = noise(dx-c,dy);
         int py = noise(dx,dy-c);
         int pxy = noise(dx-c,dy-c);
         d += MathUtils::IMul(MathUtils::ILerp(ty, 
            MathUtils::ILerp(tx,p,px), MathUtils::ILerp(tx,py,pxy)), a[o]);
         d = osg::clampTo(d,-4095,4095);
      }

      d <<= 3;
      d += 40000;
      d = osg::clampTo(d,0,65535);
      
      return static_cast<unsigned short>(d);
   }
   
   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::CreateDetailScaleMap(unsigned int width,
         unsigned int height)
   {
      if (width == 0 || height == 0)
      {
         LOG_ERROR("Cannot create scale map.  Invalid width or height was "
          "specified.");
         return NULL;
      }
      
      dtCore::RefPtr<osg::Image> image = new osg::Image();
      image->allocateImage(width,height,1,GL_LUMINANCE,GL_UNSIGNED_SHORT);
      unsigned short *imgData = (unsigned short *)image->data();
      for (unsigned int i=0; i<height; i++)
         for (unsigned int j=0; j<width; j++)
            *(imgData++) = CalculateScaleMapNoise(j,i);
            
      return image;
   }
   
   
   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::MakeHeightMapImage(const osg::HeightField* hf)
   {      
      unsigned int width = hf->getNumColumns();
      unsigned int height = hf->getNumRows();
      
      if (width == 0 || height == 0)
      {
         LOG_ERROR("Could not convert height field to an image.  The heightfield "
            " has invalid dimensions.");
         return NULL;
      }
        
      //Create an image big enough to hold the heightfield data.
      dtCore::RefPtr<osg::Image> image = new osg::Image();
      image->allocateImage(width,height,1,GL_LUMINANCE,GL_UNSIGNED_SHORT);
      unsigned int i,j;
      
      //Before we can copy the image data, we need to determine a 
      //min and max height value so we can scale accordingly.
      float minValue = 1e10;//MAXFLOAT;
      float maxValue = -1e10;//MAXFLOAT;
      for (j=0; j<height; j++)
      {
         for (i=0; i<width; i++)
         {
            float value = hf->getHeight(i,j);
            if (value < minValue)
               minValue = value;
            if (value > maxValue)
               maxValue = value;
         }
      }
      
      //Copy the height data to the image.
      unsigned short *data = (unsigned short *)image->data();
      for (j=0; j<height; j++) 
         for (i=0; i<width; i++)
            *(data++) = (unsigned short)(((hf->getHeight(i,j) - minValue) / maxValue) 
               * 65536.0f);
     
      return image;
   }
   
   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 ImageUtils::HeightColorMap::GetColor(float height)
   {
      osg::Vec3 color;

      if(size() >= 2)
      {
         iterator c1, c2 = upper_bound(height);

         if(c2 == begin())
         {
            c1 = c2;
            c2++;
         }
         else if(c2 == end())
         {
            c2--;
            c1 = c2;
            c1--;
         }
         else
         {
            c1 = c2;
            c1--;
         }
         float t = (height-(*c1).first)/((*c2).first-(*c1).first);
         color = (*c1).second + ((*c2).second-(*c1).second)*t;
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  "SOARXTerrain::HeightColorMap: Must have at least two entries");
      }
      return color;
   }

   //////////////////////////////////////////////////////////////////////////
   void ImageUtils::ImageStats(const osg::Image* image, std::string* imagename)
   {
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();

      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "ImageStats:**********************************");

      int width = image->s();
      int height = image->t();
      int depth = image->r();

      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "ImageStats:**********************************");
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image Name = %s", imagename);
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image Width = %i", width);
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image Height = %i", height);
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image Depth = %i", depth);
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image DataType = %i", image->getDataType());
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image Packing = %i", image->getPacking());
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image PixelFormat = %i", image->getPixelFormat());
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image MipMaps = %i", image->getNumMipmapLevels());
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image PixelSize = %i", image->getPixelSizeInBits());  //24 = RGB, 32 = RGBA
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image RowSize = %i", image->getRowSizeInBytes());
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image ImageSize = %i", image->getImageSizeInBytes());
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Image TotalSize = %i", image->getTotalSizeInBytes());
   }

   //////////////////////////////////////////////////////////////////////////
   float ImageUtils::GetInterpolatedHeight(const osg::HeightField* hf, double x, double y)
   {
      int fx = (int)floor(x), cx = (int)ceil(x),
         fy = (int)floor(y), cy = (int)ceil(y);

      double v1 = hf->getHeight(fx, fy),
         v2 = hf->getHeight(cx, fy),
         v3 = hf->getHeight(fx, cy),
         v4 = hf->getHeight(cx, cy),
         v12 = v1 + (v2-v1)*(x-fx),
         v34 = v3 + (v4-v3)*(x-fx);

      return v12 + (v34-v12)*(y-fy);
   }   

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::MakeFilteredImage(const osg::Image* src_image, const osg::Vec3& rgb_selected)
   {
      int width = src_image->s();
      int height = src_image->t();

      dtCore::RefPtr<osg::Image> dst_image = new osg::Image;
      dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      unsigned char* src_data = NULL;
      unsigned char* dst_data = NULL;

      int border = 3;
      float value = 0;

      int neighbor_hits = 0;
      int next_neighbor_hits = 0;
      int third_neighbor_hits = 0;

      for(int y=0;y<height;y++)
      {
         for(int x=0;x<width;x++)
         {
            src_data = (unsigned char*)src_image->data(x,y);
            dst_data = (unsigned char*)dst_image->data(x,y);

            if ((y<border) || (x<border) || (y>height-border) || (x>width-border))
            {
               if ((src_data[0] == rgb_selected[0]) &&
                  (src_data[1] == rgb_selected[1]) &&
                  (src_data[2] == rgb_selected[2]))
                  value = 100;
               else
                  value = 0;
            }
            else
            {
               //third nearest neighbor algorithm
               if ((src_data[0] == rgb_selected[0]) &&
                  (src_data[1] == rgb_selected[1]) &&
                  (src_data[2] == rgb_selected[2]))
                  value = 50;
               else
                  value = 0;

               neighbor_hits = 0;
               next_neighbor_hits = 0;
               third_neighbor_hits = 0;

               unsigned char* tmp_data = NULL;

               tmp_data = (unsigned char*)src_image->data(x,y-1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x,y+1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x-1,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x+1,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x-1,y-1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x-1,y+1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x+1,y-1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x+1,y+1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x-2,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x+2,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x,y-2);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               tmp_data = (unsigned char*)src_image->data(x,y+2);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               value = value +									//50 for getting a hit
                  (6.82f * neighbor_hits) +				   //6.82 for getting a neighbor hit
                  (3.41f * next_neighbor_hits) +			//3.41 for getting a next neighbor hit
                  (2.27f * third_neighbor_hits);			//2.27 for getting a third neighbor hit

            }
            dst_data[0]=abs((int)(value/100.0*255.0 - 255.0));
            dst_data[1]=abs((int)(value/100.0*255.0 - 255.0));
            dst_data[2]=abs((int)(value/100.0*255.0 - 255.0));
         }
      }
      src_data = NULL;
      dst_data = NULL;

      return dst_image;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::ApplyMask(const osg::Image* src_image, const osg::Image* mask_image)
   {
      int width = src_image->s();
      int height = src_image->t();
      int width2 = mask_image->s();
      int height2 = mask_image->t();
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();

      if ((width!=width2)||(height!=height2))
      {
         mLog->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, "Source image size is %i by %i", width, height);
         mLog->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, "Mask image size is %i by %i", width2, height2);
         mLog->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, "Image sizes not identical!  Exitting program.");
         exit(-47);
      }

      dtCore::RefPtr<osg::Image> dst_image = new osg::Image;
      dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      unsigned char* src_data = NULL;
      unsigned char* mask_data = NULL;
      unsigned char* dst_data = NULL;

      float value = 0;

      for(int y=0;y<height;y++)
      {
         for(int x=0;x<width;x++)
         {
            src_data = (unsigned char*)src_image->data(x,y);
            mask_data = (unsigned char*)mask_image->data(x,y);
            dst_data = (unsigned char*)dst_image->data(x,y);

            if (mask_data[0]>225)         //not masked-out
               value = src_data[0];
            else
               value = 255;

            dst_data[0]=value;
            dst_data[1]=value;
            dst_data[2]=value;
         }
      }
      src_data = NULL;
      dst_data = NULL;
      mask_data = NULL;

      return dst_image;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::MakeSlopeAspectImage(const osg::HeightField* hf, int maxTextureSize)
   {
      int width = hf->getNumColumns();
      int height = hf->getNumRows();
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();

      dtCore::RefPtr<osg::Image> dst_image = new osg::Image;

      dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
      unsigned char* dst_data = NULL;

      float dx = hf->getXInterval();
      float dy = hf->getYInterval();

      float h1,h2,h3,h4,h5,h6,h7,h8,h9,aspect,slope,b,c;

      float myscale = 255.0f;

      float maxslope = 0.0f;

      for(int y=0;y<height;y++)
      {
         for(int x=0;x<width;x++)
         {
            dst_data = (unsigned char*)dst_image->data(x,y);

            if ((x!=0)&&(x<width)&&(y!=0)&&(y<height))
            {
               h1 = (((x-1) >= 0) && ((y+1) < height)) ? hf->getHeight(x-1,y+1) : 0;
               h2 = ((y+1) < height) ? hf->getHeight(x, y+1) : 0;
               h3 = (((x+1) < width) && ((y+1) < height)) ? hf->getHeight(x+1, y+1) : 0;
               h4 = ((x-1) >= 0) ? hf->getHeight(x-1,y) : 0;
               h5 = hf->getHeight(x,y);
               h6 = (((x+1) < width)) ? hf->getHeight(x+1,y) : 0;
               h7 = (((x-1) > 0) && ((y-1) >= 0)) ? hf->getHeight(x-1, y-1) : 0;
               h8 = ((y-1) > 0) ? hf->getHeight(x, y-1) : 0;
               h9 = (((x+1) < width) && ((y-1) >= 0)) ? hf->getHeight(x+1, y-1) : 0;

               //h1 = hf->getHeight(x-1, y+1);
               //h2 = hf->getHeight(x, y+1);
               //h3 = hf->getHeight(x+1, y+1);
               //h4 = hf->getHeight(x-1, y);
               //h5 = hf->getHeight(x, y);
               //h6 = hf->getHeight(x+1, y);
               //h7 = hf->getHeight(x-1, y-1);
               //h8 = hf->getHeight(x, y-1);
               //h9 = hf->getHeight(x+1, y-1);

               b = (h3+(2.0f*h6)+h9-h1-(2.0f*h4)-h7)/(8.0f*dx);
               c = (h1+(2.0f*h2)+h3-h7-(2.0f*h8)-h9)/(8.0f*dy);

               slope = (atan(sqrt(b*b + c*c)))*(180.0f/3.14159f);  // in degrees
               aspect = (atan(b/c))*(180.0f/3.14159f);			    // in degrees

               if (c>0) aspect = aspect + 180.0f;
               if ((c<0)&&(b>0)) aspect = aspect + 360.0f;
               if (slope==0) aspect = 0;

               if ((y==512)&&(x>1020))
                  mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Slope(%i,%i): b=%5.3f, c=%5.3f  slope=%5.3f aspect=%5.3f",x,y,b,c,slope, aspect);
            }
            else
            {
               slope = 0.0f;
               aspect = 0.0f;
            }

            dst_data[0] = 0.0f;
            dst_data[1] = (unsigned char)osg::clampTo((slope/90.0f)*myscale, 0.0f, 255.0f);  //store slope (90 degrees = 255)
            dst_data[2] = (unsigned char)osg::clampTo((aspect/360.0f)*myscale, 0.0f, 255.0f);  //store aspect

            if (slope > maxslope) maxslope = slope;
         }
      }

      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Max slope = %5.2f", maxslope);

      dst_image->ensureValidSizeForTexturing(maxTextureSize);

      dst_data = NULL;

      return dst_image;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::MakeRelativeElevationImage(const osg::HeightField* hf, int maxTextureSize)
   {
      dtCore::RefPtr<osg::Image> image = new osg::Image;
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();

      int width = hf->getNumColumns();
      int height = hf->getNumRows();

      image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      unsigned char* ptr = (unsigned char*)image->data();

      float scale = 128.0f/hf->getXInterval();

      float myscale = 5.0f;

      float relative = 0.0f;
      float relativenorm = 0.0f;
      float h = 0.0f;
      float averageheight = 0.0f;

      float maxrel = 0;
      float minrel = 999;

      //NEED TO IMPLEMENT BORDER CONDITION CASE

      for(int y=0;y<height;y++)
      {
         for(int x=0;x<width;x++)
         {
            if ((y==0) || (x==0))
               relative = 0.0f;
            else
            {
               averageheight += ((x-1) >= 0) ? hf->getHeight(x-1,y) : 0;
               averageheight += ((x-1) >= 0 && (y-1) >= 0) ? hf->getHeight(x-1,y-1) : 0;
               averageheight += ((x-1) >= 0 && (y+1) < height) ? hf->getHeight(x-1,y+1) :0;
               averageheight += ((x+1) < width) ? hf->getHeight(x+1,y) : 0;
               averageheight += ((x+1) < width && ((y-1) >=0)) ? hf->getHeight(x+1,y-1) :0;
               averageheight += ((x+1) < width && ((y+1) < height)) ? hf->getHeight(x+1,y+1) : 0;
               averageheight += ((y-1) >= 0) ? hf->getHeight(x, y-1) : 0;
               averageheight += ((y+1) < height) ? hf->getHeight(x, y+1) : 0;
               averageheight = averageheight/8.0f;

               //averageheight =
               //   ( hf->getHeight(x-1, y)
               //   + hf->getHeight(x-1, y-1)
               //   + hf->getHeight(x-1, y+1)
               //   + hf->getHeight(x+1, y)
               //   + hf->getHeight(x+1, y-1)
               //   + hf->getHeight(x+1, y+1)
               //   + hf->getHeight(  x, y-1)
               //   + hf->getHeight(  x, y+1))/8.0f;

               h = hf->getHeight(x, y);

               relative = h-averageheight;
               relativenorm = relative/(sqrt(averageheight*averageheight+1));
            }

            if (relative > maxrel)
               maxrel = relative;
            if (relative < minrel)
               minrel = relative;

            *(ptr++) = (unsigned char)osg::clampTo(relative*myscale+128.0f, 0.0f, 255.0f);  //store height
            *(ptr++) = (unsigned char)osg::clampTo(relative*myscale+128.0f, 0.0f, 255.0f);  //store height
            *(ptr++) = (unsigned char)osg::clampTo(relative*myscale+128.0f, 0.0f, 255.0f);  //store height

         }
      }

      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "MaxRel = %5.2f, MinRel = %5.2f", maxrel, minrel);

      image->ensureValidSizeForTexturing(maxTextureSize);

      ptr = NULL;

      return image;
   }

}

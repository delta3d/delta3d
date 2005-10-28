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
#include <sstream>

#include <osg/Vec3>
#include <osg/io_utils>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osgDB/ImageOptions>
#include <ogrsf_frmts.h>
#include <gdal_priv.h>
#include <gdalwarper.h>

#include "dtUtil/exception.h"
#include "dtTerrain/imageutils.h"
#include "dtTerrain/mathutils.h"
#include "dtTerrain/fixedpointnoise.h"

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ImageUtilException);
   ImageUtilException ImageUtilException::INVALID_IMAGE_DIMENSIONS("INVALID_IMAGE_DIMENSIONS");
   ImageUtilException ImageUtilException::INVALID_RASTER_FORMAT("INVALID_RASTER_FORMAT");
   ImageUtilException ImageUtilException::LOAD_FAILED("LOAD_FAILED");
   
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
   dtCore::RefPtr<osg::Image> ImageUtils::CreateBaseGradientMap(const HeightField &hf, 
      float scale)
   {
      if (hf.GetHeightFieldData() == NULL)
      {
         LOG_ERROR("Cannot create base gradient map.  HeightField data is invalid.");
         return NULL;
      }

      dtCore::RefPtr<osg::Image> image = new osg::Image();                  
      unsigned int width = hf.GetNumColumns()-1;
      unsigned int height = hf.GetNumRows()-1;    

      image->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);
      unsigned char *dstData = image->data();
      for (unsigned int i=0; i<height; i++)
      {
         for (unsigned int j=0; j<width; j++)
         {
            float h = hf.GetHeight(j,i);
            float right = hf.GetHeight(j+1,i);
            float top = hf.GetHeight(j,i+1);

            osg::Vec3 grad(0.0f,(h-right)*scale,(h-top)*scale);
            //grad.normalize();

            *(dstData++) = (unsigned char)osg::clampBetween((grad.x())+128.0f,0.0f,255.0f);
            *(dstData++) = (unsigned char)osg::clampBetween((grad.y())+128.0f,0.0f,255.0f);
            *(dstData++) = (unsigned char)osg::clampBetween((grad.z())+128.0f,0.0f,255.0f); 
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

            *(imageData++) = 0;
            *(imageData++) = (unsigned char)osg::clampTo(gx,0.0f,255.0f);
            *(imageData++) = (unsigned char)osg::clampTo(gy,0.0f,255.0f);
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
   dtCore::RefPtr<osg::Image> ImageUtils::EnsurePow2Image(const osg::Image *srcImage)
   {
      dtCore::RefPtr<osg::Image> dstImage = new osg::Image;
      unsigned int width = srcImage->s();
      unsigned int height = srcImage->t();
      float aspectRatio = 0.0;
      int dimension;

      if(width != height)
      {
         LOG_ERROR("The source image has invalid dimensions.");
         return NULL;
      }

      // find nearest power of two
      dimension = osg::Image::computeNearestPowerOfTwo(width);

      // take our new dimension and create an aspect ratio
      aspectRatio = (float)height/(float)dimension;

      // assign our new image dimensions
      width =  dimension;
      height = dimension;

      unsigned char *dstData;
      unsigned char *srcData;

      // allocate the correct size for the destination image
      dstImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      // By multiplying our aspect ratio by the x,y image coordinates
      // we can correctly size a new image x',y'. This is a simple
      // nearest neighbor algorithm.
      if(aspectRatio != 0)
      {
         // Resize the image appropriatly
         // simply x',y' = (x'*aspect,y'*aspect)
         for(unsigned int y = 0; y < height; ++y)
         {
            for(unsigned int x = 0; x < width; ++x)
            {
               dstData = (unsigned char*)dstImage->data(x,y);
               float yNew = y*aspectRatio;
               float xNew = x*aspectRatio;

               srcData = (unsigned char*)srcImage->data((int)xNew, (int)yNew);

               dstData[0] = srcData[0];
               dstData[1] = srcData[1];
               dstData[2] = srcData[2];
            }
         }
      }

      return dstImage;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 ImageUtils::HeightColorMap::GetColor(float height) const
   {
      osg::Vec3 color;

      if(size() >= 2)
      {
         const_iterator c1, c2 = upper_bound(height);

         if(c2 == begin())
         {
            c1 = c2;
            ++c2;
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
         LOG_WARNING("HeightColorMap must have at least two entries.");
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
   dtCore::RefPtr<osg::Image> ImageUtils::MakeFilteredImage(const osg::Image &src_image, 
      const osg::Vec3& rgb_selected)
   {
      int width = src_image.s();
      int height = src_image.t();

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
            src_data = (unsigned char*)src_image.data(x,y);
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

               tmp_data = (unsigned char*)src_image.data(x,y-1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x,y+1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x-1,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x+1,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x-1,y-1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x-1,y+1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x+1,y-1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x+1,y+1);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  next_neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x-2,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x+2,y);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x,y-2);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               tmp_data = (unsigned char*)src_image.data(x,y+2);
               if ((tmp_data[0] == rgb_selected[0]) &&
                  (tmp_data[1] == rgb_selected[1]) &&
                  (tmp_data[2] == rgb_selected[2]))
                  third_neighbor_hits++;

               value = value +									//50 for getting a hit
                  (6.82f * neighbor_hits) +				   //6.82 for getting a neighbor hit
                  (3.41f * next_neighbor_hits) +			//3.41 for getting a next neighbor hit
                  (2.27f * third_neighbor_hits);			//2.27 for getting a third neighbor hit

            }
            
            dst_data[0]=(unsigned char)osg::absolute(value/100.0*255.0 - 255.0);
            dst_data[1]=(unsigned char)osg::absolute(value/100.0*255.0 - 255.0);
            dst_data[2]=(unsigned char)osg::absolute(value/100.0*255.0 - 255.0);
         }
      }
      
      return dst_image;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::ApplyMask(const osg::Image &src_image, const osg::Image &mask_image)
   {
      int width = src_image.s();
      int height = src_image.t();
      int width2 = mask_image.s();
      int height2 = mask_image.t();

      if ((width!=width2) || (height!=height2))
         EXCEPT(ImageUtilException::INVALID_IMAGE_DIMENSIONS,"Source and mask image must be of the "
            "same dimensions.");
      
      dtCore::RefPtr<osg::Image> dst_image = new osg::Image;
      dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      for(int y=0;y<height;y++)
      {
         for(int x=0;x<width;x++)
         {
            unsigned char *src_data, *mask_data, *dst_data;
            unsigned char value;
            
            src_data = (unsigned char*)src_image.data(x,y);
            mask_data = (unsigned char*)mask_image.data(x,y);
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
     
      return dst_image;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::MakeSlopeAspectImage(const HeightField &hf)
   {
      dtCore::RefPtr<osg::Image> dst_image = new osg::Image;
      unsigned char* dst_data;
  
      dst_image->allocateImage(hf.GetNumColumns()-2, hf.GetNumRows()-2, 1, GL_RGB, GL_UNSIGNED_BYTE);
      dst_data = dst_image->data();
      for(unsigned int y=1; y<hf.GetNumRows()-1; y++)
      {
         for(unsigned int x=1; x<hf.GetNumColumns()-1; x++)
         {
            float h1,h2,h3,h4,h5,h6,h7,h8,h9,aspect,slope,b,c;
            
            h1 = hf.GetHeight(x-1,y+1);
            h2 = hf.GetHeight(x,y+1);
            h3 = hf.GetHeight(x+1,y+1);
            h4 = hf.GetHeight(x-1,y);
            h5 = hf.GetHeight(x,y);
            h6 = hf.GetHeight(x+1,y);
            h7 = hf.GetHeight(x-1,y-1);
            h8 = hf.GetHeight(x,y-1);
            h9 = hf.GetHeight(x+1,y-1);
                
            b = (h3+(2.0f*h6)+h9-h1-(2.0f*h4)-h7) / (8.0f*hf.GetXInterval());
            c = (h1+(2.0f*h2)+h3-h7-(2.0f*h8)-h9) / (8.0f*hf.GetYInterval());
            slope = osg::RadiansToDegrees(atanf(sqrtf(b*b + c*c)));
            aspect = osg::RadiansToDegrees(atanf(b/c));

            if (slope == 0.0f)
               aspect = 0.0f;
            else if (c > 0) 
               aspect += 180.0f;
            else if (c < 0.0f && b > 0.0f)
               aspect += 360.0f;
          
            *(dst_data++) = 0;
            *(dst_data++) = (unsigned char)osg::clampTo((slope/90.0f)*255.0f, 0.0f, 255.0f);
            *(dst_data++) = (unsigned char)osg::clampTo((aspect/360.0f)*255.0f, 0.0f, 255.0f);
         }
      }

      dst_image = ImageUtils::EnsurePow2Image(dst_image.get());
      return dst_image;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::MakeRelativeElevationImage(const HeightField &hf,
      float scale)
   {
      dtCore::RefPtr<osg::Image> image = new osg::Image;
      
      image->allocateImage(hf.GetNumColumns()-2, hf.GetNumRows()-2, 1, GL_RGB, GL_UNSIGNED_BYTE);
      unsigned char* ptr = (unsigned char*)image->data();

      float relative = 0.0f;
      float relativenorm = 0.0f;
      float h = 0.0f;
      float averageheight = 0.0f;

      float maxrel = 0;
      float minrel = 999;

      for(unsigned int y=1; y<hf.GetNumRows()-1; y++)
      {
         for(unsigned int x=1; x<hf.GetNumColumns()-1; x++)
         {
            averageheight = hf.GetHeight(x-1,y);
            averageheight += hf.GetHeight(x-1,y-1);
            averageheight += hf.GetHeight(x-1,y+1);
            averageheight += hf.GetHeight(x+1,y);
            averageheight += hf.GetHeight(x+1,y-1);
            averageheight += hf.GetHeight(x+1,y+1);
            averageheight += hf.GetHeight(x, y-1);
            averageheight += hf.GetHeight(x, y+1);
            averageheight /= 8.0f;

            h = hf.GetHeight(x, y);
            relative = h-averageheight;
            relativenorm = relative/(sqrt(averageheight*averageheight+1));

            if (relative > maxrel)
               maxrel = relative;
            if (relative < minrel)
               minrel = relative;

            *(ptr++) = (unsigned char)osg::clampTo(relative*scale+128.0f, 0.0f, 255.0f);
            *(ptr++) = (unsigned char)osg::clampTo(relative*scale+128.0f, 0.0f, 255.0f);
            *(ptr++) = (unsigned char)osg::clampTo(relative*scale+128.0f, 0.0f, 255.0f);
         }
      }

      image = ImageUtils::EnsurePow2Image(image.get());
      return image;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> ImageUtils::MakeBaseColor(const HeightField &hf, int latitude, 
         int longitude, const ImageUtils::HeightColorMap &upperHeightColorMap,
         const ImageUtils::HeightColorMap &lowerHeightColorMap, 
         std::vector<GeospecificImage> &geospecificImages, float gamma)
   {
      std::vector<GeospecificImage> images;
      std::vector<GeospecificImage>::iterator it;

      int width = hf.GetNumColumns()-1;
      int height = hf.GetNumRows()-1;

      for(it = geospecificImages.begin(); it != geospecificImages.end(); ++it)
      {
         if(latitude >= (*it).mMinLatitude && latitude <= (*it).mMaxLatitude &&
            longitude >= (*it).mMinLongitude && longitude <= (*it).mMaxLongitude)
         {
            images.push_back(*it);

            width = osg::maximum(width, 
               osg::Image::computeNearestPowerOfTwo((int)osg::absolute(1.0/(*it).mGeoTransform[1])));

            height = osg::maximum(height,
               osg::Image::computeNearestPowerOfTwo((int)osg::absolute(1.0/(*it).mGeoTransform[5])));
         }
      }

      dtCore::RefPtr<osg::Image> image = new osg::Image;
      image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
      unsigned char* ptr = (unsigned char*)image->data();

      float heightVal, l1, l2, l3, l4, l12, l34;
      osg::Vec3 c1, c2, c3, c4, c12, c34, color, coord, imgcolor, goodcolor;
      const osg::Vec3 black(0, 0, 0);
      const osg::Vec3 ltblack(20, 20, 20);

      double latStep = 1.0/height, lonStep = 1.0/width;
      double lat = latitude+latStep*0.5, lon;
      double sStep = (hf.GetNumColumns()-1.0)/width, tStep = (hf.GetNumRows()-1.0)/height;
      double s, t = tStep*0.5;

      for(int y=0;y<height;y++)
      {
         lon = longitude + lonStep*0.5;
         s = sStep*0.5;

         for(int x=0;x<width;x++)
         {
            goodcolor.set(0.0,0.0,0.0);
            color.set(0.0,0.0,0.0);

            //find color based on imagery
            for(it = images.begin();it != images.end();it++)
            {
               double x = (*it).mInverseGeoTransform[0] +
                  (*it).mInverseGeoTransform[1]*lon +
                  (*it).mInverseGeoTransform[2]*lat,
                  y = (*it).mInverseGeoTransform[3] +
                  (*it).mInverseGeoTransform[4]*lon +
                  (*it).mInverseGeoTransform[5]*lat;

               int fx = (int)floor(x), fy = (int)floor(y),
                  cx = (int)ceil(x), cy = (int)ceil(y);
               int ix = (int)x, iy = (int)y;

               if(fx >= 0 && cx < (*it).mImage->s() && fy >= 0 && cy < (*it).mImage->t())
               {
                  float ax = (float)(x - fx), ay = (float)(y - fy);

                  unsigned char* data = (*it).mImage->data(ix, iy);

                  if((*it).mImage->getPixelFormat() == GL_LUMINANCE)
                  {
                     data = (*it).mImage->data(fx, fy);
                     l1 = data[0]/255.0f;

                     data = (*it).mImage->data(cx, fy);
                     l2 = data[0]/255.0f;

                     data = (*it).mImage->data(fx, cy);
                     l3 = data[0]/255.0f;

                     data = (*it).mImage->data(cx, cy);
                     l4 = data[0]/255.0f;

                     if ((l1!=0.0)&&(l2!=0.0)&&(l3!=0.0)&&(l4!=0.0))
                     {
                        l12 = l1*(1.0f-ax) + l2*ax;
                        l34 = l3*(1.0f-ax) + l4*ax;
                        imgcolor *= (l12*(1.0f-ay) + l34*ay); 
                     }
                     else
                        imgcolor = black;
                  }
                  else
                  {
                     data = (*it).mImage->data(fx, fy);
                     c1.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     data = (*it).mImage->data(cx, fy);
                     c2.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     data = (*it).mImage->data(fx, cy);
                     c3.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     data = (*it).mImage->data(cx, cy);
                     c4.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     if ((c1!=black)&&(c2!=black)&&(c3!=black)&&(c4!=black))
                     {
                        c12 = c1*(1.0f-ax) + c2*ax;
                        c34 = c3*(1.0f-ax) + c4*ax;
                        imgcolor = c12*(1.0f-ay) + c34*ay;
                     }
                     else
                        imgcolor = black;
                  }
                  if (imgcolor!=black)
                     goodcolor = imgcolor;
               }
            }

            if (goodcolor == black)  // not able to find a non-black imagery color
            {
               //find color based on height
               heightVal = hf.GetInterpolatedHeight(s,t);
               if(heightVal > 0.0f)
               {
                  color = upperHeightColorMap.GetColor(heightVal);
               }
               else
               {
                  color = lowerHeightColorMap.GetColor(heightVal);
               }
            }
            else
            {
               color[0] = pow(goodcolor[0],1.0f/gamma);
               color[1] = pow(goodcolor[1],1.0f/gamma);
               color[2] = pow(goodcolor[2],1.0f/gamma);
            }

            *(ptr++) = (unsigned char)osg::clampTo(color[0]*255.0f, 0.0f, 255.0f);  
            *(ptr++) = (unsigned char)osg::clampTo(color[1]*255.0f, 0.0f, 255.0f);  
            *(ptr++) = (unsigned char)osg::clampTo(color[2]*255.0f, 0.0f, 255.0f);  

            lon += lonStep;            
            s += sStep;            
         }

         lat += latStep;
         t += tStep;
      }
      
      return image;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void ImageUtils::LoadGeospecificLCCImage(ImageUtils::GeospecificImage &gslcc)
   {
      int width = 0;
      int height = 0;
      int i,x,y;
      
      GDALAllRegister();
      GDALDataset* ds = (GDALDataset*)GDALOpen(gslcc.mFileName.c_str(), GA_ReadOnly);
      if (ds == NULL)
      {
         //If GDAL did not parse the image, lets try and read it as a normal
         //image.
         gslcc.mImage = osgDB::readImageFile(gslcc.mFileName.c_str());         
      }
      else
      {
         OGRSpatialReference sr(ds->GetProjectionRef()), wgs84;
         wgs84.SetWellKnownGeogCS("WGS84");

         if(!sr.IsSame(&wgs84))
         {
            LOG_INFO("Warping " + gslcc.mFileName + " to WGS84 coordinate system.");

            void* transformArg;
            double newGeoTransform[6];
            int newWidth = 0, newHeight = 0;
            char* wgs84wkt;

            wgs84.exportToWkt(&wgs84wkt);
            transformArg = GDALCreateGenImgProjTransformer(ds,
               ds->GetProjectionRef(),NULL,wgs84wkt,false,0,1);

            GDALSuggestedWarpOutput(ds,GDALGenImgProjTransform,transformArg,
               newGeoTransform,&newWidth,&newHeight);

            GDALDestroyGenImgProjTransformer(transformArg);

            GDALDataset* newDS = (GDALDataset*)GDALCreate(GDALGetDriverByName("GTiff"),
               "temp.tif",newWidth,newHeight,ds->GetRasterCount(),
               ds->GetRasterBand(1)->GetRasterDataType(),NULL);

            newDS->SetProjection(wgs84wkt);
            newDS->SetGeoTransform(newGeoTransform);

            GDALColorTableH hCT;
            hCT = GDALGetRasterColorTable( ds->GetRasterBand(1));
            if( hCT != NULL )
               GDALSetRasterColorTable( newDS->GetRasterBand(1), hCT );
            
            GDALWarpOptions* warpOptions = GDALCreateWarpOptions();
            warpOptions->hSrcDS = ds;
            warpOptions->hDstDS = newDS;
            warpOptions->nBandCount = ds->GetRasterCount();

            warpOptions->panSrcBands = (int*)CPLMalloc(sizeof(int)*warpOptions->nBandCount);
            warpOptions->panDstBands = (int*)CPLMalloc(sizeof(int)*warpOptions->nBandCount);
            for(int i=0; i<warpOptions->nBandCount; i++)
               warpOptions->panSrcBands[i] = warpOptions->panDstBands[i] = i+1;
            
            warpOptions->pTransformerArg = GDALCreateGenImgProjTransformer(
               ds,ds->GetProjectionRef(),newDS,newDS->GetProjectionRef(),false,0,1);

            warpOptions->pfnTransformer = GDALGenImgProjTransform;
            warpOptions->pfnProgress = GDALTermProgress;
            
            GDALWarpOperation warpOperation;
            warpOperation.Initialize(warpOptions);
            warpOperation.ChunkAndWarpImage(0,0,newWidth,newHeight);

            GDALDestroyGenImgProjTransformer(warpOptions->pTransformerArg);
            GDALDestroyWarpOptions(warpOptions);

            GDALClose(ds);
            ds = newDS;
         }

         ds->GetGeoTransform(gslcc.mGeoTransform);
         width = ds->GetRasterXSize();
         height = ds->GetRasterYSize();

         int bands = ds->GetRasterCount();
         if(bands == 1)
         {
            gslcc.mImage = new osg::Image;
            GDALColorTableH hCT = GDALGetRasterColorTable(ds->GetRasterBand(1));
            if(hCT != NULL) //color palette exists
            {
               gslcc.mImage->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);

               //set RGB values to color map indices
               for(i=0; i<3; i++)
               {
                  ds->GetRasterBand(1)->RasterIO(GF_Read,0,0,width, height,
                     gslcc.mImage->data()+i,width,height,GDT_Byte,3,0);
               }

               unsigned char* ptr = (unsigned char*)gslcc.mImage->data();
               GDALColorTableH hTable;
               hTable = GDALGetRasterColorTable(ds->GetRasterBand(1));
               GDALColorEntry sEntry;

               for(y=0; y<height; y++)
               {
                  for(x=0; x<width; x++)
                  {
                     GDALGetColorEntryAsRGB( hTable,*(ptr), &sEntry );
                     *(ptr++) = sEntry.c1;
                     *(ptr++) = sEntry.c2;
                     *(ptr++) = sEntry.c3;
                  }
               }
            }
            else // no color palette exists -> grayscale
            {
               gslcc.mImage->allocateImage(width,height,1,GL_LUMINANCE,GL_UNSIGNED_BYTE);
               ds->GetRasterBand(1)->RasterIO(GF_Read,0,0,width,height,
                  gslcc.mImage->data(),width,height,GDT_Byte,0,0);
            }
         }
         else if(bands == 3)
         {
            gslcc.mImage = new osg::Image;
            gslcc.mImage->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);

            for(i=0; i<3; i++)
            {
               ds->GetRasterBand(i+1)->RasterIO(GF_Read,0,0,width,height,
                  gslcc.mImage->data()+i,width,height,GDT_Byte,3,0);
            }
         }
         else
         {
            std::ostringstream error;
            error << "Image file: " << gslcc.mFileName << " has an invalid raster format. NumBands = " 
               << bands;
            EXCEPT(ImageUtilException::INVALID_RASTER_FORMAT,error.str());
         }

         delete ds;
      }
   
      if (!gslcc.mImage.valid())
      {
         std::string errorFile = gslcc.mFileName;
         if (gslcc.mFileName.empty())
            errorFile = "nil";         
         EXCEPT(ImageUtilException::LOAD_FAILED,"Failed to load: " + errorFile);
      }
         
      gslcc.mMinLatitude = gslcc.mMinLongitude = 999;
      gslcc.mMaxLatitude = gslcc.mMaxLongitude = -999;

      for(i=0; i<4; i++)
      {
         x = (i & 1) ? 0 : width-1,
         y = (i & 2) ? 0 : height-1;

         int longitude = (int)floor(gslcc.mGeoTransform[0] + x*gslcc.mGeoTransform[1] + y*gslcc.mGeoTransform[2]);
         int latitude = (int)floor(gslcc.mGeoTransform[3] + x*gslcc.mGeoTransform[4] + y*gslcc.mGeoTransform[5]);

         gslcc.mMinLatitude = osg::minimum(latitude, gslcc.mMinLatitude);
         gslcc.mMinLongitude = osg::minimum(longitude, gslcc.mMinLongitude);
         gslcc.mMaxLatitude = osg::maximum(latitude, gslcc.mMaxLatitude);
         gslcc.mMaxLongitude = osg::maximum(longitude, gslcc.mMaxLongitude);
      }

      double d = gslcc.mGeoTransform[1]*gslcc.mGeoTransform[5]-gslcc.mGeoTransform[2]*gslcc.mGeoTransform[4];
     
      gslcc.mInverseGeoTransform[0] = 
         (gslcc.mGeoTransform[2]*gslcc.mGeoTransform[3]-gslcc.mGeoTransform[5]*gslcc.mGeoTransform[0])/d;

      gslcc.mInverseGeoTransform[1] = gslcc.mGeoTransform[5]/d;
      gslcc.mInverseGeoTransform[2] = -gslcc.mGeoTransform[2]/d;

      gslcc.mInverseGeoTransform[3] = 
         (gslcc.mGeoTransform[4]*gslcc.mGeoTransform[0]-gslcc.mGeoTransform[1]*gslcc.mGeoTransform[3])/d;

      gslcc.mInverseGeoTransform[4] = -gslcc.mGeoTransform[4]/d;
      gslcc.mInverseGeoTransform[5] = gslcc.mGeoTransform[1]/d;
   }
}

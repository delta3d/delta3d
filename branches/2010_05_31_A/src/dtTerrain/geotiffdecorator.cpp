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
* Matthew W. Campbell
*/
#include <dtTerrain/geotiffdecorator.h>

#include <osg/Image>
#include <osg/Math>
#include <dtCore/refptr.h>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

namespace dtTerrain
{
   const std::string GEOTIFF_IMAGE_NAME = "geo_tiff_decorator_image.rgb";
   
   //////////////////////////////////////////////////////////////////////////
   GeoTiffDecorator::GeoTiffDecorator(const std::string &name) : TerrainDecorationLayer(name)
   {
      mResultImageWidth = 1024;
      mResultImageHeight = 1024;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void GeoTiffDecorator::OnLoadTerrainTile(PagedTerrainTile &tile)
   {
      int lat = (int)floor(tile.GetGeoCoordinates().GetLatitude());
      int lon = (int)floor(tile.GetGeoCoordinates().GetLongitude());
      osg::Image *image;
      
      if (tile.IsCachingEnabled())
      {
         std::string imagePath = tile.GetCachePath() + "/" + GEOTIFF_IMAGE_NAME;         
         if (dtUtil::FileUtils::GetInstance().FileExists(imagePath))
         {
            LOG_INFO("Reading geo tiff base image from cache: " + imagePath);
            image = osgDB::readImageFile(imagePath);
            tile.SetBaseTextureImage(image);
         }
         else
         {
            LOG_INFO("Generating base geo tiff texture drape.");
            image = CalculateBaseImage(lat,lon);
            tile.SetBaseTextureImage(image);
            
            LOG_INFO("Caching base geo tiff texture drape.");
            if (tile.GetBaseTextureImage() != NULL)
               osgDB::writeImageFile(*image,imagePath);
         }
      }
      else
      {
         LOG_INFO("Generating base geo tiff texture drape.");
         image = CalculateBaseImage(lat,lon);
         tile.SetBaseTextureImage(image);
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void GeoTiffDecorator::LoadAllGeoSpecificImages()
   {
      std::vector<ImageUtils::GeospecificImage>::iterator itor;
      for (itor=mImageList.begin(); itor!=mImageList.end(); ++itor)
      {
         if (itor->mImage == NULL)
            ImageUtils::LoadGeospecificLCCImage(*itor);
      }     
   }
         
   //////////////////////////////////////////////////////////////////////////
   osg::Image *GeoTiffDecorator::CalculateBaseImage(int lat, int lon)
   {
      if (mResultImageWidth == 0 || mResultImageHeight == 0)
         return NULL;
         
      LoadAllGeoSpecificImages();
      osg::Image *resultImage = new osg::Image();
      
      std::vector<ImageUtils::GeospecificImage> images;
      std::vector<ImageUtils::GeospecificImage>::iterator itor;
      unsigned int width = mResultImageWidth;
      unsigned int height = mResultImageHeight;

      for(itor=mImageList.begin(); itor!=mImageList.end(); ++itor)
      {
         if(lat >= itor->mMinLatitude && lat <= itor->mMaxLatitude &&
            lon >= itor->mMinLongitude && lon <= itor->mMaxLongitude)
         {
            images.push_back(*itor);

            width = osg::maximum((int)width, 
               osg::Image::computeNearestPowerOfTwo((int)osg::absolute(1.0/itor->mGeoTransform[1])));

            height = osg::maximum((int)height,
               osg::Image::computeNearestPowerOfTwo((int)osg::absolute(1.0/itor->mGeoTransform[5])));
         }
      }

      resultImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
      unsigned char* ptr = (unsigned char*)resultImage->data();

      float l1, l2, l3, l4, l12, l34;
      osg::Vec3 c1, c2, c3, c4, c12, c34, color, coord, imgcolor, goodcolor;
      const osg::Vec3 black(0, 0, 0);
      const osg::Vec3 ltblack(20, 20, 20);

      float latStep = 1.0f/height, lonStep = 1.0f/width;
      float currLat = lat+latStep*0.5f, currLon;
    
      for(unsigned int y=0; y<height; y++)
      {
         currLon = lon + lonStep*0.5f;
       
         for(unsigned int x=0; x<width; x++)
         {
            goodcolor.set(0.0,0.0,0.0);
            color.set(0.0,0.0,0.0);

            //find color based on imagery
            for(itor=images.begin(); itor!=images.end(); ++itor)
            {
               double x = itor->mInverseGeoTransform[0] +
                  itor->mInverseGeoTransform[1]*currLon +
                  itor->mInverseGeoTransform[2]*currLat,
                  y = itor->mInverseGeoTransform[3] +
                  itor->mInverseGeoTransform[4]*currLon +
                  itor->mInverseGeoTransform[5]*currLat;

               int fx = (int)floor(x), fy = (int)floor(y),
                  cx = (int)ceil(x), cy = (int)ceil(y);
               int ix = (int)x, iy = (int)y;

               if(fx >= 0 && cx < itor->mImage->s() && fy >= 0 && cy < itor->mImage->t())
               {
                  float ax = (float)(x - fx), ay = (float)(y - fy);
                  unsigned char* data = itor->mImage->data(ix, iy);

                  if(itor->mImage->getPixelFormat() == GL_LUMINANCE)
                  {
                     data = itor->mImage->data(fx, fy);
                     l1 = data[0]/255.0f;

                     data = itor->mImage->data(cx, fy);
                     l2 = data[0]/255.0f;

                     data = itor->mImage->data(fx, cy);
                     l3 = data[0]/255.0f;

                     data = itor->mImage->data(cx, cy);
                     l4 = data[0]/255.0f;

                     if ((l1!=0.0)&&(l2!=0.0)&&(l3!=0.0)&&(l4!=0.0))
                     {
                        l12 = l1*(1.0f-ax) + l2*ax;
                        l34 = l3*(1.0f-ax) + l4*ax;
                        imgcolor *= (l12*(1.0f-ay) + l34*ay); 
                     }
                     else
                        imgcolor = ltblack;
                  }
                  else
                  {
                     data = itor->mImage->data(fx, fy);
                     c1.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     data = itor->mImage->data(cx, fy);
                     c2.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     data = itor->mImage->data(fx, cy);
                     c3.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     data = itor->mImage->data(cx, cy);
                     c4.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);

                     if ((c1!=ltblack) && (c2!=ltblack) && (c3!=ltblack) && (c4!=ltblack))
                     {
                        c12 = c1*(1.0f-ax) + c2*ax;
                        c34 = c3*(1.0f-ax) + c4*ax;
                        imgcolor = c12*(1.0f-ay) + c34*ay;
                     }
                     else
                        imgcolor = ltblack;
                  }
                  if (imgcolor!=ltblack)
                     goodcolor = imgcolor;
               }
            }

            *(ptr++) = (unsigned char)osg::clampTo(imgcolor[0]*255.0f, 0.0f, 255.0f);  
            *(ptr++) = (unsigned char)osg::clampTo(imgcolor[1]*255.0f, 0.0f, 255.0f);  
            *(ptr++) = (unsigned char)osg::clampTo(imgcolor[2]*255.0f, 0.0f, 255.0f);  

            currLon += lonStep;            
         }

         currLat += latStep;
      }
           
      return resultImage;
   }
   
}

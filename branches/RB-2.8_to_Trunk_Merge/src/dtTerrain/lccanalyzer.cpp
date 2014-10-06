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
 * Teague Coonan
 */

#include <dtCore/refptr.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <osg/Image>
#include <osg/Math>
#include <osgDB/ImageOptions>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtTerrain/mathutils.h>
#include <dtTerrain/imageutils.h>
#include <dtTerrain/lccanalyzer.h>

#include <ogrsf_frmts.h>
#include <gdal_priv.h>
#include <gdalwarper.h>

#include <sstream>

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(LCCAnalyzerResourceName);
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::IMAGE_EXT(".bmp");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::BASE_LCC_COLOR("baselcc");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::WATER_MASK("water_mask");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::BASE_FILTER_NAME("lcc_filter");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::LCC_IMAGE_NAME("lcc_image");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::SLOPE_IMAGE("slope");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::REL_ELEV_IMAGE("rel_elevation");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::COMPOSITE_LCC_IMAGE("combined_lcc");
   const LCCAnalyzerResourceName LCCAnalyzerResourceName::SCENE_GRAPH("vegescene.ive");


   //////////////////////////////////////////////////////////////////////////
   LCCAnalyzer::LCCAnalyzer()
   {
      mOutputDebugImages = false;
      mImageSize = 1024;
   }

   //////////////////////////////////////////////////////////////////////////
   LCCAnalyzer::~LCCAnalyzer()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   bool LCCAnalyzer::ProcessLCCData(const PagedTerrainTile &tile, LCCType &type)
   {
      if (!tile.IsCachingEnabled())
         throw dtTerrain::LCCInvalidCacheException("Must enable terrain caching for "
            " the LCC analyzer to function properly.", __FILE__, __LINE__);

      //First, we need to check and see if we have a combined image for the
      //LCC type.  The combined image represents the final composited probability
      //for that type.  If we have it, no need to continue!
      std::ostringstream ss;
      ss << tile.GetCachePath() << "/" <<
         LCCAnalyzerResourceName::COMPOSITE_LCC_IMAGE.GetName() << type.GetIndex() <<
         LCCAnalyzerResourceName::IMAGE_EXT.GetName();
      if (dtUtil::FileUtils::GetInstance().FileExists(ss.str()))
         return true;

      //If not, go through the LOONG process of generating the probability map.
      const HeightField *hf = tile.GetHeightField();
      int latitude = (int)floor(tile.GetGeoCoordinates().GetLatitude());
      int longitude = (int)floor(tile.GetGeoCoordinates().GetLongitude());

      //Before we can process an lcc type, we need to make sure there are base
      //lcc images which are required to start the probability calculations.
      if (mGeospecificLCCImages.empty())
      {
         LOG_WARNING("Must specifiy at least one valid geographic image for use "
          "by the LCC analyzer.");
         return false;
      }

      LoadAllGeoSpecificImages();
      if (!CheckBaseLCCImages(*hf,latitude,longitude,tile.GetCachePath()))
         return false;

      //Now we can continue processing the required images for the specified
      //LCC type.
      LOG_INFO("Computing probability map for: " + tile.GetCachePath());
      ComputeProbabilityMap(*hf,type,latitude,longitude,tile.GetCachePath());
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   void LCCAnalyzer::CheckSlopeAndElevationMaps(const HeightField &hf,
      const std::string &tileCachePath)
   {
      if (!mSlopeMap.valid())
      {
         LOG_INFO("Generating and caching slope map.");
         mSlopeMap = ImageUtils::MakeSlopeAspectImage(hf);
         std::string slopeMapPath = tileCachePath + "/" +
            LCCAnalyzerResourceName::SLOPE_IMAGE.GetName() +
            LCCAnalyzerResourceName::IMAGE_EXT.GetName();

         //We always cache the slope map since it is used by other parts
         //of the analyzer.
         osgDB::writeImageFile(*mSlopeMap.get(),slopeMapPath);
      }

      if (!mRelativeElevationMap.valid())
      {
         LOG_INFO("Generating relative elevation map.");
         mRelativeElevationMap = ImageUtils::MakeRelativeElevationImage(hf,5.0f);
         if (OutputDebugImages())
         {
            std::string relElevMapPath = tileCachePath + "/" +
               LCCAnalyzerResourceName::REL_ELEV_IMAGE.GetName() +
               LCCAnalyzerResourceName::IMAGE_EXT.GetName();

            osgDB::writeImageFile(*mRelativeElevationMap.get(),relElevMapPath);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool LCCAnalyzer::CheckBaseLCCImages(const HeightField &hf, int latitude, int longitude,
      const std::string &tileCachePath)
   {
      if (!mBaseLCCColorImage.valid())
      {
         //Generate the base lcc color image.
         LOG_INFO("Generating base LCC color map.");
         mBaseLCCColorImage = MakeBaseLCCColor(hf,latitude,longitude);
         if (mBaseLCCColorImage == NULL)
            return false;

         if (OutputDebugImages())
         {
            std::string baseLCCColorPath = tileCachePath + "/" +
               LCCAnalyzerResourceName::BASE_LCC_COLOR.GetName() +
               LCCAnalyzerResourceName::IMAGE_EXT.GetName();

            LOG_INFO("Writing lcc color map to cache: " + baseLCCColorPath);
            osgDB::writeImageFile(*mBaseLCCColorImage.get(),baseLCCColorPath);
         }
      }

      if (!mWaterMask.valid())
      {
         //Generate a water LCC filter.
         LOG_INFO("Generating water mask image.");
         mWaterMask = MakeLCCMask(*mBaseLCCColorImage.get(),110,130,177);
         mWaterMask = ImageUtils::MakeFilteredImage(*mWaterMask.get(),osg::Vec3(0,0,0));

         if (OutputDebugImages())
         {
            std::string waterMaskPath = tileCachePath + "/" +
               LCCAnalyzerResourceName::WATER_MASK.GetName() +
               LCCAnalyzerResourceName::IMAGE_EXT.GetName();

            LOG_INFO("Writing water mask image to cache: " + waterMaskPath);
            osgDB::writeImageFile(*mWaterMask.get(),waterMaskPath);
         }
      }

      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   void LCCAnalyzer::ComputeProbabilityMap(const HeightField &hf, LCCType &type,
      int latitude, int longitude, const std::string &tileCachePath)
   {
      //Create our base filter for this LCC type.
      dtCore::RefPtr<osg::Image> lccImage =
         GenerateBaseFilterImage(type,tileCachePath);

      //Make sure we have valid slope and elevation maps.
      CheckSlopeAndElevationMaps(hf,tileCachePath);

      //Finally, we composite our generated images into the final product:
      //a probability map for LCC vegetation for a particular region.
      LOG_INFO("Generating combined probability map.");
      dtCore::RefPtr<osg::Image> compositeImage =
         MakeCombinedImage(type,hf,*lccImage.get(),*mSlopeMap.get(),*mRelativeElevationMap.get());

      LOG_INFO("Caching combined probability map.");
      std::ostringstream ss;
      ss << tileCachePath << "/" <<
         LCCAnalyzerResourceName::COMPOSITE_LCC_IMAGE.GetName() << type.GetIndex() <<
         LCCAnalyzerResourceName::IMAGE_EXT.GetName();
      osgDB::writeImageFile(*compositeImage.get(),ss.str());
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> LCCAnalyzer::GenerateBaseFilterImage(LCCType &type,
      const std::string &tileCachePath)
   {
      std::ostringstream fileNameSS;
      int idx = type.GetIndex();

      //First we need a base filter.
      LOG_INFO("Generating LCC base filter.");
      dtCore::RefPtr<osg::Image> lccImage;

      LOG_INFO("Generating LCC image.");
      unsigned char r,g,b;
      type.GetRGB(r,g,b);
      lccImage = MakeLCCMask(*mBaseLCCColorImage.get(),r,g,b);

      if (OutputDebugImages())
      {
         fileNameSS.str("");
         fileNameSS << tileCachePath << "/" <<
            LCCAnalyzerResourceName::LCC_IMAGE_NAME.GetName() << idx <<
            LCCAnalyzerResourceName::IMAGE_EXT.GetName();

         LOG_INFO("Writing LCC image to cache: " + fileNameSS.str());
         osgDB::writeImageFile(*lccImage.get(),fileNameSS.str());
      }

      LOG_INFO("Filtering LCC image.");
      osg::Vec3 filterRGB(0.0,0.0,0.0);
      lccImage = ImageUtils::MakeFilteredImage(*lccImage.get(),filterRGB);

      if (OutputDebugImages())
      {
         fileNameSS.str("");
         fileNameSS << tileCachePath << "/" <<
            LCCAnalyzerResourceName::BASE_FILTER_NAME.GetName() << idx <<
            LCCAnalyzerResourceName::IMAGE_EXT.GetName();

         LOG_INFO("Writing filtered LCC image to cache: " + fileNameSS.str());
         osgDB::writeImageFile(*lccImage.get(),fileNameSS.str());
      }

      LOG_INFO("Applying water mask to base LCC filter.");
      lccImage = ImageUtils::ApplyMask(*lccImage.get(),*mWaterMask.get());

      if (OutputDebugImages())
      {
         fileNameSS.str("");
         fileNameSS << tileCachePath << "/" <<
            LCCAnalyzerResourceName::BASE_FILTER_NAME.GetName() << "11" <<
            LCCAnalyzerResourceName::IMAGE_EXT.GetName();
         osgDB::writeImageFile(*lccImage.get(),fileNameSS.str());
      }

      return lccImage;
   }

   //////////////////////////////////////////////////////////////////////////
   void LCCAnalyzer::LoadAllGeoSpecificImages()
   {
      std::vector<ImageUtils::GeospecificImage>::iterator itor;
      for (itor=mGeospecificLCCImages.begin(); itor!=mGeospecificLCCImages.end(); ++itor)
      {
         if (itor->mImage == NULL)
         {
            ImageUtils::LoadGeospecificLCCImage(*itor);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> LCCAnalyzer::MakeBaseLCCColor(const HeightField &hf,
      int latitude, int longitude)
   {
      std::vector<ImageUtils::GeospecificImage> currRegionImages;
      std::vector<ImageUtils::GeospecificImage>::iterator itor;
      unsigned int width,height;

      // LCC data used for this particular terrain tile
      //Need to use mImageSize!
      width=height=osg::Image::computeNearestPowerOfTwo(hf.GetNumColumns());
      for (itor = mGeospecificLCCImages.begin(); itor!=mGeospecificLCCImages.end(); ++itor)
      {
         if (latitude >= itor->mMinLatitude && latitude <= itor->mMaxLatitude &&
            longitude >= itor->mMinLongitude && longitude <= itor->mMaxLongitude)
         {
            currRegionImages.push_back(*itor);
         }
      }

      //If we do not have any LCC data for this latitude/longitude region no use
      //in proceeding.
      if (currRegionImages.empty())
      {
         LOG_WARNING("No available LCC data for current latitude/longitude cell.");
         return NULL;
      }

      dtCore::RefPtr<osg::Image> lccImage = new osg::Image();
      lccImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
      unsigned char *data = (unsigned char *)lccImage->data();

      osg::Vec3 color;
      float latStep = 1.0f/height, lonStep = 1.0f/width;
      float currLat = latitude, currLon;

      for (unsigned int y=0; y<height; y++)
      {
         currLon = longitude;
         for (unsigned int x=0; x<width; x++)
         {
            //Calculate the value at this pixel using LCC color data.
            color = osg::Vec3(0,0,0);
            for (itor = currRegionImages.begin(); itor!=currRegionImages.end(); ++itor)
            {
               int iX = (int)(itor->mInverseGeoTransform[0] +
                  (itor->mInverseGeoTransform[1]*currLon) +
                  (itor->mInverseGeoTransform[2]*currLat));

               int iY = (int)(itor->mInverseGeoTransform[3] +
                  (itor->mInverseGeoTransform[4]*currLon) +
                  (itor->mInverseGeoTransform[5]*currLat));

               if (iX >= 0 && iY >= 0 && iX < itor->mImage->s() && iY < itor->mImage->t())
               {
                  unsigned char *srcData = itor->mImage->data(iX,iY);
                  color[0] = (srcData[0]/255.0f);
                  color[1] = (srcData[1]/255.0f);
                  color[2] = (srcData[2]/255.0f);
               }
            }

            *(data++) = (unsigned char)(color[0]*255);
            *(data++) = (unsigned char)(color[1]*255);
            *(data++) = (unsigned char)(color[2]*255);

            currLon += lonStep;
         }

         currLat += latStep;
      }

      return lccImage;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> LCCAnalyzer::MakeLCCMask(const osg::Image &src_image,
      unsigned char r, unsigned char g, unsigned char b)
   {
      dtCore::RefPtr<osg::Image> dst_image = new osg::Image();

      if (src_image.valid())
      {
         int width = src_image.s();
         int height = src_image.t();
         unsigned char* src_data = NULL;
         unsigned char* dst_data = NULL;

         dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
         for (int y=0; y<height; y++)
         {
            for (int x=0; x<width; x++)
            {
               src_data = (unsigned char*)src_image.data(x,y);
               dst_data = (unsigned char*)dst_image->data(x,y);

               if (src_data[0] == r && src_data[1] == g && src_data[2] == b)
               {
                  dst_data[0]=0;
                  dst_data[1]=0;
                  dst_data[2]=0;
               }
               else
               {
                  dst_data[0]=255;
                  dst_data[1]=255;
                  dst_data[2]=255;
               }
            }
         }
      }

      return dst_image;
   }
   //////////////////////////////////////////////////////////////////////////
   void LCCAnalyzer::LCCHistogram(const osg::Image &LCCbase, const osg::Image &image,
      const std::string &fileName, int binsize)
   {
      unsigned char* lcc_data = NULL;
      unsigned char* image_data = NULL;

      int width = image.s();
      int height = image.t();

      unsigned int hitbin[51];      //range is 5 for 51 bins
      unsigned int missbin[51];      //range is 5 for 51 bins

      unsigned int LCChits =0;
      unsigned int LCCmisses =0;

      unsigned int binnumber=0;

      for (int i=0;i<51;i++)
      {
         hitbin[i] = 0;
         missbin[i] = 0;
      }

      for (int y=0;y<height;y++)
      {
         for (int x=0;x<width;x++)
         {
            lcc_data = (unsigned char*)LCCbase.data(x,y);
            image_data = (unsigned char*)image.data(x,y);

            if (lcc_data[0] == 0) // a hit!
            {
               binnumber = int(image_data[0]/binsize);
               hitbin[binnumber]++;
               LCChits++;
            }
            else            // a miss!
            {
               binnumber = int(image_data[0]/binsize);
               missbin[binnumber]++;
               LCCmisses++;
            }
         }
      }

      FILE *histofile = fopen(fileName.c_str(), "w");
      fprintf(histofile, "%s\n", fileName.c_str());
      fprintf(histofile, "hits = %i, misses = %i\n", LCChits, LCCmisses);
      fprintf(histofile, "%s, %s, %s\n", "bin#", "hitbin", "missbin");

      for (int i=0;i<51;i++)
      {
         fprintf(histofile, "%i, %i, %i\n", i, hitbin[i], missbin[i]);
      }
      fflush(histofile);
      fclose(histofile);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> LCCAnalyzer::MakeCombinedImage(dtTerrain::LCCType &l,
      const HeightField &hf, const osg::Image &f_image, const osg::Image &s_image,
      const osg::Image &r_image)
   {
      //Make sure
      int max_height = int(l.GetMaxElevation()/10.0f);
      int min_height = int(l.GetMinElevation()/10.0f);
      int max_slope = int((l.GetMaxSlope()/90.0f) * 255.0f);


      unsigned char* f_data = NULL;
      unsigned char* s_data = NULL;
      unsigned char* r_data = NULL;
      unsigned char* dst_data = NULL;

      int im_width = f_image.s();
      int im_height = f_image.t();
      int hf_width = hf.GetNumColumns();

      float scale = (float)im_width / (float)hf_width;
      dtCore::RefPtr<osg::Image> dst_image = new osg::Image;
      dst_image->allocateImage(im_width, im_height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      float value = 0;
      float height_value=0;
      float slope_value=0;
      float aspect_value=0;
      float relel_value=0;

      for (int y=0;y<im_height;y++)
      {
         for (int x=0;x<im_width;x++)
         {
            height_value = hf.GetHeight((int)(x/scale), (int)(y/scale));

            dst_data = (unsigned char*)dst_image->data(x,y);
            f_data = (unsigned char*)f_image.data(x,y);
            s_data = (unsigned char*)s_image.data(int(x/scale), int(y/scale));
            r_data = (unsigned char*)r_image.data(int(x/scale), int(y/scale));

            value = f_data[0]; // start with filter data as basis
            relel_value  = r_data[0];
            slope_value  = s_data[1];
            aspect_value = s_data[2];

            if (value <= 254 )                        // nonwhite -> has vegetation possibility
            {
               if (height_value <= min_height)            // busted height limits (do this as a curve)
                  value = 999;
               if (height_value >= max_height)            // busted height limits (do this as a curve)
                  value = 998;
               if (slope_value > max_slope)            // busted slope limit  (do this as a curve)
                  value = 997;

               if (value <= 254)
               {
                  float redelta = relel_value - 128.0f;
                  if (redelta > 0)
                  {
                     // upward relative elevation (unfavorable)
                     value = value + 1.5f*redelta;
                  }
                  else
                  {
                     // downward relevation elevation (favorable)
                     value = value + 1.5f*redelta;
                  }
                  value = value + (slope_value/max_slope)*100.0f;      //greater slope is unfavorable (linear)
               }
            }

            if (value == 999)                     //below min elevation
            {
               dst_data[0] = 0;
               dst_data[1] = 0;
               dst_data[2] = 255;
            }
            else if (value == 998)                  //above max elevation
            {
               dst_data[0] = 0;
               dst_data[1] = 255;
               dst_data[2] = 0;
            }
            else if (value == 997)                  //slope too great
            {
               dst_data[0] = 0;
               dst_data[1] = 128;
               dst_data[2] = 255;
            }
            else
            {
               dst_data[0] = (unsigned char)osg::clampTo(value,0.0f,255.0f);  //store aspect
               dst_data[1] = (unsigned char)osg::clampTo(value,0.0f,255.0f);  //store probability
               dst_data[2] = (unsigned char)osg::clampTo(value,0.0f,255.0f);  //store probability
            }
         }
      }

      return dst_image;
   }

   //////////////////////////////////////////////////////////////////////////
   bool LCCAnalyzer::IsGeoSpecificLCCImageLoaded(const std::string &fileName)
   {
      for (unsigned int i=0; i<mGeospecificLCCImages.size(); i++)
         if (mGeospecificLCCImages[i].mFileName == fileName &&
             mGeospecificLCCImages[i].mImage.valid())
            return true;

      return false;
   }


   ////////////////////////////////////////////////////////////////////////////////
   LCCInvalidCacheException::LCCInvalidCacheException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   LCCNoValidGeoImagesException::LCCNoValidGeoImagesException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
}

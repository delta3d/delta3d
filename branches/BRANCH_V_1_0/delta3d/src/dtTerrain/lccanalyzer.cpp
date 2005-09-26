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

#include <dtCore/refptr.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <osg/Image>
#include <osgDB/ImageOptions>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <dtUtil/log.h>
#include "dtTerrain/mathutils.h"
#include "dtTerrain/imageutils.h"
#include "dtTerrain/lccanalyzer.h"

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   LCCAnalyzer::LCCAnalyzer()
   {
      mLowerHeightColorMap[-11000.0f].set(0, 0, 0); // Marianas Trench
      mLowerHeightColorMap[0.0f].set(0, 0.3, 0.6);

      mUpperHeightColorMap[0.0f].set(0.647, 0.482, 0.224);
      mUpperHeightColorMap[300.0f].set(0.710, 0.647, 0.388);
      mUpperHeightColorMap[600.0f].set(0.741, 0.741, 0.482);
      mUpperHeightColorMap[1200.0f].set(0.290, 0.612, 0.290);
      mUpperHeightColorMap[2000.0f].set(0.482, 0.741, 0.322);
      mUpperHeightColorMap[2500.0f].set(0.647, 0.809, 0.518);
      mUpperHeightColorMap[3000.0f].set(1, 1, 1);
      mUpperHeightColorMap[9000.0f].set(1, 1, 1); // Everest

      mImageExtension = ".jpg";
   }

   //////////////////////////////////////////////////////////////////////////
   LCCAnalyzer::~LCCAnalyzer()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void LCCAnalyzer::LoadLCCData(const osg::HeightField *hf, unsigned int latitude, unsigned int longitude, int mMaxTextureSize, char* cellName)
   {  
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();
      dtCore::RefPtr<osg::Image> baseLCCColor;
      
      std::string mCachePath = "cache";

      std::string baseLCCColorPath = mCachePath + "/" + cellName + ".baselcc.color" + mImageExtension;
      dtCore::RefPtr<osgDB::ImageOptions> options = new osgDB::ImageOptions;

      // Grab the base LCC color from osgDB
      if(osgDB::fileExists(baseLCCColorPath))
      {
         baseLCCColor = osgDB::readImageFile(baseLCCColorPath);
      }
      else
      {
         mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "SOARXTerrain: Making base LCC color image for %s...", cellName);
         baseLCCColor = MakeBaseLCCColor(hf, latitude, longitude, mMaxTextureSize, mUpperHeightColorMap, mLowerHeightColorMap);
         baseLCCColor->ensureValidSizeForTexturing(mMaxTextureSize);

         //write out the image
         osgDB::writeImageFile(*(baseLCCColor.get()),baseLCCColorPath);
      }

      // mLCCs is the collection lcc data
      for(std::vector<dtTerrain::LCCType>::iterator l = mLCCs.begin();
         l != mLCCs.end();
         l++)
      {
         int idx = (*l).GetIndex();
         char idxnum[3];
         sprintf(idxnum, "%i",idx);

         std::string LCCfilterPath = mCachePath + "/" + cellName + ".lcc.filter." + idxnum + mImageExtension;

         if(osgDB::fileExists(LCCfilterPath))
         {
            //not really needed - the file exists!
            //LCCfilter = osgDB::readImageFile(LCCfilterPath);
         }
         else
         {
            dtCore::RefPtr<osg::Image> LCCimage;
            std::string LCCimagePath = mCachePath + "/" + cellName + ".lcc.image." + idxnum + mImageExtension;

            if(osgDB::fileExists(LCCimagePath))
            {
               LCCimage = osgDB::readImageFile(LCCimagePath);
            }
            else
            {
               mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "SOARXTerrain: Making LCC image for LCC type %s...", idxnum);
               osg::Vec3 selectedRGB((*l).rgb[0],(*l).rgb[1],(*l).rgb[2]);	
               LCCimage = MakeLCCImage(baseLCCColor.get(), selectedRGB);
               osgDB::writeImageFile(*(LCCimage.get()), LCCimagePath);
            }

            //dtCore::RefPtr<osg::Image> LCCfilter;

            mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "SOARXTerrain: Making LCC smoothed image for LCC type %s...", idxnum);
            osg::Vec3 filterRGB(0.0,0.0,0.0);					//select black
            dtCore::RefPtr<osg::Image> LCCfilter = ImageUtils::MakeFilteredImage((LCCimage.get()), filterRGB);
            osgDB::writeImageFile(*(LCCfilter.get()),LCCfilterPath);

            mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "LCCimage count = %i", LCCimage->referenceCount());
            LCCimage.release();
            LCCimage.~RefPtr();
            LCCimage = NULL;

            // Create a Mask Image for water
            // This uses LCC 11 (water) for the mask
            dtCore::RefPtr<osg::Image> MaskImage;
            std::string MaskPath = mCachePath + "/" + cellName + ".lcc.filter." + "11" + mImageExtension;

            if (osgDB::fileExists(MaskPath))
            {
               mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Reading the mask image");
               //								dtCore::RefPtr<osgDB::ImageOptions> options = new osgDB::ImageOptions;
               options->_destinationImageWindowMode = osgDB::ImageOptions::PIXEL_WINDOW;
               options->_destinationPixelWindow.set(0,0,mMaxTextureSize,mMaxTextureSize);
               osgDB::Registry::instance()->setOptions(options.get());
               MaskImage = osgDB::readImageFile(MaskPath);
            }
            else
            {
               // settings for water
               osg::Vec3 selectedRGB(110,130,177);	  
               MaskImage = MakeLCCImage(baseLCCColor.get(), selectedRGB);
               osg::Vec3 filterRGB(0.0,0.0,0.0);	  //select black
               MaskImage = ImageUtils::MakeFilteredImage((MaskImage.get()), filterRGB);               
               osgDB::writeImageFile(*(MaskImage.get()),MaskPath);
            }

            // Apply the mask to the image
            LCCfilter = ImageUtils::ApplyMask(LCCfilter.get(), MaskImage.get());

            mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "MaskImage count = %i", MaskImage->referenceCount());
            MaskImage.release();
            MaskImage.~RefPtr();
            MaskImage = NULL;

            LCCfilter->ensureValidSizeForTexturing(mMaxTextureSize);

            osgDB::writeImageFile(*(LCCfilter.get()),LCCfilterPath);

            mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "LCCfilter count = %i", LCCfilter->referenceCount());
            LCCfilter.release();
            LCCfilter.~RefPtr();
            LCCfilter = NULL;

            //	LCCfilter = osgDB::readImageFile(LCCfilterPath);
         }
      }
      // Grab the heightfield and continue processing LCC data
      if(hf != NULL)
      {
         mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "hf columns = %i  rows = %i", hf->getNumColumns(),hf->getNumRows());

         dtCore::RefPtr<osg::Image> HFimage;
         std::string HFimagePath = mCachePath + "/" + cellName + ".hf.image" + ".dds";

         // If the heightfield image exists read it from the osgDB
         //if(osgDB::fileExists(HFimagePath))
        // {
            //HFimage = osgDB::readImageFile(HFimagePath);
        // }
        // else
        // {
            HFimage = ImageUtils::MakeHeightMapImage(hf);
            //osgDB::writeImageFile(*(HFimage.get()),HFimagePath);
        // }

         dtCore::RefPtr<osg::Image> SLimage;
         std::string SLimagePath = mCachePath + "/" + cellName + ".sl.image" + mImageExtension;

         if(osgDB::fileExists(SLimagePath))
         {
            SLimage = osgDB::readImageFile(SLimagePath);
         }
         else
         {
            SLimage = ImageUtils::MakeSlopeAspectImage(hf,mMaxTextureSize);
            SLimage->ensureValidSizeForTexturing(mMaxTextureSize);
            osgDB::writeImageFile(*(SLimage.get()), SLimagePath);
         }

         // Do the following to check for it not to exist and then make the image in the bracket and saving.
         // Then reload the image within the iterator.

         dtCore::RefPtr<osg::Image> REimage;
         std::string REimagePath = mCachePath + "/" + cellName + ".re.image" + mImageExtension;

         if(osgDB::fileExists(REimagePath))
         {
            REimage = osgDB::readImageFile(REimagePath);
         }
         else
         {
            REimage = ImageUtils::MakeRelativeElevationImage(hf,mMaxTextureSize);
            REimage->ensureValidSizeForTexturing(mMaxTextureSize);
            osgDB::writeImageFile(*(REimage.get()), REimagePath);
         }

         for(std::vector<dtTerrain::LCCType>::iterator l = mLCCs.begin();
            l != mLCCs.end();
            l++)
         {
            int idx = (*l).GetIndex();
            char idxnum[3];

            sprintf(idxnum, "%i",idx);

            std::string CimagePath = mCachePath + "/" + cellName + ".c.image." + idxnum + mImageExtension;
            std::string LCCfilterPath = mCachePath + "/" + cellName + ".lcc.filter." + idxnum + mImageExtension;

            if(osgDB::fileExists(CimagePath))
            {
               mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "SOARXTerrain: Probability map for LCC type %i exists.",idx);
            }
            else
            {
               options->_destinationImageWindowMode = osgDB::ImageOptions::PIXEL_WINDOW;
               options->_destinationPixelWindow.set(0,0,mMaxTextureSize,mMaxTextureSize);
               osgDB::Registry::instance()->setOptions(options.get());

               dtCore::RefPtr<osg::Image> LCCfilter = osgDB::readImageFile(LCCfilterPath);
               mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "SOARXTerrain: Making probability map for LCC type %i.",idx);

               dtCore::RefPtr<osg::Image> mCimage = MakeCombinedImage((*l), LCCfilter.get(), HFimage.get(), SLimage.get(), REimage.get());
               mCimage->ensureValidSizeForTexturing(mMaxTextureSize);

               osgDB::writeImageFile(*(mCimage.get()), CimagePath);

               mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "LCCfilter count = %i", LCCfilter->referenceCount());
               LCCfilter.release();
               LCCfilter.~RefPtr();
               LCCfilter = NULL;

               mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "mCimage count = %i", mCimage->referenceCount());
               mCimage.release();
               mCimage.~RefPtr();
               mCimage = NULL;

            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> LCCAnalyzer::MakeBaseLCCColor(const osg::HeightField* hf,
      int latitude, int longitude, int mMaxTextureSize,
      ImageUtils::HeightColorMap mUpperHeightColorMap,
      ImageUtils::HeightColorMap mLowerHeightColorMap)
   {
      std::vector<GeospecificImage> lccimages;
      std::vector<GeospecificImage>::iterator it;
      std::vector<GeospecificImage> mGeospecificLCCImages;

      int width = hf->getNumColumns()-1,
         height = hf->getNumRows()-1;

      for(it = mGeospecificLCCImages.begin();
         it != mGeospecificLCCImages.end();
         it++)
      {
         if(latitude >= (*it).mMinLatitude && latitude <= (*it).mMaxLatitude &&
            longitude >= (*it).mMinLongitude && longitude <= (*it).mMaxLongitude)
         {
            lccimages.push_back(*it);

            width = osg::maximum(
               width,
               osg::Image::computeNearestPowerOfTwo(
               abs(1.0/(*it).mGeoTransform[1])
               )
               );

            height = osg::maximum(
               height,
               osg::Image::computeNearestPowerOfTwo(
               abs(1.0/(*it).mGeoTransform[5])
               )
               );
         }
      }

      width = mMaxTextureSize;
      height = mMaxTextureSize;

      dtCore::RefPtr<osg::Image> lccimage = new osg::Image;

      lccimage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      unsigned char* ptr = (unsigned char*)lccimage->data();

      float heightVal;
      osg::Vec3 c1, c2, c3, c4, c12, c34, color, coord;

      double latStep = 1.0/height, lonStep = 1.0/width,
         lat = latitude+latStep*0.5, lon,
         sStep = (hf->getNumColumns()-1.0)/width, tStep = (hf->getNumRows()-1.0)/height,
         s, t = tStep*0.5;

      for(int y=0;y<height;y++)
      {
         lon = longitude + lonStep*0.5;
         s = sStep*0.5;

         for(int x=0;x<width;x++)
         {
            heightVal = ImageUtils::GetInterpolatedHeight(hf, s, t);

            if(heightVal > 0.0f)
            {
               color = mUpperHeightColorMap.GetColor(heightVal);
            }
            else
            {
               color = mLowerHeightColorMap.GetColor(heightVal);
            }

            for(it = lccimages.begin();it != lccimages.end();it++)
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
                     color[0] *= (data[0]/255.0f);
                     color[1] *= (data[0]/255.0f);
                     color[2] *= (data[0]/255.0f);
                  }
                  else
                  {
                     color[0] = (data[0]/255.0f);
                     color[1] = (data[1]/255.0f);
                     color[2] = (data[2]/255.0f);
                  }
               }
            }

            *(ptr++) = (unsigned char)(color[0]*255);
            *(ptr++) = (unsigned char)(color[1]*255);
            *(ptr++) = (unsigned char)(color[2]*255);

            lon += lonStep;
            s += sStep;
         }

         lat += latStep;
         t += tStep;
      }
      return lccimage;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Image> LCCAnalyzer::MakeLCCImage(const osg::Image* src_image, const osg::Vec3& rgb_selected)
   {
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();
      dtCore::RefPtr<osg::Image> dst_image = new osg::Image();

      if(src_image->valid())
      {
         int width = src_image->s();
         int height = src_image->t();

         dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

         unsigned char* src_data = NULL;
         unsigned char* dst_data = NULL;

         mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "RGB selected for LCC = %3.0f, %3.0f, %3.0f", rgb_selected[0], rgb_selected[1], rgb_selected[2]);
         unsigned int hits =0;
         unsigned int misses =0;

         for(int y=0;y<=height;++y)
         {
            for(int x=0;x<=width;++x)
            {
               src_data = (unsigned char*)src_image->data(x,y);
               dst_data = (unsigned char*)dst_image->data(x,y);

               if ((src_data[0] == rgb_selected[0]) &&
                  (src_data[1] == rgb_selected[1]) &&
                  (src_data[2] == rgb_selected[2]))
               {

                  dst_data[0]=0.0;
                  dst_data[1]=0.0;
                  dst_data[2]=0.0;

                  hits++;
               }
               else
               {
                  dst_data[0]=255.0;
                  dst_data[1]=255.0;
                  dst_data[2]=255.0;

                  misses++;
               }
            }
         }
         mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "hits = %i", hits);
         mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "misses = %i", misses);
      }
      return dst_image;
   }
   //////////////////////////////////////////////////////////////////////////
   void LCCAnalyzer::LCCHistogram(const osg::Image* LCCbase, const osg::Image* image, char* filename, int binsize)
   {
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();

      //size of the image
      int endx = 1024;
      int endy = 1024;

      unsigned char* lcc_data = NULL;
      unsigned char* image_data = NULL;

      int width = image->s();
      int height = image->t();

      unsigned int hitbin[51];		//range is 5 for 51 bins
      unsigned int missbin[51];		//range is 5 for 51 bins

      unsigned int LCChits =0;
      unsigned int LCCmisses =0;

      unsigned int binnumber=0;
      unsigned int testnumber=0;

      for(int i=0;i<51;i++)
      {
         hitbin[i] = 0;
         missbin[i] = 0;
      }

      for(int y=0;y<height;y++)
      {
         for(int x=0;x<width;x++)
         {
            lcc_data = (unsigned char*)LCCbase->data(x,y);
            image_data = (unsigned char*)image->data(x,y);

            if (lcc_data[0] == 0) // a hit!
            {
               binnumber = int(image_data[0]/binsize);
               hitbin[binnumber]++;
               LCChits++;
            }
            else				// a miss!
            {
               binnumber = int(image_data[0]/binsize);
               missbin[binnumber]++;
               LCCmisses++;
            }
            if (testnumber<10)
               mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,
               "testnumber = %i, binnumber = %i", testnumber, binnumber);
            testnumber++;
         }
      }

      FILE *histofile = fopen(filename, "w");
      fprintf(histofile, "%s\n", filename);
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
   dtCore::RefPtr<osg::Image> LCCAnalyzer::MakeCombinedImage(
      dtTerrain::LCCType l,
      const osg::Image* f_image,			// filtered LCC image
      const osg::Image* h_image,			// heightmap
      const osg::Image* s_image,			// slopemap image
      const osg::Image* r_image)  		// relative elevation image
   {
      dtCore::RefPtr<dtUtil::Log> mLog = &dtUtil::Log::GetInstance();
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, "Making the Combined Image for %i", l.GetIndex());

      if (f_image == NULL)
         mLog->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, "missing filter image" );

      int max_height = int(l.GetMaxElevation()/10.0f);
      int min_height = int(l.GetMinElevation()/10.0f);
      int max_slope = int((l.GetMaxSlope()/90.0f) * 255.0f);	


      unsigned char* f_data = NULL;
      unsigned short* h_data = NULL;
      unsigned char* s_data = NULL;
      unsigned char* r_data = NULL;

      int im_width = f_image->s();
      int im_height = f_image->t();

      int hf_width = h_image->s();
      int hf_height = h_image->t();

      int scale = im_width/hf_width;

      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, "scale = %i,  image width = %i,  hf width = %i", scale, im_width, hf_width);

      dtCore::RefPtr<osg::Image> dst_image = new osg::Image;
      dst_image->allocateImage(im_width, im_height, 1, GL_RGB, GL_UNSIGNED_BYTE);

      unsigned char* dst_data = NULL;

      float value = 0;
      float height_value=0;
      float slope_value=0;
      float aspect_value=0;
      float relel_value=0;

      for(int y=0;y<im_height;y++)
      {
         for(int x=0;x<im_width;x++)
         {
            dst_data = (unsigned char*)dst_image->data(x,y);
            f_data = (unsigned char*)f_image->data(x,y);
            h_data = (unsigned short*)h_image->data(int(x/scale), int(y/scale));
            s_data = (unsigned char*)s_image->data(int(x/scale), int(y/scale));
            r_data = (unsigned char*)r_image->data(int(x/scale), int(y/scale));

            value = f_data[0];									// start with filter data as basis

            relel_value  = r_data[0];
            slope_value  = s_data[1];
            aspect_value = s_data[2];
            height_value = (float)*h_data / 65536.0f * 255.0f;

            if (value <= 254 )								// nonwhite -> has vegetation possibility
            {
               if (height_value <= min_height)				// busted height limits (do this as a curve)
                  value = 999;
               if (height_value >= max_height)				// busted height limits (do this as a curve)
                  value = 998;
               if (slope_value > max_slope)				// busted slope limit  (do this as a curve)
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
                  value = value + (slope_value/max_slope)*100.0f;		//greater slope is unfavorable (linear)
               }
            }

            if (value == 999)							//below min elevation
            {
               dst_data[0] = (unsigned char) 0.0;
               dst_data[1] = (unsigned char) 0.0;
               dst_data[2] = (unsigned char) 255.0;
            }
            else if (value == 998)						//above max elevation
            {
               dst_data[0] = (unsigned char) 0.0;
               dst_data[1] = (unsigned char) 255.0;
               dst_data[2] = (unsigned char) 0.0;
            }
            else if (value == 997)						//slope too great
            {
               dst_data[0] = (unsigned char) 0.0;
               dst_data[1] = (unsigned char) 128.0;
               dst_data[2] = (unsigned char) 255.0;
            }
            else
            {
               dst_data[0] = (unsigned char)osg::clampTo(value,0.0f,255.0f);  //store aspect
               dst_data[1] = (unsigned char)osg::clampTo(value,0.0f,255.0f);  //store probability
               dst_data[2] = (unsigned char)osg::clampTo(value,0.0f,255.0f);  //store probability
            }
         }
      }

      f_data = NULL;
      h_data = NULL;
      s_data = NULL;
      r_data = NULL;

      return dst_image;
   }
}

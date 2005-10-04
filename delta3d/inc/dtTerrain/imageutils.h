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
#ifndef _IMAGEUTILS_H
#define _IMAGEUTILS_H

#include <osg/Vec3>
#include <osg/Texture2D>
#include <osgDB/FileUtils>
#include <dtCore/refptr.h>
#include <dtCore/physical.h>
//#include <ogrsf_frmts.h>
#include "dtTerrain/terrain_export.h"

namespace dtTerrain
{
   class DT_TERRAIN_EXPORT ImageUtils
   {
   public:
   
      struct GeospecificImage
      {
         dtCore::RefPtr<osg::Image> mImage;

         std::string mFilename;

         int mMinLatitude;
         int mMaxLatitude;
         int mMinLongitude;
         int mMaxLongitude;

         double mGeoTransform[6];
         double mInverseGeoTransform[6];
      };
      
      /**
      * Maps height values to colors with interpolation/extrapolation.
      */
      struct DT_TERRAIN_EXPORT HeightColorMap : public std::map<float, osg::Vec3>
      {
         /**
         * Gets the color corresponding to the specified height.
         *
         * @param height the height value to map
         * @return the corresponding color
         */
         osg::Vec3 GetColor(float height);
      };
     
      /**
      * Logs information about the given image. (LogLevel is INFO).
      * @param image the handle to the image you want to examine
      * @param imagename a descriptive name to call this image
      */        
      static void ImageStats(const osg::Image* image, std::string* imagename);
      
      /**
       * @param width The target width of the created image.
       * @param eight The target height of the created image.
       */
      static dtCore::RefPtr<osg::Image> CreateDetailMap(unsigned int width, unsigned int height);
      
      /**
       * Calculates a noise value for a given x,y pair.
       * @param x The x value.
       * @param y The y value.
       */
      static unsigned short CalculateDetailNoise(int x, int y);
      
      /**
       * @param srcImage An 16-bit single channel image representing a heightmap.
       * @param scale The scale value to apply to the resulting gradient calculations.
       */
      static dtCore::RefPtr<osg::Image> CreateBaseGradientMap(const osg::Image *srcImage,
         float scale);
         
      
      static dtCore::RefPtr<osg::Image> CreateDetailGradientMap(unsigned int width,
         unsigned int height, float scale);
         
         
      static unsigned short CalculateScaleMapNoise(int x, int y);
      
      
      static dtCore::RefPtr<osg::Image> CreateDetailScaleMap(unsigned int width,
         unsigned int height);

      /**
       * Gets the interpolated height
       */
      static float GetInterpolatedHeight(const osg::HeightField* hf, double x, double y);
      
      /**
       * Converts an OSG heightfield into a valid image.
       * @param hf The OSG heightfield.
       * @return The newly created image.
       * @note The format of the image is a 16-bit single channel map.  This
       *    maps to the format: (GL_LUMINANCE, GL_UNSIGNED_SHORT).
      */
      static dtCore::RefPtr<osg::Image> MakeHeightMapImage(const osg::HeightField* hf);

      /**
      * Create smoothed grayscale map of the terrain by LCC type
      * Uses weighted next nearest neighbor to "fuzzy"-up the LCCImage data
      * @param src_image the black/white LCC Image by LCC type
      * @param rgb_selected the RGB color of the points to smooth (always 0,0,0 - black)
      * @return the newly created image
      */
      static dtCore::RefPtr<osg::Image> MakeFilteredImage(const osg::Image* src_image, const osg::Vec3& rgb_selected);

      /**
      * Use an image to mask-out vegetation probability (set probability to 0%)
      * Useful for masking-out bodies of water or user-created urban models
      * @param src_image the black/white LCC Image by LCC type
      * @param mask_image the black/white - using LCC type 11 as default
      * @return the modified filtered image
      */
      static dtCore::RefPtr<osg::Image> ApplyMask(const osg::Image* src_image, const osg::Image* mask_image);

      /**
      * Create slopemap from GDAL-derived heightfield data
      * @param hf the GDAL-derived heightfield
      * @return the newly created image
      */
      static dtCore::RefPtr<osg::Image> MakeSlopeAspectImage(const osg::HeightField* hf, int maxTextureSize);

      /**
      * Create relative elevation map from GDAL-derived heightfield data
      * @param hf the GDAL-derived heightfield
      * @return the newly created image
      */
      static dtCore::RefPtr<osg::Image> MakeRelativeElevationImage(const osg::HeightField* hf, int maxTextureSize);
   };
   
}

#endif

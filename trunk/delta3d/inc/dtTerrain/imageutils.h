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
#ifndef DELTA_IMAGEUTILS
#define DELTA_IMAGEUTILS

#include <osg/Vec3>
#include <dtCore/refptr.h>
#include <dtTerrain/heightfield.h>
#include <dtTerrain/terrain_export.h>
#include <map>

namespace dtTerrain
{
   class InvalidImageDimensionsException : public dtUtil::Exception
   {
   public:
   	InvalidImageDimensionsException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidImageDimensionsException() {};
   };
   
   class InvalidRasterFormatException : public dtUtil::Exception
   {
   public:
   	InvalidRasterFormatException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidRasterFormatException() {};
   };
   
   class LoadFailedException : public dtUtil::Exception
   {
   public:
   	LoadFailedException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~LoadFailedException() {};
   };
   
   
   /**
    * This class is a static class containing many methods for creating images
    * procedurally, concatenating images, and building images from source height
    * data.
    */
   class DT_TERRAIN_EXPORT ImageUtils
   {
   public:
   
      struct GeospecificImage
      {
         dtCore::RefPtr<osg::Image> mImage;
         std::string mFileName;

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
         osg::Vec3 GetColor(float height) const;
      };
     
      /**
      * Logs information about the given image. (LogLevel is INFO).
      * @param image the handle to the image you want to examine
      * @param imagename a descriptive name to call this image
      */        
      static void ImageStats(const osg::Image* image, std::string* imagename);
      
      /**
       *
       */
      static void LoadGeospecificLCCImage(ImageUtils::GeospecificImage &gslcc);
      
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
       * Generates a gradient map based on the given heightfield.
       * @param hf The source heightfield.
       * @param scale The scale value to apply to the resulting gradient calculations.
       * @return An RGB image encoded with gradient values cooresponding to the 
       *    specified heightfield.
       */
      static dtCore::RefPtr<osg::Image> CreateBaseGradientMap(const HeightField &hf,
         float scale);
         
      /**
       * Generates a gradient map based on perlin noise.  This can be used to 
       * procedurally add lighting detail to surfaces.
       * @param width The width of the image.
       * @param height The height of the image.
       * @param float Scale value with with to scale the resulting gradient values.
       * @return Image containing the gradient map.
       */
      static dtCore::RefPtr<osg::Image> CreateDetailGradientMap(unsigned int width,
         unsigned int height, float scale);
         
      /**
       *
       */
      static unsigned short CalculateScaleMapNoise(int x, int y);
      
      /**
       * Creates a perlin noise map which is useful for adding perturbing existing
       * noise values in a random fashion.
       * @param width Width of the image.
       * @param height Height of the image.
       * @return An image containing perlin noise based scale values.
       */
      static dtCore::RefPtr<osg::Image> CreateDetailScaleMap(unsigned int width,
         unsigned int height);

      /**
       * Create smoothed grayscale map of the terrain by LCC type
       * Uses weighted next nearest neighbor to "fuzzy"-up the LCCImage data
       * @param src_image the black/white LCC Image by LCC type
       * @param rgb_selected the RGB color of the points to smooth (always 0,0,0 - black)
       * @return the newly created image
       */
      static dtCore::RefPtr<osg::Image> MakeFilteredImage(const osg::Image &src_image, 
         const osg::Vec3& rgb_selected);

      /**
      * Use an image to mask-out vegetation probability (set probability to 0%)
      * Useful for masking-out bodies of water or user-created urban models
      * @param src_image the black/white LCC Image by LCC type
      * @param mask_image the black/white - using LCC type 11 as default
      * @return the modified filtered image
      */
      static dtCore::RefPtr<osg::Image> ApplyMask(const osg::Image &src_image, const osg::Image &mask_image);

      /**
      * Create a slope map.
      * @param hf The heightfield heightfield
      * @return the newly created image
      */
      static dtCore::RefPtr<osg::Image> MakeSlopeAspectImage(const HeightField &hf);

      /**
      * Creates a relative elevation map.
      * @param hf The heightfield with which to build the elevation map.
      * @param scale The amount to scale the resulting values by.
      * @return the newly created image
      */
      static dtCore::RefPtr<osg::Image> MakeRelativeElevationImage(const HeightField &hf,
         float scale);

      /**
       * Generates an image which maps the height value in the heightfield to
       * a color value given by one of the two color maps.
       * @param hf The heightfield with which to map colors.
       * @param upperHeightColorMap The color values to map the height values which
       *    are above 0 (sea-level).
       * @param lowerHeightColorMap The color values to map the height values which are
       *    are below or equal to zero.
       * @param gamma Gamma correction value.  Greater than 1.0 to brighten, less than
       *    1.0 to darken.
       */
      static dtCore::RefPtr<osg::Image> MakeBaseColor(const HeightField &hf, 
         const ImageUtils::HeightColorMap &upperHeightColorMap,
         const ImageUtils::HeightColorMap &lowerHeightColorMap, float gamma=1.0f);

      /**
      * Nearest neighbor geometric image manipulation. This assumes that the original image
      * is nxn square.
      * @param srcImage The image to scale.
      * @return Destination image with the correct power of 2 dimensions.
      */
      static dtCore::RefPtr<osg::Image> EnsurePow2Image(const osg::Image *srcImage);
   };
   
}

#endif

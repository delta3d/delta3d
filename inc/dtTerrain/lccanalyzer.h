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

#ifndef _LCCANALYZER_H
#define _LCCANALYZER_H

#include <string>
#include <osg/Vec3>
#include <osg/Image>
#include "dtTerrain/terrain_export.h"
#include "dtTerrain/imageutils.h"
#include "dtTerrain/lcctype.h"
#include "dtTerrain/pagedterraintile.h"

namespace dtTerrain
{

   /**
    * This enumeration identitifies the resources that are cached and managed by the
    * vegetation decorator layer.
    */
   class DT_TERRAIN_EXPORT LCCAnalyzerResourceName : public dtUtil::Enumeration
   {
      DECLARE_ENUM(LCCAnalyzerResourceName);
      public:

         static const LCCAnalyzerResourceName IMAGE_EXT;
         static const LCCAnalyzerResourceName BASE_LCC_COLOR;
         static const LCCAnalyzerResourceName WATER_MASK;
         static const LCCAnalyzerResourceName BASE_COLOR;
         static const LCCAnalyzerResourceName BASE_FILTER_NAME;
         static const LCCAnalyzerResourceName LCC_IMAGE_NAME;
         static const LCCAnalyzerResourceName SLOPE_IMAGE;
         static const LCCAnalyzerResourceName REL_ELEV_IMAGE;
         static const LCCAnalyzerResourceName COMPOSITE_LCC_IMAGE;
         static const LCCAnalyzerResourceName SCENE_GRAPH;

      protected:
         LCCAnalyzerResourceName(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

   class LCCInvalidCacheException : public dtUtil::Exception
   {
   public:
   	LCCInvalidCacheException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~LCCInvalidCacheException() {};
   };

   class LCCNoValidGeoImagesException : public dtUtil::Exception
   {
   public:
   	LCCNoValidGeoImagesException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~LCCNoValidGeoImagesException() {};
   };
   

   /**
    * The LCC Analyzer calculates LCC data for use in the
    * vegetation decorator.
   */
   class DT_TERRAIN_EXPORT LCCAnalyzer
   {
   public:

      /**
       * Constructor
       */
      LCCAnalyzer();

      /**
       * Destructor
       */
      virtual ~LCCAnalyzer();

      /**
       * Makes the base color texture map for the specified heightfield.
       * @param hf the heightfield to process
       * @param latitude the latitude of the terrain segment
       * @param longitude the longitude of the terrain segment
       * @return the newly created image
       */
      dtCore::RefPtr<osg::Image> MakeBaseLCCColor(const HeightField &hf, int latitude,
         int longitude);

      /**
       * Creates a mask image based on this LCC type where a hit corresponds to
       * a black pixel and white corresponds to a miss.
       * @param src_image Image containing LCC coloration data.
       * @param r Red pixel color value with which to create the mask.
       * @param g Green pixel color value with which to create the mask.
       * @param b Blue pixel color value with which to create the mask.
       * @return The mask image.
       */
      dtCore::RefPtr<osg::Image> MakeLCCMask(const osg::Image &src_image,
         unsigned char r, unsigned char g, unsigned char b);

      /**
       * Buggy "histogram" of an image by a particular LCC type.
       *
       * @param LCCbase the black/white LCC image of picked points of a particular LCC type
       * @param image the slopemap, heightmap, or relative elevation
       * @param fileName the filename to save the histogram data
       * @param binsize the sampling size of the image (i.e. the delta height or slope).
       */
      void LCCHistogram(const osg::Image &LCCbase, const osg::Image &image,
         const std::string &fileName, int binsize);

      /**
       * Create probability map of the likehihood for a particular LCC type
       * @param l The LCC type to calculate probabilities for.
       * @param hf The heightfield.
       * @param f_image The masked LCC image containing placement statistics.
       * @param s_image the slopemap image
       * @param r_image the relative elevation image
       * @return the newly created image
      */
      dtCore::RefPtr<osg::Image> MakeCombinedImage(
         dtTerrain::LCCType &l, const HeightField &hf, const osg::Image &f_image,
         const osg::Image &s_image, const osg::Image &r_image);

      dtCore::RefPtr<osg::Image> GenerateBaseFilterImage(LCCType &type,
         const std::string &tileCachePath);

      bool CheckBaseLCCImages(const HeightField &hf, int latitude, int longitude,
         const std::string &tileCachePath);

      void CheckSlopeAndElevationMaps(const HeightField &hf,
         const std::string &tileCachePath);

      bool ProcessLCCData(const PagedTerrainTile &tile, LCCType &type);

      void ComputeProbabilityMap(const HeightField &hf, LCCType &type,
         int latitude, int longitude, const std::string &tileCachePath);

      /**
       * Adds a geospecific image to the LCC analyzers list of geo images.
       * There must be at least one valid LCC geospecific image in this
       * list, else an exception will be thrown while processing LCC types.
       * @param filename The name of the geospecific resource.
       */
      void AddGeospecificImage(const std::string &fileName)
      {
         ImageUtils::GeospecificImage newImage;
         newImage.mFileName = fileName;
         mGeospecificLCCImages.push_back(newImage);
      }

      /**
       * Checks the list of currently loaded images to see if the specified
       * image has already been loaded and processed.
       * @param fileName The filename of the image.
       * @return True if already loaded, false otherwise.
       */
      bool IsGeoSpecificLCCImageLoaded(const std::string &fileName);

      /**
       * Makes sure all the geospecific images that have been added to the
       * analyzer are loaded and ready for use in the LCC data processing.
       * @note If an image has already been loaded previously, it will not
       *    be reloaded.
       */
      void LoadAllGeoSpecificImages();

      /**
       * Gets whether or not inter-process images processed during
       * LCC analysis are saved for review.
       * @return True if images are saved, false otherwise.
       */
      bool OutputDebugImages() { return mOutputDebugImages; }

      /**
       * Sets whether or not inter-process images processed during
       * LCC analysis are saved for review.
       * @param value True if images should be saved, false otherwise.
       */
      void SetOutputDebugImages(bool value) { mOutputDebugImages = value; }

      /**
       * Sets the maximum size of the images constructed during LCC analysis.
       * @param newSize The size in pixels of the image.
       * @note The default is 1024.  Increasing this value will result
       *    in more accurate vegetation computations; however, will dramatically
       *    increase the time taken to generate the necessary data.
       */
      void SetMaxImageSize(unsigned int newSize) { mImageSize = newSize; }

      /**
       * Gets the current maximum image size used during LCC analysis.
       * @return The current image size.
       */
      unsigned int GetImageSize() const { return mImageSize; }

      /**
       * Clears all data cached in this analyzer.  This should be called before
       * processing a different tile or region.
       */
      void Clear()
      {
         mBaseLCCColorImage = NULL;
         mSlopeMap = NULL;
         mRelativeElevationMap = NULL;
         mWaterMask = NULL;
      }

   private:
      dtCore::RefPtr<osg::Image> mBaseLCCColorImage;
      dtCore::RefPtr<osg::Image> mSlopeMap;
      dtCore::RefPtr<osg::Image> mRelativeElevationMap;
      dtCore::RefPtr<osg::Image> mWaterMask;
      std::vector<ImageUtils::GeospecificImage> mGeospecificLCCImages;

      unsigned int mImageSize;
      bool mOutputDebugImages;
   };
}
#endif

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

#ifndef _LCCANALYZER_H
#define _LCCANALYZER_H

#include <osg/Vec3>
#include "dtTerrain/terrain_export.h"
#include "dtTerrain/imageutils.h"
#include "dtTerrain/lcctype.h"

namespace dtTerrain
{

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
      *
      * @param hf the heightfield to process
      * @param latitude the latitude of the terrain segment
      * @param longitude the longitude of the terrain segment
      * @return the newly created image
      */
      dtCore::RefPtr<osg::Image> MakeBaseLCCColor(const osg::HeightField* hf, int latitude, 
         int longitude, int mMaxTextureSize,
         ImageUtils::HeightColorMap mUpperHeightColorMap,
         ImageUtils::HeightColorMap mLowerHeightColorMap);

      /**
      * Create hit/miss map of the terrain by LCC type
      * @param src_image the Base Color image with LCC encoded within
      * @param rgb_selected the RGB color of the LCC type selected
      * @return the newly created image
      */
      dtCore::RefPtr<osg::Image> MakeLCCImage(const osg::Image* src_image, const osg::Vec3& rgb_selected);

		struct LCCCells
		{
		   /**
			* The top node of the vegetation scene graph.
			*/
			dtCore::RefPtr<osg::Group> mRootVegeGroup;
		};

      /**
      * A geospecific image.
      */
      struct GeospecificImage
      {
         dtCore::RefPtr<osg::Image> mImage;

         std::string mFilename;

         int mMinLatitude, mMaxLatitude, mMinLongitude, mMaxLongitude;

         double mGeoTransform[6];
         double mInverseGeoTransform[6];
      };

      /**
      * Buggy "histogram" of an image by a particular LCC type.
      *
      * @param LCCbase the black/white LCC image of picked points of a particular LCC type
      * @param image the slopemap, heightmap, or relative elevation
      * @param filename the filename to save the histogram data
      * @param binsize the sampling size of the image (i.e. the delta height or slope).
      */
      void LCCHistogram(const osg::Image* LCCbase, const osg::Image* image, char* filename, int binsize);

      /**
      * Create probability map of the likehihood for a particular LCC type
      * @param LCCidx LCC image index
      * @param h_image the heightmap image
      * @param s_image the slopemap image
      * @param r_image the relative elevation image
      * @return the newly created image
      */
      dtCore::RefPtr<osg::Image> MakeCombinedImage(
         dtTerrain::LCCType l,
         const osg::Image* f_image,			// LCC filtered image
         const osg::Image* h_image,			// heightmap
         const osg::Image* s_image,			// slopemap image
         const osg::Image* r_image);		    // relative elevation image

      void LoadLCCData(const osg::HeightField* hf,unsigned int latitude, unsigned int longitude, int mMaxTextureSize, char* cellName);

      /**
      * Sets the LCCTypes 
      * @param Vector of LCCTypes
      */
      void SetLCCData(std::vector<dtTerrain::LCCType> LCCtypes)
      {
         this->mLCCs = LCCtypes;
      }

      ImageUtils::HeightColorMap mUpperHeightColorMap;
      ImageUtils::HeightColorMap mLowerHeightColorMap;
      std::string mImageExtension;
      std::string mCachePath;
      std::vector<dtTerrain::LCCType> mLCCs;
      
   };
}
#endif

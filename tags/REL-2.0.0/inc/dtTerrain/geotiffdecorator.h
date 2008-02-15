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
#ifndef DELTA_GEOTIFFDECORATOR
#define DELTA_GEOTIFFDECORATOR

#include <vector>
#include <osg/Image>
#include "dtTerrain/imageutils.h"
#include "dtTerrain/terraindecorationlayer.h"
#include "dtTerrain/terrain_export.h"

namespace dtTerrain
{
   
   /**
    * This class is a terrain texturing decorator that supports the mapping of
    * geospecific images onto terrain tiles.  A user may add as many geospecific
    * images as they wish and the appropriate image region will be loaded and 
    * assigned to the terrain tile as needed.
    */
   class DT_TERRAIN_EXPORT GeoTiffDecorator : public TerrainDecorationLayer
   {
      public:
      
         /**
          * Constructs the decoration layer.
          */
         GeoTiffDecorator(const std::string &name="GeoTiffDecorator");
           
         /**
          * Based on the currently registered geo tiff images, this method
          * will generate a base texture and assign it to the tile.
          */
         virtual void OnLoadTerrainTile(PagedTerrainTile &tile);
         
         /**
          *  Since this decorator does not add any geometry to the terrain,
          *  just return NULL here.
          */
         virtual osg::Node *GetOSGNode() { return NULL; }
         
         /**
          * Adds a new geo tiff image to the decorator.
          */
         void AddGeoSpecificImage(const std::string &fileName)
         {
            ImageUtils::GeospecificImage newImage;
            newImage.mFileName = fileName;
            mImageList.push_back(newImage);
         }
         
         /**
          * Loads any images in the image list.
          */
         void LoadAllGeoSpecificImages();
         
         /**
          * Calculates an image based on the curret geospecific image set.
          * @param lat The latitude to build an image for.
          * @param lon The longitude to build an image for.
          */
         osg::Image *CalculateBaseImage(int lat, int lon);
         
         /**
          * Sets the image dimensions for the resulting terrain tile base
          * texture.
          */
         void SetResultingImageDimensions(unsigned int w, unsigned int h)
         {
            mResultImageWidth = w;
            mResultImageHeight = h;
         }
         
      protected:
      
         virtual ~GeoTiffDecorator() { }
         
      private:
         
         std::vector<ImageUtils::GeospecificImage> mImageList;
         unsigned int mResultImageWidth;
         unsigned int mResultImageHeight;
   };
   
   
   
}

#endif 

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

#include <dtTerrain/colormapdecorator.h>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

namespace dtTerrain
{
   const std::string COLOR_MAP_IMAGE_NAME = "colormap_decorator_image.rgb";
   
   //////////////////////////////////////////////////////////////////////////
   ColorMapDecorator::ColorMapDecorator(const std::string &name) : TerrainDecorationLayer(name)
   {
      mLowerColorMap[-11000.0f].set(0,0,0);
      mLowerColorMap[0.0f].set(0.09f,0.09f,0.501f);

      //Water coloration...
      mLowerColorMap[0.0f].set(0.09f,0.09f,0.501f);
      mUpperColorMap[10.0f].set(0.274f,0.305f,0.549f);
      
      //Shoreline and coastal coloration... 
      mUpperColorMap[15.0f].set(0.4f,0.4f,0.4f);
      mUpperColorMap[35.0f].set(0.60f,0.52f,0.08f);
      
      mUpperColorMap[75.0f].set(0.61f,0.82f,0.43f);
      mUpperColorMap[150.0f].set(0.25f,0.7f,0.25f);
            
      mUpperColorMap[300.0f].set(0.710f,0.647f,0.388f);
      mUpperColorMap[600.0f].set(0.741f,0.741f,0.482f);
      mUpperColorMap[1200.0f].set(0.290f,0.612f,0.290f);
      mUpperColorMap[2000.0f].set(0.482f,0.741f,0.322f);
      mUpperColorMap[2500.0f].set(0.647f,0.809f,0.518f);
      mUpperColorMap[3000.0f].set(1,1,1);
      mUpperColorMap[9000.0f].set(1,1,1);
   }
   
   //////////////////////////////////////////////////////////////////////////
   void ColorMapDecorator::OnLoadTerrainTile(PagedTerrainTile &tile)
   {
      dtCore::RefPtr<osg::Image> image;
      
      if (tile.IsCachingEnabled())
      {
         std::string imagePath = tile.GetCachePath() + "/" + COLOR_MAP_IMAGE_NAME;         
         if (dtUtil::FileUtils::GetInstance().FileExists(imagePath))
         {
            LOG_INFO("Reading colormap image from cache: " + imagePath);
            image = osgDB::readImageFile(imagePath);
            tile.SetBaseTextureImage(image.get());
         }
         else
         {
            LOG_INFO("Generating colormap image.");
            image = ImageUtils::MakeBaseColor(*tile.GetHeightField(),mUpperColorMap,
               mLowerColorMap);
            tile.SetBaseTextureImage(image.get());
            
            LOG_INFO("Caching colormap image to cache: " + imagePath);
            if (tile.GetBaseTextureImage() != NULL)
               osgDB::writeImageFile(*image,imagePath);
         }
      }
      else
      {
         LOG_INFO("Generating colormap image.");
         image = ImageUtils::MakeBaseColor(*tile.GetHeightField(),mUpperColorMap,
            mLowerColorMap);
         tile.SetBaseTextureImage(image.get());
      }
   }
   
}

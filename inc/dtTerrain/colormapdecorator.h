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
#ifndef DELTA_COLORMAPDECORATOR
#define DELTA_COLORMAPDECORATOR

#include "dtTerrain/terraindecorationlayer.h"
#include "dtTerrain/imageutils.h"
#include "dtTerrain/terrain_export.h"

namespace dtTerrain
{
   
   /**
    * This terrain decorator is a terrain tile base texture decorator.  It calculates
    * a texture map for the tile based on its height field.  Color values may be mapped
    * to specific height values.  All color values falling between these posts are 
    * interpolated forming a smooth gradient.
    */
   class DT_TERRAIN_EXPORT ColorMapDecorator : public TerrainDecorationLayer
   {
      public:
      
         /**
          * Constructs the decoration layer.
          */
         ColorMapDecorator(const std::string &name="ColorMapDecorator");
           
         /**
          * Generates a base texture map for this tile using the current
          * height-color mapping assigned to this decorator.
          * @param tile The tile with which to generate the base texture.
          */
         virtual void OnLoadTerrainTile(PagedTerrainTile &tile);
         
         /**
          *  Since this decorator does not add any geometry to the terrain,
          *  just return NULL here.
          */
         virtual osg::Node *GetOSGNode() { return NULL; }
         
      private:
         ImageUtils::HeightColorMap mUpperColorMap;
         ImageUtils::HeightColorMap mLowerColorMap;
   };

}

#endif 

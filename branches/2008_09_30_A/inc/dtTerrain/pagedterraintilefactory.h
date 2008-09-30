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
#ifndef DELTA_PAGEDTERRAINTILEFACTORY
#define DELTA_PAGEDTERRAINTILEFACTORY

#include <osg/Referenced>

#include "dtTerrain/terrain_export.h"
#include "dtTerrain/geocoordinates.h"
#include "dtTerrain/pagedterraintile.h"

namespace dtTerrain
{
 
   /**
    * This class is the base for a simple factory used to create new terrain tiles.  
    * The terrain relies on this factory when it needs to create a new tile.
    * Specific applications which rely on custom terrain tiles may wish to build
    * a custom factory for the terrain to use.
    */  
   class DT_TERRAIN_EXPORT PagedTerrainTileFactory : public osg::Referenced
   {
      public:
      
         ///Empty constructor...
         PagedTerrainTileFactory() { }
         
         /**
          * This method creates a new terrain tile.  The default implementation
          * merely creates a new tile and assigns its coordinates.
          * @param coords The coordinates mapped to this tile's location.
          * @param owner The terrain instance that owns this tile.
          * @return A newly created tile.
          */
         virtual PagedTerrainTile *CreateNewTile(const GeoCoordinates &coords, Terrain &owner)
         {
            PagedTerrainTile *newTile = new PagedTerrainTile(&owner);
            newTile->SetGeoCoordinates(coords);
            return newTile;
         }
         
      protected:
      
         ///Empty destructor...
         virtual ~PagedTerrainTileFactory() { }         
   };
   
}


#endif

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

#ifndef _TERRAINTILE_H
#define _TERRAINTILE_H

#include "dtTerrain/terrain_export.h"

namespace dtTerrain 
{
   /**
   * This class holds simple properties that contain information related
   * to a tile of terrain at a geospecific location. 
   */
   class DT_TERRAIN_EXPORT TerrainTile
   {

   public:
      /**
      * Constructor
      */
      TerrainTile();

      /**
      * Destructor
      */
      virtual ~TerrainTile();

      /**
      * Less than operator to compare a specific tiles latitude and longitude to the current tile
      * @param TerrainTile to compare
      * @return boolean if the latitude is less or false if it's greater.
      */
      bool operator < (TerrainTile& tile)
      {
         if(getLatitude() < tile.getLatitude())
         {
            return true;
         }
         else if(getLatitude() > tile.getLatitude())
         {
            return false;
         }
         else
         {
            return (getLongitude() < tile.getLongitude());
         }
      }

      /**
      * This returns the tiles latitude
      * @return integer of the currently set Latitude
      */
      int getLatitude()
      {
         return mLatitude;
      }

      /**
      * This sets the tiles latitude
      * @param integer of the latitude
      */
      void setLatitude(unsigned int latitude)
      {
         mLatitude = latitude;
      }

      /**
      * This returns the tiles longitude
      * @return integer of the currently set Longitude
      */
      int getLongitude()
      {
         return mLongitude;
      }

      /**
      * This sets the longitude of the tile
      * @param integer of the longitude
      */
      void setLongitude(unsigned int longitude)
      {
         mLongitude = longitude;
      }

   private:
      unsigned int mLatitude;
      unsigned int mLongitude;
   };
}
#endif

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
#ifndef DELTA_DTEDTERRAINREADER
#define DELTA_DTEDTERRAINREADER

#include <osg/Shape>

#include "dtTerrain/terraindatareader.h"
#include "dtTerrain/heightfield.h"

namespace dtTerrain 
{

   /**
    * This data reader loads DTED data mapping to a specified latitude and
    * longitude.  Under the covers this reader uses GDAL to load the actual
    * heightfield values.  Note, to load specific DTED levels, one must
    * specify the level to load.  By default, the reader will attempt to load
    * DTED level 1.
    */
   class DT_TERRAIN_EXPORT DTEDTerrainReader : public TerrainDataReader 
   {
      public:
   
         /**
          * This enumeration represents the different DTED levels available for
          * loading.
          */
         class DT_TERRAIN_EXPORT DTEDLevelEnum : public dtUtil::Enumeration 
         {
            DECLARE_ENUM(DTEDLevelEnum);
            public:
            
               ///DTED data with 1km resolution data posts.
               static const DTEDLevelEnum ZERO; 
               
               ///DTED data with 100m resolution data posts.
               static const DTEDLevelEnum ONE;
               
               ///DTED data with 30m resolution data posts.
               static const DTEDLevelEnum TWO;
               
               ///DTED data with 10m resolution data posts.
               static const DTEDLevelEnum THREE;
               
               ///DTED data with 3m resolution data posts.
               static const DTEDLevelEnum FOUR; 
               
               ///DTED data with 1m resolution data posts.
               static const DTEDLevelEnum FIVE;
               
               ///Gets the integer representation of this enumeration.
               int GetNumeral() const { return mNumeral; }
               
            private:
               
               ///Simple enumeration constructor.
               DTEDLevelEnum(const std::string &name, int num) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
                  mNumeral = num;
               }
               
               int mNumeral;
         };
       
         /**
          * Constructs the terrain reader.
          * @param level The level of DTED data to load.  
          */
         DTEDTerrainReader(const DTEDLevelEnum &level = DTEDLevelEnum::ONE,
            const std::string &name="DTEDReader");
            
         /**
          * Gets the type of terrain data this reader supports.
          * @return TerrainDataType::DTED.
          */
         const TerrainDataType &GetDataType() const { return TerrainDataType::DTED; }
         
         /**
          * Called by the terrain when a tile needs to be loaded.  This method
          * will first check the tile's cache for the heightfield.  If it is not
          * found, this method will load the DTED data for the specified tile.
          * @param tile The tile to load.  Based on its latitude and longitude
          *    position and the reader's max DTED level, the appropriate data
          *    will be loaded.
          * @note If any errors occur during the load process, the appropriate
          *    exception is thrown.
          */
         virtual bool OnLoadTerrainTile(PagedTerrainTile &tile); 
         
         /**
          * This generates the cache path for the specified tile.  The cache path
          * is equal to: $Latitude_$Longitude_$DTEDLevel.
          * @param tile The tile with which to generate the path for.
          * @return A string containing the generated cache path.
          */
         const std::string GenerateTerrainTileCachePath(
            const PagedTerrainTile &tile);       
         
         /**
          * Sets the DTED level to load.
          * @param level The level of data to load.  By default, this value
          *    is set to DTEDLevelEnum::ONE.
          */
         void SetDTEDLevel(const DTEDLevelEnum &level);
         
         /**
          * Gets the current DTED level this reader is trying to load.
          * @return An enumeration corresponding to appropriate DTED level.
          */
         const DTEDLevelEnum &GetDTEDLevel() const
         {
            return *mDTEDLevel;
         }         
        
      protected:
      
         ///Destroys the terrain reader.
         virtual ~DTEDTerrainReader();
         
         /**
          * Searches the list of file resource paths for DTED data
          * of the given latitude, longitude and level.
          * @param basePath String containing the root DTED path.  
          * @param lat Latitude (ex. e90)
          * @param lon Longitude (ex. w150)
          * @return The full path to the DTED file resource.
          */
         std::string GetDTEDFilePath(const std::string &lat, 
            const std::string &lon, int level);         
      
      private:
      
         ///Current max DTED level to search for when loading data.
         const DTEDLevelEnum *mDTEDLevel;     
   };    

}

#endif

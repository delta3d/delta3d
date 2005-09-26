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
* @author Matthew W. Campbell
*/
#ifndef DELTA_DTEDTERRAINREADER
#define DELTA_DTEDTERRAINREADER

#include  "dtTerrain/terraindatareader.h"

namespace dtTerrain 
{

   /**
    * This data reader loads DTED data mapping to a specified latitude and
    * longitude.  Under the covers this reader uses GDAL to load the actual
    * heightfield values.  Note, to load specific DTED levels, one must
    * specify the maximum level to load.  If the specified level is not present,
    * the next highest DTED level will be loaded.
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
          * @param maxLevel The maximum level of DTED data to load.  If the maximum
          *    level is not available, the next highest available dataset will
          *    be loaded.  By default, the loaded will load the maximum available
          *    resolution dataset.
          * @note This may be queried once data is loaded to check to see if the
          *    specified dataset level was actually available.
          */
         DTEDTerrainReader(const DTEDLevelEnum &maxlevel = DTEDLevelEnum::FIVE,
            const std::string &name="DTEDReader");
         
         /**
          * Sets the maximum resolution DTED level to load.  This means that the data
          * loaded will be as close to the specified level as possible based on 
          * availability of data, however, will not be anything greater than this
          * value.
          * @param level The maximum level of data to load.
          */
         void SetMaxDTEDLevel(const DTEDLevelEnum &level);
         
         /**
          * Gets the current maximum DTED level on this loader.
          * @return An enumeration corresponding to the max level.
          */
         const DTEDLevelEnum &GetMaxDTEDLevel() const
         {
            return *mMaxDTEDLevel;
         }
         
         /**
          * Gets the type of terrain data this reader supports.
          * @return TerrainDataType::DTED.
          */
         const TerrainDataType &GetDataType() const { return TerrainDataType::DTED; }
        
      protected:
      
         ///Destroys the terrain reader.
         virtual ~DTEDTerrainReader();
         
         /**
          * Loads a chunk of elevation data corresponding to the given latitude
          * and longitude.
          * @param dataPath The path of the file or directory containing the terrain data.
          * @param lat Latitude of terrain chunk (cell) to load.
          * @param lon Longitude of the terrain chunk (cell) to load.
          */
         virtual void LoadElevationData(const std::string &dataPath, int lat, int lon);
                
         /**
          * Loads a chunk of elevation data corresponding to the given x and y
          * world coordinates.
          * @param dataPath The path of the file or directory containing the terrain data.
          * @param x X absolute world coordinate of the terrain chunk to load.
          * @param y Y absolute world coordinate of the terrain chunk to load.
          */
         virtual void LoadElevationData(const std::string &dataPath, double x, double y) { }
         
         /**
          * Searches the list of file resource paths for DTED data
          * of the given latitude, longitude and level.
          * @param basePath String containing the root DTED path.  
          * @param lat Latitude (ex. e90)
          * @param lon Longitude (ex. w150)
          * @return The full path to the DTED file resource.
          */
         std::string GetDTEDFilePath(const std::string &basePath, 
            const std::string &lat, const std::string &lon, int level);
      
      private:
      
         ///Current max DTED level to search for when loading data.
         const DTEDLevelEnum *mMaxDTEDLevel;         
   };    

}

#endif

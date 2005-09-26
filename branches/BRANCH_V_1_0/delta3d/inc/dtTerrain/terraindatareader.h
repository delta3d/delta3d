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
#ifndef DELTA_TERRAINDATAREADER
#define DELTA_TERRAINDATAREADER

#include <osg/Shape>
#include "dtCore/base.h"
#include "dtCore/refptr.h"
#include "dtTerrain/terraindatatype.h"
#include "dtTerrain/terrain.h"

namespace dtTerrain 
{
   /**
    * These exceptions are used by terrain data readers when an error occurs
    * during the load process.
    */
   class DT_TERRAIN_EXPORT TerrainDataReaderException : public dtDAL::ExceptionEnum
   {
      DECLARE_ENUM(TerrainDataReaderException);
      public:
         
         ///Thrown when a particular version of LoadElevation is not supported for a reader.
         static TerrainDataReaderException UNSUPPORTED_COORDINATE_SYSTEM;
         
         ///Thrown when a particular data file was not found.
         static TerrainDataReaderException DATA_RESOURCE_NOT_FOUND;
         
         ///Thrown if an error was encountered while reading the terrain data.
         static TerrainDataReaderException COULD_NOT_READ_DATA;
         
         ///Thrown if a third party data reader plugin could not be found.
         static TerrainDataReaderException READER_PLUGIN_NOT_FOUND;
      
      protected:
      
         ///Simple enumeration constructor.
         TerrainDataReaderException(const std::string &name) : dtDAL::ExceptionEnum(name)
         {
            AddInstance(this);
         }
   };
	
   /**
    * This is mostly an interface class to the data reader functionality of Delta3D
    * terrains.  This allows the terrain to support an arbitrary number of terrain
    * formats as long as the reader can parse the data and convert it to a valid
    * osg::HeightField.
    * @note
    *    This interface exposes several methods for loading elevation data.  One or
    *    more methods may be implemented.  The terrain class decides which method to
    *    use based on the current data loading coordinate system active at the time
    *    the terrain data is loaded.
    */
   class DT_TERRAIN_EXPORT TerrainDataReader : public dtCore::Base
   {
      public:
      
         /**
          * Constructs the terrain data reader.
          */
         TerrainDataReader(const std::string &name="TerrainDataReader") :
            dtCore::Base(name), mHeightField(NULL) { }
            
         /**
          * Gets the type of data this reader supports.
          * @return TerrainDataType The data type for this reader.
          */  
         virtual const TerrainDataType &GetDataType() const = 0;
         
         /**
          * Gets the height field generated from the terrain data.
          * @return A valid heightfield.
          */
         osg::HeightField *GetHeightField() { return mHeightField.get(); }
         
         /**
          * Gets the height field generated from the terrain data.
          * @return A valid heightfield whos contents cannot be modified.
          */     
         const osg::HeightField *GetHeightField() const { return mHeightField.get(); }    
         
         /**
          * Gets the terrain object that currently owns this reader.
          * @return A pointer to the parent terrain.
          */
         dtTerrain::Terrain *GetParentTerrain() { return mParentTerrain.get(); }
         
         /**
          * Gets a read-only terrain object that currently owns this reader.
          * @return A const pointer to the parent terrain.
          */
         const dtTerrain::Terrain *GetParentTerrain() const { return mParentTerrain.get(); }
        
      protected:
   
         ///Empty destructor...
         virtual ~TerrainDataReader() { }
         
         /**
          * Loads a chunk of elevation data corresponding to the given latitude
          * and longitude.
          * @param dataPath The path of the file or directory containing the terrain data.
          * @param lat Latitude of terrain chunk (cell) to load.
          * @param lon Longitude of the terrain chunk (cell) to load.
          */
         virtual void LoadElevationData(const std::string &dataPath, int lat, int lon) { }
                
         /**
          * Loads a chunk of elevation data corresponding to the given x and y
          * world coordinates.
          * @param dataPath The path of the file or directory containing the terrain data.
          * @param x X absolute world coordinate of the terrain chunk to load.
          * @param y Y absolute world coordinate of the terrain chunk to load.
          */
         virtual void LoadElevationData(const std::string &dataPath, double x, double y) { }
                 
         ///Heightfield data...
         dtCore::RefPtr<osg::HeightField> mHeightField;
         
         ///Allow the terrain to have access to this class.
         friend class Terrain;
         
      private:
         
         /**
          * The terrain object that currently owns this reader.
          * @note Reader instances can only be assigned to one terrain at a time.
          */
         dtCore::RefPtr<dtTerrain::Terrain> mParentTerrain;
   };
	
}

#endif

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

#ifndef DELTA_TERRAINDATAREADER
#define DELTA_TERRAINDATAREADER

#include <osg/Shape>
#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtUtil/enumeration.h>
#include <dtTerrain/terraindatatype.h>
#include <dtTerrain/pagedterraintile.h>

namespace dtTerrain
{
   class Terrain;

   class TerrainDataResourceNotFoundException : public dtUtil::Exception
   {
   public:
   	TerrainDataResourceNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~TerrainDataResourceNotFoundException() {};
   };
   
   class TerrainCouldNotReadDataException : public dtUtil::Exception
   {
   public:
   	TerrainCouldNotReadDataException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~TerrainCouldNotReadDataException() {};
   };
   
   class TerrainReaderPluginNotFoundException : public dtUtil::Exception
   {
   public:
   	TerrainReaderPluginNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~TerrainReaderPluginNotFoundException() {};
   };
   

   /**
    * This enumeration specifies the valid resize policies the data readers should
    * follow when loading a new heightfield.  This is to ensure that the data
    * formats supported by the reader are imported in a manor suitable to the
    * current renderer.
    */
   class DT_TERRAIN_EXPORT HeightFieldResizePolicy : public dtUtil::Enumeration
   {
      DECLARE_ENUM(HeightFieldResizePolicy);
      public:

         ///No resizing should occur.
         static const HeightFieldResizePolicy NONE;

         ///Data should be resized such that it is a power of two in all dimensions
         ///closests to its original size. (2^n)
         static const HeightFieldResizePolicy NEAREST_POWER_OF_TWO;

         ///Data should be resized such that it is a power of two plus one in all
         ///dimensions closests to its original size. (2^n + 1)
         static const HeightFieldResizePolicy NEAREST_POWER_OF_TWO_PLUS_ONE;

      protected:
         HeightFieldResizePolicy(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

   /**
    * This is mostly an interface class to the data reader functionality of Delta3D
    * terrains.  This allows the terrain to support an arbitrary number of terrain
    * formats as long as the reader can parse the data and convert it to a
    * terrain tile.  TerrainDataReaders are primarily in charge of loading
    * heightfield data, however, some may load more than that depending
    * on a specific applications needs.
    */
   class DT_TERRAIN_EXPORT TerrainDataReader : public dtCore::Base
   {
      public:

         /**
          * Constructs the terrain data reader.  Sets the default resize policy
          * to NEAREST_POWER_OF_TWO_PLUS_ONE since this is a fairly common
          * size requirement for terrain rendering architectures.
          */
         TerrainDataReader(const std::string &name="TerrainDataReader") :
            dtCore::Base(name)
         {
            SetResizePolicy(HeightFieldResizePolicy::NEAREST_POWER_OF_TWO_PLUS_ONE);
         }

         /**
          * This method is called when the parent terrain needs
          * to load a tile.
          * @param tile The new tile.  The loader should populate
          *    the fields of the tile as appropriate.
          * @note Should throw an exception if any errors occur.
          * @note At this point, the terrain tiles have had a chance to load
          *    data from their cache.  Therefore, readers implementing this
          *    method may want to check the status of a tile's data before
          *    loading since it may have been retrieved from the cache.
          * @note Resources may be looked up using the parent terrain's
          *    resource path list.  In most cases, the terrain will be
          *    made aware of any resource locations its readers may need.
          * @see Terrain
          * @see PagedTerrainTile
          */
         virtual bool OnLoadTerrainTile(PagedTerrainTile &tile) = 0;

         /**
          * This method is called when the parent terrain wishes
          * to unload a terrain tile from its list of resident tiles.
          * @param tile The tile being unloaded.
          * @note Should throw an exception if any errors occur.
          * @note The default implementation does nothing as most renderers
          *    will probably only process data and have no need to respond
          *    to this method, however, it is available if needed.  Also note,
          *    that the tile itself is responsible for caching its data, however,
          *    any data a renderer wishes to control may be cached in this method.
          * @see Terrain
          * @see PagedTerrainTile
          */
         virtual void OnUnloadTerrainTile(PagedTerrainTile &tile) { }

         /**
          * Gets the type of data this reader supports.
          * @return TerrainDataType The data type for this reader.
          */
         virtual const TerrainDataType &GetDataType() const = 0;

         /**
          * This method is used by the terrain reader to generate a unique
          * path name for a terrain tile's cached data.  This path is a
          * subdirectory under the parent terrain's cache path and should
          * be unique on a per tile basis.  This id should also be consistent
          * across application invocations.
          * @param tile The tile with which to generate the path for.
          * @return The tile specific cache path.
          * @note As an example, the DTEDTerrainReader generates this path
          *    by concatenating the latitude, longitude, and DTED level.
          * @note On a side note, I would have perferred that the parent
          *    terrain generate this cache path, however, I was unable to
          *    come up with a generic mechanism for this to occur so I
          *    thought the reader would be the next most likely candidate.
          */
         virtual const std::string GenerateTerrainTileCachePath(
            const PagedTerrainTile &tile) = 0;

         /**
          * Sets the heightfield resize policy for this reader.  This is used
          * when loading a new tile. If the policy is not set to NONE,
          * the loaded heightfield will be resized according to the current
          * policy.
          * @param policy The new policy.
          * @note This is useful as many renderers require their heightfield
          *    data to have certain dimensions in order for their refinement
          *    and or data hierarchy to work.  By default, the policy is set
          *    to POWER_OF_TWO_PLUS_ONE.
          */
         void SetResizePolicy(const HeightFieldResizePolicy &policy)
         {
            mResizePolicy = &policy;
         }

         /**
          * Gets the current heightfield resize policy.
          * @return The current policy.
          */
         const HeightFieldResizePolicy &GetResizePolicy() const
         {
            return *mResizePolicy;
         }

         /**
          * Gets the terrain object that currently owns this reader.
          * @return A pointer to the parent terrain.
          */
         Terrain *GetParentTerrain() { return mParentTerrain; }

         /**
          * Gets a read-only terrain object that currently owns this reader.
          * @return A const pointer to the parent terrain.
          */
         const Terrain *GetParentTerrain() const { return mParentTerrain; }

      protected:

         ///Empty destructor...
         virtual ~TerrainDataReader() { }

         /**
          * Converts an OpenSceneGraph floating point heightfield to
          * an internal 16-bit heightfield.
          * @param hf The OSG heightfield.
          * @return A 16-bit heightfield.
          * @see HeightField
          */
         HeightField *ConvertHeightField(osg::HeightField *hf);

         /**
          * Scales an OpenSceneGraph heightfield according to the current resize
          * policy.
          * @param hf The OSG heightfield to resize.
          * @return The resized heightfield.  If the heightfield to resize meets
          *    the current resize criteria or the current policy is NONE, this will
          *    be a pointer to the same data that was passed as a parameter.
          */
         dtCore::RefPtr<osg::HeightField> ScaleOSGHeightField(osg::HeightField *hf);

         /**
          * Performs a bi-linear interpolation at the specified coordinate to
          * sample a height value.
          * @param hf The heightfield to query.
          * @param x The x coordinate to sample from.
          * @param y The y coordinate to sample from.
          */
         float GetInterpolatedHeight(const osg::HeightField *hf,
            float x, float y);

         ///Allow the terrain to have access to this class.
         friend class Terrain;

      private:

         /**
          * The terrain object that currently owns this reader.
          * @note Reader instances can only be assigned to one terrain at a time.
          */
         Terrain *mParentTerrain;

         ///Current resize policy assigned to this reader.
         const HeightFieldResizePolicy *mResizePolicy;
   };

}

#endif

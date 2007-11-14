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
#ifndef DELTA_PAGEDTERRAINTILE_H
#define DELTA_PAGEDTERRAINTILE_H

#include <string>
#include <osg/Referenced>
#include <osg/Image>
#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>
#include "dtUtil/enumeration.h"
#include "dtTerrain/terrain_export.h"
#include "dtTerrain/geocoordinates.h"
#include "dtTerrain/heightfield.h"

namespace dtTerrain
{   
   class Terrain;
   
   /**
    * This enumeration defines the different states a tile could be in
    * throughout its lifetime.
    */
   class PagedTerrainTileLoadState : public dtUtil::Enumeration
   {
      DECLARE_ENUM(PagedTerrainTileLoadState);
      public:
      
         ///Tile is loaded and resides in memory.
         static const PagedTerrainTileLoadState LOADED;
         
         ///Tile is in the process of being loaded from disk or other meduim.
         static const PagedTerrainTileLoadState PAGING;
         
         ///Tile is not resident in memory.  If a tile is in this state and the application
         ///needs its data, the terrain will automatically page it from disk.
         static const PagedTerrainTileLoadState NOT_LOADED;
      
      protected:
         PagedTerrainTileLoadState(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };
   
   /**
    * This enumeration is used to identity built in resources that the terrain tiles
    * are aware of.  These provide a unique identifier for the resources that need
    * to be cached and uncached when terrain tiles are loaded and unloaded.
    */
   class PagedTerrainTileResourceName : public dtUtil::Enumeration
   {
      DECLARE_ENUM(PagedTerrainTileResourceName);
      public:
      
         ///This is a constant that defines the file name assigned to a 
         ///heightfield resource for a particular tile.  This should be used when
         ///writing and reading a tile's heightmap from its cache.
         static const PagedTerrainTileResourceName HEIGHTFIELD_FILENAME;
      
      protected:
         PagedTerrainTileResourceName(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };         
   
   /**
    * This class is the base class for a tile of data.  Instances of this class are 
    * the atomic data elements the terrain manages when navigating through its data.
    * Paging and disk caching (if enabled) is handled transparently when needed by 
    * the terrain and it provides, by default, access to its heightfield, base texture, 
    * and other fundamental data structures.  Although this base class has many 
    * features, application dependent data and behavior can still be added to this 
    * base tile through subclasses.
    */
   class DT_TERRAIN_EXPORT PagedTerrainTile : public osg::Referenced
   {
      public:       
      
         /**
          * Sets the origin with which this tile maps to.
          * @param cs The coordinate system containing this tile's origin.
          */
         void SetGeoCoordinates(const GeoCoordinates &cs) { mCoords = cs; }
         
         /**
          * Gets this tile's coordinate system.
          * @return Constant reference to this tile's coordinate system.
          */
         const GeoCoordinates &GetGeoCoordinates() const { return mCoords; }
         
         /**
          * Sets the heightfield that maps to this terrain tile.
          * @param hf The new heightfield.
          */
         void SetHeightField(HeightField *hf) 
         { 
            mHeightField = hf; 
            SetUpdateCache(true); 
         }
         
         /**
          * Gets the heightfield currently assigned to this terrain tile.
          * @return A pointer to the heightfield or NULL if it is not valid.
          */
         const HeightField *GetHeightField() const { return mHeightField.get(); }    
         
         /**
          * Sets the base texture for this terrain tile.  The base texture will be draped
          * over the tile when it is rendered.
          * @param image The image to use as the base texure of this tile.
          */
         void SetBaseTextureImage(osg::Image *image) 
         { 
            mBaseTextureImage = image; 
            SetUpdateCache(true); 
         }
         
         /**
          * Gets the image currently being used by this tile as its base texture.
          * @return A pointer to the image or NULL if it is not valid.
          */
         osg::Image *GetBaseTextureImage() { return mBaseTextureImage.get(); }
         
         /**
          * This method provides the paged tile with an opportunity to cache any
          * data that it wants to retrieve later.
          * @note The terrain reader and renderers handle data tracking of the tile's 
          *    basic needs.  For example, the terrain reader will load/save the heightfield
          *    to the cache when needed.  This method is intended to be used for 
          *    application specific data in subclasses of the pagedterraintile.
          * @note Any errors should be tracked by throwing exceptions.
          */
         virtual void WriteToCache();     
         
         /**
          * This method provides the paged tile with an opportunity to restore any
          * data that was previously cached.
          * @note The terrain readers and renderers handle data tracking of the tile's
          *    basic needs.  This method provides a hook into the tile loading
          *    process by giving the tile an opportunity to load any application 
          *    specific resources it may need.
          * @note Any errors should be tracked by throwing exceptions.
          */
         virtual void ReadFromCache();
         
         /**
          * Gets the cache path assigned to this tile.
          * @return The current cache path or the empty string if caching is disabled.
          */
         const std::string &GetCachePath() const { return mCachePath; }
         
         /**
          * Gets the status of this tile's caching.
          * @return True if enabled, false otherwise.
          */
         bool IsCachingEnabled() const { return mEnableCaching; }
         
         /**
          * Sets whether or not the cache needs to be updated to reflect the current
          * state of the tile.  This should be set to true when new data is set
          * or added to the tile that needs to be cached.
          * @param flag True if the cache should be updated, false otherwise.
          * @note If this value is false, the parent terrain will not call the
          *    tile's WriteToCache() method.
          */
         void SetUpdateCache(bool flag) { mUpdateCache = flag; }
     
         /**
          * Gets whether or not this tile's cache is out of sync with its data.
          * @return True if this tile's cache needs updating.
          */
         bool GetUpdateCache() const { return mUpdateCache; }
         
         /**
          * Gets the terrain that created this terrain tile and therefore owns it.
          * @return A const pointer to the parent terrain.
          */
         const Terrain *GetParentTerrain() const { return mParentTerrain; }
         
         /**
          * Gets the terrain that created this terrain tile and therefore owns it.
          * @return A pointer to the parent terrain.
          */
         Terrain *GetParentTerrain() { return mParentTerrain; }
                 
      protected:
         
         /**
          * Constructs a terrain tile.  Note, the constrmUpdateCacheuctor is protected as to
          * enforce the use of the terrain class in order to create a new tile.
          */
         PagedTerrainTile(Terrain *terrain);     
         
         /**
          * Destroys the terrain tile.  Since most internal data is managed by smart
          * pointers, this method does practically nothing.
          */
         virtual ~PagedTerrainTile();
         
         /**
          * Sets the cache path for this tile.  Any data corresponding to this tile
          * can be cached by terrain readers and renderers when needed.
          * @param path The path assigned to this tile.
          * @note This path is most likely set by the terrain reader loading it.
          *    In most cases, this should not be modified by any other classes.
          */
         void SetCachePath(const std::string &path);     
         
         ///Allow the factory and terrain to have access to this class.
         friend class PagedTerrainTileFactory;
         friend class Terrain;
         
      private:
        
         ///The heightfield referenced by this terrain tile.
         dtCore::RefPtr<HeightField> mHeightField;
         
         ///This is provided in case a tile wants to reference a base texture 
         ///image.  Subclasses of PagedTerrainTile could include other data
         ///elements if one base texture does not suffice.
         dtCore::RefPtr<osg::Image> mBaseTextureImage;    
         
         ///Stores the origin of this tile in both geodetic and cartesian coordinates.     
         GeoCoordinates mCoords;
         
         ///The cache path used to cache data for this tile.
         std::string mCachePath;
         
         ///Indicates whether or not data for this tile should be cached.
         bool mEnableCaching;
         
         ///Indicates whether or not this tile's cache should be updated.
         bool mUpdateCache;
         
         ///This enumeration determines the state the tile is in.. LOADING, LOADED, etc.
         const PagedTerrainTileLoadState *mLoadStatus;
         
         ///Terrain that created this tile.
         Terrain *mParentTerrain;
   };
   
}

#endif 

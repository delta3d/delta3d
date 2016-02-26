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

#ifndef DELTA_TERRAIN
#define DELTA_TERRAIN

#include <map>
#include <string>
#include <queue>
#include <list>

#include <dtCore/transformable.h>
#include <dtUtil/enumeration.h>

#include <dtTerrain/geocoordinates.h>
#include <dtTerrain/pagedterraintilefactory.h>
#include <dtTerrain/terrain_export.h>

/**
 * An extensible library for rendering terrain databases.
 */
namespace dtTerrain
{
   ///Forward declare the interfaces we need in the terrain.
   class TerrainDataReader;
   class TerrainDataRenderer;
   class TerrainDecorationLayer;
   class PagedTerrainTile;

   class NullPointerException : public dtUtil::Exception
   {
   public:
   	NullPointerException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~NullPointerException() {};
   };

   class InvalidDataReaderException : public dtUtil::Exception
   {
   public:
   	InvalidDataReaderException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidDataReaderException() {};
   };
   
   
   class InvalidDataRendererException : public dtUtil::Exception
   {
   public:
   	InvalidDataRendererException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidDataRendererException() {};
   };
   
   class InvalidDecorationLayerException : public dtUtil::Exception
   {
   public:
   	InvalidDecorationLayerException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidDecorationLayerException() {};
   };
   

   /**
    * This is the base terrain class that provides most of the functionlity
    * required for terrain rendering.  This class uses a component or aggregate
    * based design philosophy by utilizing terrain data readers and terrain data
    * renderers.  Each reader loads a specific type of terrain data and the
    * renderer implements a particular terrain rendering algorithm.  Each of these
    * components can be dynamically changed on each instance of this terrain class.
    */
   class DT_TERRAIN_EXPORT Terrain : public dtCore::Transformable
   {
      //Help minimize some typing...
      typedef std::map<std::string,dtCore::RefPtr<TerrainDecorationLayer> > TerrainLayerMap;
      typedef std::map<GeoCoordinates,dtCore::RefPtr<PagedTerrainTile> > TerrainTileMap;


      DECLARE_MANAGEMENT_LAYER(Terrain);
      public:

         /**
          * Default Constructor - Sets the name and performs some routine
          * registration.
          * @param name The name of this terrain object.
          */
         Terrain(const std::string &name="Terrain");

         /**
          * Adds a search path to the list of file paths to search for when loading
          * terrain data or other resources.  This path must be relative to a path
          * in the Delta3D data path list.
          * @param The new search path.
          */
         void AddResourcePath(const std::string &path);

         /**
          * Removes a resource path from the list of known resource paths.
          * @param path The path to remove.  If it is not currently in the list
          *    this method does nothing.
          */
         void RemoveResourcePath(const std::string &path);

         /**
          * Gets the terrain's current list of resource search paths.
          * @return The list of string paths.
          */
         const std::list<std::string> &GetResourcePathList() const
         {
            return mResourcePathList;
         }

         /**
          * Searches the terrains resource path list for the specified
          * resource.  The first resource matching the specified path is
          * returned.
          * @param The path to the resource to load.  This should be
          *    either a filename or a path relative to the resource paths
          *    registered with the terrain.
          * @return An absolute path to the specified resource.  If the
          *    resource was not found, an empty string is returned.
          */
         const std::string FindResource(const std::string &path);

         /**
          * Searches the terrain resources path list for all resources
          * matching the specified path.
          * @path The path to the resource to load.  This should be
          *    either a filename or a path relative to the resource paths
          *    registered with the terrain.
          * @resourcePaths The result list filled with absolute paths to
          *    all resources matching the specified path.
          */
         void FindAllResources(const std::string &path,
            std::vector<std::string> &resourcePaths);

         /**
          * Gets the height of the terrain at the given (x,y) coordinates.
          * @note This just calls the current terrain renderer's GetHeight
          *    method.  If the renderer is invalid, the height returned is 0.0f.
          */
         float GetHeight(float x, float y);

         /**
          * Given pointOne and pointTwo, both in world space and with all coordinates
          * in Cartesian space (essentially in meters along X, Y and Z),
          * returns true if there is a clear line of sight and false if the view
          * is blocked.
          *
          * @param pointOne The start point.
          * @param pointTwo The end point.
          * @return Returns true if there is a clear line of sight from pointOne to
          * pointTwo and false if the view is blocked.
          */
         bool IsClearLineOfSight( const osg::Vec3& pointOne,
                                  const osg::Vec3& pointTwo );

         void SetLineOfSightSpacing(float spacing) {mLOSPostSpacing = spacing;}

         float GetLineOfSightSpacing() const {return mLOSPostSpacing;}

         virtual void LoadTerrainTile(PagedTerrainTile &newTile);

         virtual void UnloadTerrainTile(PagedTerrainTile &toRemove);

         virtual void UnloadAllTerrainTiles();

         virtual PagedTerrainTile *CreateTerrainTile(const GeoCoordinates &coords);

         virtual bool IsTerrainTileResident(const GeoCoordinates &coords);

         virtual void EnsureTileVisibility(const std::set<GeoCoordinates> &coordList);

         /**
          * Sets the path of the terrain cache.  The terrain cache is a directory
          * somewhere on the hard drive which is used to store on the fly data
          * calculations which can then be reused on successive runs.  If this
          * path is not set, no data caching will occur.
          * @note The directory will be created if it does not already exist.
          * @note This path will be the root cache path for any terrain tiles
          *    that need to have their data cached.  Each tile's cache path
          *    is a subdirectory of this one.
          * @note This path should only be used directly when data needs to be
          *    cached that is independant of a particular terrain tile.  Tile
          *    specific data should be cached in the tile's cache directory.
          * @param path The path to use for the terrain cache.
          * @throws Exception if the path does not exist and cannot be created.
          */
         bool SetCachePath(const std::string &path);

         /**
          * Gets the current terrain cache.
          * @return The base cache path for this terrain.
          */
         const std::string &GetCachePath() const { return mCachePath; }

         void SetLoadDistance(float value) { mLoadDistance = value; }

         float GetLoadDistance() const { return mLoadDistance; }

         /**
          * Sets the terrain data reader.  This must be set before any terrain can
          * be loaded.
          */
         void SetDataReader(TerrainDataReader *reader);

         /**
          * Gets the current terrain data reader.
          * @return A data reader object.
          */
         TerrainDataReader *GetDataReader() { return mDataReader.get(); }

         /**
          * Gets the current terrain data reader.
          * @return A const data reader object.
          */
         const TerrainDataReader *GetDataReader() const { return mDataReader.get(); }

         /**
          * Sets the terrain data renderer.  This must be set before any terrain can be
          * loaded and rendered.
          */
         void SetDataRenderer(TerrainDataRenderer *renderer);

         /**
          * Gets the current terrain data renderer.
          * @return A data renderer object.
          */
         TerrainDataRenderer *GetDataRenderer() { return mDataRenderer.get(); }

         /**
          * Gets the current terrain data renderer.
          * @return A const data renderer object.
          */
         const TerrainDataRenderer *GetDataRenderer() const { return mDataRenderer.get(); }

         /**
          * Adds a new decoration layer to this terrain.
          * @param newLayer The new layer to add.
          * @note This will also ask the layer for its scene node (GetOSGNode())
          *    and add it as a child to the terrain.
          * @note If the new layer's name is not unique, the name is appended with
          *    a unique identifier and a warning is logged.
          */
         void AddDecorationLayer(TerrainDecorationLayer *newLayer);

         /**
          * Removes the specified decoration layer from this terrain.
          * @param toRemove The layer to remove.
          * @note This will also remove the scene node for the decoration layer
          *    from this terrain.
          */
         void RemoveDecorationLayer(TerrainDecorationLayer *toRemove);

         /**
          * Removes a decoration layer matching the specified name.
          * @param name The name of the terrain layer to remove.
          */
         void RemoveDecorationLayer(const std::string &name);

         /**
          * Gets the decoration layer whos name matches the specified
          * parameter.
          * @param name The name of the layer to retrieve.
          * @return A valid decoration layer or NULL if the layer could not
          *    be found.
          */
         TerrainDecorationLayer *GetDecorationLayer(const std::string &name);

         /**
          * Gets the decoration layer who's name matches the specified
          * parameter.
          * @param name The name of the layer to retrieve.
          * @return A valid decoration layer or NULL if the layer could not be found.
          */
         const TerrainDecorationLayer *GetDecorationLayer(const std::string &name) const;

         /**
          * Gets the number of decorator layers currently overlaid on this terrain.
          * @return The number of layers.
          */
         unsigned int GetNumDecorationLayers() const
         {
            return mDecorationLayers.size();
         }

         /**
          * Removes all the decorator layers from this terrain.
          */
         void ClearDecorationLayers();

         /**
          * Hides the decoration layer that matches the specified name.
          * @param name The name of the layer to hide.
          */
         void HideDecorationLayer(const std::string &name);

         /**
          * Hides the specified decoration layer.
          * @param toHide The layer to hide.
          */
         void HideDecorationLayer(TerrainDecorationLayer *toHide);

         /**
          * Makes the decoration layer matching the specified name visible.
          * @param name The name of the layer to show.
          */
         void ShowDecorationLayer(const std::string &name);

         /**
          * Makes the specified decoration layer visible in the scene.
          * @param The layer to make visible.
          */
         void ShowDecorationLayer(TerrainDecorationLayer *toShow);

         /**
          * Fills a vector with the decoration layers currently attached to this
          * terrain.
          * @param layers The vector to be filled.
          * @note The vector is cleared before it is filled so any data that is
          *    passed to this method will be destroyed.
          */
         void GetDecorationLayers(std::vector<dtCore::RefPtr<TerrainDecorationLayer> > &layers);

         void SetTerrainTileFactory(PagedTerrainTileFactory &factory) { mTileFactory = &factory; }

         PagedTerrainTileFactory *GetTerrainTileFactory() { return mTileFactory.get(); }

         virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

      protected:

         ///Cleans up the terrain and its components.
         virtual ~Terrain();

         virtual void PreFrame(double frameTime);

         virtual void PostFrame(double frameTime);

         ///List of terrain tiles currently queued up for loading.
         std::queue<dtCore::RefPtr<PagedTerrainTile> > mTilesToLoadQ;

         ///List of terrain tiles which are currently loaded into memory.
         TerrainTileMap mResidentTiles;

         ///Queue of terrain tiles that need to be cached or destroyed.
         std::queue<dtCore::RefPtr<PagedTerrainTile> > mTilesToUnloadQ;

      private:

         ///Full path to the terrain cache directory.
         std::string mCachePath;

         ///This specifies a radius which tells the terrain how many terrain tiles
         ///to load into memory.
         float mLoadDistance;

         ///List of resource paths used by the terrain, its reader, and its
         ///renderer to search for data.
         std::list<std::string> mResourcePathList;

         ///The current terrain data reader.
         dtCore::RefPtr<TerrainDataReader> mDataReader;

         ///The current terrain data renderer.
         dtCore::RefPtr<TerrainDataRenderer> mDataRenderer;

         ///The current terrain tile creation factory.
         dtCore::RefPtr<PagedTerrainTileFactory> mTileFactory;

         ///List of the layers currently attached to this terrain.
         TerrainLayerMap mDecorationLayers;

         float mLOSPostSpacing;
   };

   /**
    * Trivial Line Of Sight calcuator. We basically walk along the ray between
    * the two points, doing point sampling.  This is brute force and not the
    * most efficient means of doing this test, but this is the first implementation.
    *
    * This routine ONLY checks visibility against ground terrain!  We do not
    * check test against other objects, such as buildings or other vehicles.
    *
    * Sampling rate: our DTED data is sampled at about 30m resolution. Therefore
    * we adjust the postSpacing to be smaller than that, and use that to step along
    * the viewing ray.
    *
    * This uses dtTerrain::GetHeight which uses the current Renderer
    * this may be cause lots extra work, especially when called repeatedly
    * might be better to do it directly on heightField of tile?
    *
    * @param terrain Test the line of sight against this instance of Terrain.
    * @param pointOne The start point.
    * @param pointTwo The end point.
    *
    * @pre terrain != 0
    */
   bool SimpleLineOfSight( dtTerrain::Terrain* terrain,
                           const osg::Vec3& pointOne,
                           const osg::Vec3& pointTwo );

}

#endif

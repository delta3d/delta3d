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

#ifndef DELTA_TERRAIN
#define DELTA_TERRAIN

#include <map>
#include <string>
#include <osg/Vec3d>
#include "dtCore/physical.h"
#include "dtDAL/exceptionenum.h"
#include "dtTerrain/terrain_export.h"

namespace dtTerrain 
{
   /**
    * The length of the semi-major axis (equatorial radius).
    * (WGS 84)
    */
   const float SEMI_MAJOR_AXIS = 6378137.0f;
   
   /**
    * The reciprocal of the flattening parameter. (WGS 84).
    */
   const float FLATTENING_RECIPROCAL = 298.257223563f;   
  
   ///Forward declare the interfaces we need in the terrain.
   class TerrainDataReader;
   class TerrainDataRenderer;
   class TerrainDecorationLayer;
   
   /**
    * This class enumerates the different exceptions that can be thrown by
    * terrain instances.
    */
   class DT_TERRAIN_EXPORT TerrainException : public dtDAL::ExceptionEnum
   {
      DECLARE_ENUM(TerrainException);
      public:
      
         ///Thrown if a specified resource could not be read or loaded.
         static TerrainException INVALID_RESOURCE_PATH;
         
         ///Thrown if the current data reader does not support the specified
         ///resource.
         static TerrainException UNSUPPORTED_DATA_FORMAT;
         
         ///Thrown if the terrain requires a data reader but it is currently invalid.
         static TerrainException INVALID_DATA_READER;
         
         ///Thrown if the terrain encounters an invalid data renderer.
         static TerrainException INVALID_DATA_RENDERER;
         
         ///Thrown if an invalid decoration layer is added to the terrain.
         static TerrainException INVALID_DECORATION_LAYER;
         
      protected:
         
         ///Simple enumeration constructor.
         TerrainException(const std::string &name) : dtDAL::ExceptionEnum(name)
         {
            AddInstance(this);
         }
   };
   
   /**
    * This is a simple enumeration specifiying in what type of coordinate system
    * the terrain should reference its data.
    */
   class DT_TERRAIN_EXPORT TerrainCoordinateSystem : public dtUtil::Enumeration
   {
      DECLARE_ENUM(TerrainCoordinateSystem);
      public:
         
         ///Simple X,Y,Z coordinates.     
         static const TerrainCoordinateSystem CARTESIAN;      
         
         ///Latitude, Longitude, and Elevation coordinates.
         static const TerrainCoordinateSystem GEOGRAPHIC;
      
      protected:
     
         ///Simple constructor for enumerations.
         TerrainCoordinateSystem(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

   /**
    * This is the base terrain class that provides most of the functionlity
    * required for terrain rendering.  This class uses a component or aggregate
    * based design philosophy by utilizing terrain data readers and terrain data
    * renderers.  Each reader loads a specific type of terrain data and the 
    * renderer implements a particular terrain rendering algorithm.  Each of these
    * components can be dynamically changed on each instance of this terrain class.
    */
   class DT_TERRAIN_EXPORT Terrain : public dtCore::Physical
   {
      //Help minimize some typing...
      typedef std::map<std::string,dtCore::RefPtr<TerrainDecorationLayer> > TerrainLayer;
      
      DECLARE_MANAGEMENT_LAYER(Terrain);
      public:
      
         /**
          * Default Constructor - Sets the name and performs some routine
          * registration.
          * @param name The name of this terrain object.
          */
         Terrain(const std::string &name="Terrain");
         
         /**
          * Loads terrain data from the specified resource path.  The resource path
          * could be a directory or single file, the terrain class does not care.
          * The TerrainDataReader assigned to this terrain will load the resource
          * if it supports it.
          * @param path Path to the resource to load.
          * @throws Throws TerrainException if any errors occur during resource 
          *    loading.
          * @note The terrain is loaded using the current origin.  Therefore, before
          *    loading terrain resources the origin should be set to the coordinates
          *    mapping to the region to start in.
          * @note If this method succeeds, the resource data path will be set to 
          *    the path given to this method.
          */
         virtual void LoadResource(const std::string &path);
         
         /**
          * Gets the terrain resource path currently being referenced by this terrain.
          * @return The current data path.
          */
         const std::string &GetResourcePath() const
         {
            return mDataPath;
         }
         
         /**
          * Gets the type of coordinate system the terrain is currentl
          * referencing.
          * @return The current coordinate system type.
          */
         const TerrainCoordinateSystem &GetCoordinateSystem() const 
         { 
            return *mCoordinateSystem;
         }
            
         /**
          * Sets the origin of the terrain.  If the terrain coordinate system 
          * is set to geographic, the origin is interpreted as (latitude, longitude,
          * and elevation).  If the terrain coordinate system is set to
          * CARTESIAN the origin is interpreted as (x,y,z).
          */
         void SetOrigin(const osg::Vec3d &origin);
         
         /**
          * Gets the origin of the terrain.
          * @return The origin of the terrain.
          */
         const osg::Vec3d &GetOrigin() const;
         
         /**
          * Gets the height of the terrain at the given (x,y) coordinates.
          * @note This just calls the current terrain renderer's GetHeight
          *    method.  If the renderer is invalid, the height returned is 0.0f.
          */
         float GetHeight(float x, float y);
         
         /**
          * Sets the path of the terrain cache.  The terrain cache is a directory
          * somewhere on the hard drive which is used to store on the fly data
          * calculations which can then be reused on successive uses.  If this
          * path is not set, no data caching will occur.
          * @note The directory will be created if it does not already exist.
          * @note Useful for data readers and/or terrain renderers that need a place
          *    for temporary data.
          * @param path The path to use for the terrain cache.
          * @throws Exception if the path does not exist and cannot be created.
          */
         bool SetCachePath(const std::string &path);
         
         /**
          * Gets the current terrain cache.
          */
         const std::string &GetCachePath() const { return mCachePath; }
         
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
            
      protected:
      
         ///Cleans up the terrain and its components.
         virtual ~Terrain();
   
      private:
         
         ///Full path to the terrain cache directory.
         std::string mCachePath;
         
         ///Path to the current data this terrain object is working with.
         std::string mDataPath;
         
         ///The current coordinate system type.
         const TerrainCoordinateSystem *mCoordinateSystem;
         
         ///The terrain's origin.
         osg::Vec3d mOrigin;
         
         ///The current terrain data reader.
         dtCore::RefPtr<TerrainDataReader> mDataReader;
         
         ///The current terrain data renderer.
         dtCore::RefPtr<TerrainDataRenderer> mDataRenderer;
         
         ///List of the layers currently attached to this terrain.
         std::map<std::string,dtCore::RefPtr<TerrainDecorationLayer> > mDecorationLayers;
   };

}

#endif

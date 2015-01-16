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
#ifndef DELTA_TERRAINDATARENDERER
#define DELTA_TERRAINDATARENDERER

#include <osg/Vec3>
#include <osg/Group>
#include "dtCore/base.h"
#include "dtCore/refptr.h"
#include "dtUtil/exception.h"
#include "dtTerrain/pagedterraintile.h"
#include "dtTerrain/terrain_export.h"

namespace dtTerrain
{
   class Terrain;
   
   class InvalidHeightfieldDataException : public dtUtil::Exception
   {
   public:
   	InvalidHeightfieldDataException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidHeightfieldDataException() {};
   };
   
   
   /**
    * This class is the interface for a terrain renderer.  A terrain renderer handles
    * rendering of the terrain.  It can access the terrain reader through its parent
    * terrain handle if data paging and such is required.  This class enables different
    * terrain rendering algorithms to operate transparently and independent of the 
    * underlying terrain data so they can be interchanged when needed.
    */
   class DT_TERRAIN_EXPORT TerrainDataRenderer : public dtCore::Base
   {
      
      public:
         
         /**
          * Constructs the terrain renderer.
          */
         TerrainDataRenderer(const std::string &name = "TerrainRenderer");
            
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
         virtual void OnLoadTerrainTile(PagedTerrainTile &tile) = 0;
         
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
          * Gets the height of the terrain at the specified (x,y) coordinates.
          * @return The height of the terrain at the specified point.
          */
         virtual float GetHeight(float x, float y) = 0;
         
         /**
          * Gets the normal vector at the specified point.
          * @return A vector perpendicular to terrain at the given point.
          */
         virtual osg::Vec3 GetNormal(float x, float y) = 0;
         
         /**
          * Returns a scene node that encapsulates the renderable terrain.  This 
          * is the entry point by which the terrain is added to the scene.
          * @return An OpenSceneGraph group node containing the renderer's 
          *    scene graph.
          * @note It is OK to return a group node with no children.  This is 
          *    most likely the case since this method is called before any tiles
          *    are inserted in the terrain's load queue.
          */
         virtual osg::Group *GetRootDrawable() = 0;       
                 
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
         virtual ~TerrainDataRenderer();
      
         ///Allow the terrain to have access to this class.
         friend class Terrain;
         
      private:
      
         /**
          * The terrain object that currently owns this reader.
          * @note Renderer instances can only be assigned to one terrain at a time.
          */
         Terrain *mParentTerrain;
   };
     
     
} 

#endif

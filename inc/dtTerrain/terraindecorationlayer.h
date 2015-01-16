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
#ifndef DELTA_TERRAINDECORATIONLAYER
#define DELTA_TERRAINDECORATIONLAYER

#include <osg/Node>
#include "dtCore/base.h"
#include "dtCore/refptr.h"
#include "dtTerrain/pagedterraintile.h"
#include "dtTerrain/terrain_export.h"

namespace dtTerrain
{
   class Terrain;
   
   /**
    * This class is a terrain decoration layer.  It provides the interface for providing layers
    * of decorations on the terrain from trees and shrubs to roads and buildings.  Note that 
    * any number of these layers may be added to the terrain so one may create many layers
    * each loading or manipulating an entirely different data set for that particular layer.
    */
   class DT_TERRAIN_EXPORT TerrainDecorationLayer : public dtCore::Base
   {
      public:
         
         /**
          * Constructs a new terrain decoration layer.
          */
         TerrainDecorationLayer(const std::string &name="TerrainDecorationLayer");
         
         /**
          * This method returns the root scene node of the decoration layer
          * scene graph.  This node then gets added as a child to its parent 
          * terrain.
          * @return Any OpenSceneGraph node.  Most likely a group node, but it
          *    could be any type of node.
          */
         virtual osg::Node *GetOSGNode() = 0;
         
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
          * This method is called when a tile has been completely loaded by
          * the terrain reader, terrain renderer, and all decorator layers.
          * This is useful if certain operations need to be performed on a tile
          * that is dependent on the tile being fully loaded by all terrain 
          * components.
          * @param The tile that was just loaded.
          */
         virtual void OnTerrainTileResident(PagedTerrainTile &tile) { }
         
         /**
          * Checks to see if this decoration layer is visible in the scene.
          * @return True if this layer is currently visible, false otherwise.
          */
         bool IsVisible() const 
         {
            return mIsVisible;
         }
         
         /**
          * Sets the view status of this decoration layer.  If set to false, it will not
          * show up on the terrain.
          * @param show If true, the decoration layer will be visible.
          */
         void SetVisible(bool show);
         
         /**
          * Gets the terrain object that currently owns this reader.
          * @return A pointer to the parent terrain.
          */
         dtTerrain::Terrain *GetParentTerrain() { return mParentTerrain; }
         
         /**
          * Gets a read-only terrain object that currently owns this reader.
          * @return A const pointer to the parent terrain.
          */
         const dtTerrain::Terrain *GetParentTerrain() const { return mParentTerrain; }
        
      protected:
      
         ///Destroys this decoration layer.
         virtual ~TerrainDecorationLayer();
         
         ///Allow the terrain to have access to this class.
         friend class Terrain;
         
      private:
      
         ///True if the layer is currently hidden.  By default layers are not hidden.
         bool mIsVisible;
         
         /**
          * The terrain object that currently owns this reader.
          * @note Renderer instances can only be assigned to one terrain at a time.
          */
         Terrain *mParentTerrain;
         
   };

}

#endif

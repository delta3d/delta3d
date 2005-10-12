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
#ifndef DELTA_TERRAINDECORATIONLAYER
#define DELTA_TERRAINDECORATIONLAYER

#include "dtCore/base.h"
#include "dtCore/refptr.h"
#include "dtTerrain/terrain_export.h"
#include "dtTerrain/lcctype.h"
//Foward declare necessary OSG classes.
namespace osg
{
   class Node;
}

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
          * This method is provided for decoration layers that must load external
          * data in order to generate the required scene data.
          * @param path A path to a resource.  Could be either a file or directory
          *    depending on the implementation of a particular decoration layer.
          */
         virtual void LoadResource(const std::string &path) = 0;
         
         virtual void LoadResource(int latitude, int longitude) = 0;

         /**
          * This method returns the root scene node of the decoration layer
          * scene graph.  This node then gets added as a child to its parent 
          * terrain.
          * @return Any OpenSceneGraph node.  Most likely a group node, but it
          *    could be any type of node.
          */
         virtual osg::Node *GetOSGNode() = 0;
         
         /**
          * This method is called if the data reader on this layer's parent 
          * terrain is changed.  Therefore, if this decoration layer relies on
          * the heightfield data loaded by the reader, it can update its internal
          * state here.
          * @note In other words, this method gets called if a new terrain data
          *    reader is assigned to the parent terrain or new data is loaded
          *    into the existing data reader.
          */
         virtual void OnTerrainDataReaderChanged() { }
         
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
         dtTerrain::Terrain *GetParentTerrain() { return mParentTerrain.get(); }
         
         /**
          * Gets a read-only terrain object that currently owns this reader.
          * @return A const pointer to the parent terrain.
          */
         const dtTerrain::Terrain *GetParentTerrain() const { return mParentTerrain.get(); }

         virtual void SetLCCData(std::vector<dtTerrain::LCCType> types){}
         
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
         dtCore::RefPtr<dtTerrain::Terrain> mParentTerrain;
         
   };

}

#endif

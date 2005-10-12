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
* @author Teague Coonan
*/
#ifndef DELTA_ROADDECORATOR
#define DELTA_ROADDECORATOR

#include <osg/Vec3>
#include <osg/Texture2D>
#include <dtCore/refptr.h>
#include "dtTerrain/terraindecorationlayer.h"

//Foward declare necessary OSG classes.
namespace osg
{
   class Node;
}

namespace dtTerrain
{

   /**
   * This class is the road decorator layer. It is responsible
   * for managing and manipulating the vegetation added to the terrain.
   */
   class DT_TERRAIN_EXPORT RoadDecorator : public TerrainDecorationLayer
   {
   public:
      /**
      * Constructs a new terrain decoration layer.
      */
      RoadDecorator(const std::string &name="RoadDecoratorLayer");

      /**
      * Determine the scene node of the vegetation layer
      * @return osg node
      */
      osg::Node *GetOSGNode()
      {
         return roadNode;
      }

      /**
      * Loads road data from the specified filename.
      *
      * @param filename the name of the vector data file to load
      * @param query a SQL query that selects road features from
      * the data file
      * @param width the width of the roads to create
      * @param texture the name of the texture to use, or "" for
      * none (in which case the roads will be baked into the base
      * texture)
      * @param sScale the texture s scale
      * @param tScale the texture t scale
      */
      void LoadRoads(std::string filename,
         std::string query = "SELECT * FROM *",
         float width = 7.0f,
         std::string texture = "",
         float sScale = 1.0f,
         float tScale = 1.0f);            

      /**
      * Makes roads for the specified segment.
      *
      * @param latitude the latitude of the terrain segment
      * @param longitude the longitude of the terrain segment
      * @param origin the origin of the terrain cell
      * @return the newly created road node
      */
      dtCore::RefPtr<osg::Geode> MakeRoads(int latitude, int longitude, const osg::Vec3& origin);

   protected:
      /** 
      * Destructor
      */
      virtual ~RoadDecorator();

   private:
      osg::Node *roadNode;
   };
}
#endif

/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2008 MOVES Institute
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
*/

#ifndef GEOMETRY_COLLECTOR
#define GEOMETRY_COLLECTOR

#include <osg/NodeVisitor>
#include <osg/Geode>

namespace dtUtil
{
   // Node visitor that gather all geometry contained within a subgraph
   class GeometryCollector : public osg::NodeVisitor
   {
   public:

      GeometryCollector()
         : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN){}

      virtual void apply(osg::Geode& node)
      {
         int numberOfDrawables = node.getNumDrawables();

         for (int drawableIndex = 0; drawableIndex < numberOfDrawables; ++drawableIndex)
         {
            // If this is geometry, get a pointer to it
            osg::Geometry* geom = node.getDrawable(drawableIndex)->asGeometry();

            if (geom)
            {
               // store the geometry in this list
               mGeomList.push_back(geom);
            }
         }

         traverse(node);
      }

      std::vector<osg::Geometry*> mGeomList;
   };
}

#endif // GEOMETRY_COLLECTOR

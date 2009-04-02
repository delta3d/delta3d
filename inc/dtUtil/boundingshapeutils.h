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

#ifndef BOUNDING_SHAPE_UTILS
#define BOUNDING_SHAPE_UTILS

#include <osg/NodeVisitor>
#include <osg/BoundingBox>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Version>

namespace dtUtil
{
   class BoundingBoxVisitor : public osg::NodeVisitor
   {
   public:

      BoundingBoxVisitor()
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      {}

      /**
      * Visits the specified geode.
      *
      * @param node the geode to visit
      */
      virtual void apply(osg::Geode& node)
      {
         osg::NodePath nodePath = getNodePath();

#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR == 0
         // Luckily, this behavior is redundant with OSG 1.1
         if (std::string(nodePath[0]->className()) == std::string("CameraNode"))
         {
            nodePath = osg::NodePath(nodePath.begin()+1, nodePath.end());
         }
#endif // OSG 1.1

         osg::Matrix matrix = osg::computeLocalToWorld(nodePath);

         for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
         {
            for (unsigned int j = 0; j < 8; ++j)
            {
               mBoundingBox.expandBy(node.getDrawable(i)->getBound().corner(j) * matrix);
            }
         }
      }

      /**
      * The aggregate bounding box.
      */
      osg::BoundingBox mBoundingBox;
   };

}

#endif // BOUNDING_SHAPE_UTILS

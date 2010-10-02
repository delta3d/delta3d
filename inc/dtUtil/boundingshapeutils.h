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

#include <osg/ComputeBoundsVisitor>
#include <osg/BoundingBox>

namespace dtUtil
{
   //TODO: Deprecate me in favor of osg::ComputeBoundsVisitor
   class BoundingBoxVisitor : public osg::ComputeBoundsVisitor
   {
   public:

      BoundingBoxVisitor(TraversalMode tMode = osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
         : osg::ComputeBoundsVisitor()
         , mBoundingBox(_bb)  //little hack to maintain backwards compatibility
      {
      }      

      osg::BoundingBox& mBoundingBox;
   };   
}

#endif // BOUNDING_SHAPE_UTILS

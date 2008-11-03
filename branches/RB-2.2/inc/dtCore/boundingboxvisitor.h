/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2005 MOVES Institute
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

#ifndef DELTA_BOUNDING_BOX
#define DELTA_BOUNDING_BOX

#include <osg/NodeVisitor>
#include <osg/BoundingBox>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Version>

#include <dtUtil/boundingshapeutils.h>
#include <dtUtil/deprecationmgr.h>

namespace dtCore
{
   class BoundingBoxVisitor : public osg::NodeVisitor
   {
   public:

      BoundingBoxVisitor()
      {
         DEPRECATE("dtCore::BoundingBoxVisitor()", "dtUtil::BoundingBoxVisitor()");
         mBoxVisitor = new dtUtil::BoundingBoxVisitor;
      }

      /**
      * Visits the specified geode.
      *
      * @param node the geode to visit
      */
      virtual void apply(osg::Geode& node)
      {
         DEPRECATE("dtCore::BoundingBoxVisitor::apply(osg::Geode& node)",
            "dtUtil::BoundingBoxVisitor::apply(osg::Geode& node)");

         mBoxVisitor->apply(node);

         this->mBoundingBox = mBoxVisitor->mBoundingBox;
      }

      dtCore::RefPtr<dtUtil::BoundingBoxVisitor> mBoxVisitor;

      /**
      * The aggregate bounding box.
      */
      osg::BoundingBox mBoundingBox;
   };

}

#endif

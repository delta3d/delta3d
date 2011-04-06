/*
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

#include <dtCore/export.h>
#include <osg/Transform>

#ifndef DELTA_MOVEEARTHYSKYWITHEYEPOINTTRANSFORM
#define DELTA_MOVEEARTHYSKYWITHEYEPOINTTRANSFORM

/// From osghangglide

namespace dtCore
{
   class DT_CORE_EXPORT MoveEarthySkyWithEyePointTransform : public osg::Transform
   {
      public:

         ///Get the transformation matrix which moves from local coords to world coords.
         virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;

         ///Get the transformation matrix which moves from world coords to local coords.
         virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;
   };

   class DT_CORE_EXPORT MoveEarthySkyWithEyePointTransformAzimuth : public osg::Transform
   {
      public:

         ///Get the transformation matrix which moves from local coords to world coords.
         virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;

         ///Get the transformation matrix which moves from world coords to local coords.
         virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;

         float GetAzimuth() const { return mAzimuth; }
         void SetAzimuth( float azimuth ) { mAzimuth = azimuth; }

      private:

         float mAzimuth;
   };
}

#endif // DELTA_MOVEEARTHYSKYWITHEYEPOINTTRANSFORM

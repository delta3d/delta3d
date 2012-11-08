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

#include <prefix/dtcoreprefix.h>
#include <dtCore/moveearthtransform.h>
#include <osgUtil/CullVisitor>

/// From osghangglide

namespace dtCore
{
   ///Get the transformation matrix which moves from local coords to world coords.
   bool MoveEarthySkyWithEyePointTransform::computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
   {
      if ( osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv) )
      {
         osg::Vec3 eyePointLocal = cv->getEyeLocal();
         matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
      }
      return true;
   }

   ///Get the transformation matrix which moves from world coords to local coords.
   bool MoveEarthySkyWithEyePointTransform::computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
   {
      if ( osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv) )
      {
         osg::Vec3 eyePointLocal = cv->getEyeLocal();
         matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
      }
      return true;
   }

   ///Get the transformation matrix which moves from local coords to world coords.
   bool MoveEarthySkyWithEyePointTransformAzimuth::computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
   {
      if ( osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv) )
      {
         osg::Vec3 eyePointLocal = cv->getEyeLocal();
         matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
         matrix.preMult(osg::Matrix::rotate(osg::DegreesToRadians(90.0f - mAzimuth), 0.0f, 0.0f, 1.0f));
      }
      return true;
   }

   ///Get the transformation matrix which moves from world coords to local coords.
   bool MoveEarthySkyWithEyePointTransformAzimuth::computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
   {
      if ( osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv) )
      {
         osg::Vec3 eyePointLocal = cv->getEyeLocal();
         matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
      }
      return true;
   }
}

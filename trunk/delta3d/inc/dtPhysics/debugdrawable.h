/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * david
 */

#ifndef DEBUGDRAWABLE_H_
#define DEBUGDRAWABLE_H_

#include <dtCore/deltadrawable.h>
#include <osg/Group>
#include <osg/Geometry>
#include <dtPhysics/physicstypes.h>

class palDebugGeometry;

namespace dtPhysics
{

   class DebugDrawable: public dtCore::DeltaDrawable
   {
   public:
      DebugDrawable();
      virtual ~DebugDrawable();

      virtual osg::Node* GetOSGNode();
      virtual const osg::Node* GetOSGNode() const;

      void SetReferencePosition(const osg::Vec3& referencePos);
      void GetReferencePosition(osg::Vec3& referencePosOut) const;

      void Update(dtPhysics::DebugDraw& debugDraw);

   private:
      void UpdateGeometry(osg::Geometry& geomDrawable, palDebugGeometry& debugGeom);

      dtCore::RefPtr<osg::Group> mDebugGroup;
      dtCore::RefPtr<osg::Geometry> mTriangles;
      dtCore::RefPtr<osg::Geometry> mLines;
      dtCore::RefPtr<osg::Geometry> mPoints;

      osg::Vec3 mReferencePos;
   };

}

#endif /* DEBUGDRAWABLE_H_ */

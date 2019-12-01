/* -*-c++-*-
 * dtPhysics
 * Copyright 2015, Caper Holdings, LLC
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
 * David Guthrie
 */

#ifndef DTPHYSICS_TRANSFORMJOINTUPDATER_H_
#define DTPHYSICS_TRANSFORMJOINTUPDATER_H_

#include <dtPhysics/physicsexport.h>
#include <dtCore/observerptr.h>
#include <dtCore/transformpropertydelegate.h>
#include <dtUtil/getsetmacros.h>
#include <osg/Referenced>
#include <osg/Transform>

class palLink;

namespace dtPhysics
{
   class DT_PHYSICS_EXPORT TransformJointUpdater : public dtCore::TransformPropertyDelegate
   {
   public:
      typedef dtCore::TransformPropertyDelegate BaseClass;

      TransformJointUpdater(osg::Transform& transformNode, palLink& mJoint);
      virtual ~TransformJointUpdater();

      virtual void operator()();

      palLink* GetJoint() const { return mJoint; }

      DT_DECLARE_ACCESSOR(bool, DeleteJoint);

   private:
      palLink* mJoint;
   };

   typedef dtCore::RefPtr<TransformJointUpdater> TransformJointUpdaterPtr;

} /* namespace dtPhysics */

#endif /* DTPHYSICS_TRANSFORMJOINTUPDATER_H_ */

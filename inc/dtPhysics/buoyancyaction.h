/* -*-c++-*-
 * dtPhysics
 * Copyright 2014, Caper Holdings, LLC
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

#ifndef DELTA_BUOYANCY_ACTION_H_
#define DELTA_BUOYANCY_ACTION_H_

#include <dtPhysics/physicsexport.h>
#include <osg/Referenced>
#include <dtUtil/getsetmacros.h>

class palFakeBuoyancy;
class palLiquidDrag;
class palWaterHeightQuery;

namespace dtPhysics
{
   class PhysicsObject;

   class DT_PHYSICS_EXPORT BuoyancyAction: public osg::Referenced
   {
   public:
      BuoyancyAction();

      /**
       * This gets the physics act comp from the po and registers the action, then uses this to apply the operator.
       * Only single box and sphere shapes are supported at the moment.
       */
      virtual void Register(dtPhysics::PhysicsObject& po);

      palWaterHeightQuery* GetWaterHeightQuery();
      void SetWaterHeightQuery(palWaterHeightQuery* query);

      DT_DECLARE_ACCESSOR(float, LiquidDensityGramsPerSqCm);
      DT_DECLARE_ACCESSOR(float, CD);
      DT_DECLARE_ACCESSOR(float, SurfaceAreaSqM);

      virtual void Update(float dt);

   protected:
      virtual ~BuoyancyAction();
      palFakeBuoyancy* mBuoyancy;
      palLiquidDrag* mDrag;
   };

} /* namespace dtPhysics */

#endif /* BUOYANCYACTION_H_ */

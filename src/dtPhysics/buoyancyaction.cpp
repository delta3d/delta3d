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
 *
 * David Guthrie
 */

#include <dtPhysics/buoyancyaction.h>
#include <pal/palActuators.h>
#include <pal/palFactory.h>
#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/bodywrapper.h>


#include <cassert>

namespace dtPhysics
{

   ///////////////////////////////////////////////
   BuoyancyAction::BuoyancyAction()
   : mBuoyancy()
   , mDrag()
   {
   }

   ///////////////////////////////////////////////
   BuoyancyAction::~BuoyancyAction()
   {
      delete mBuoyancy;
      mBuoyancy = NULL;
      delete mDrag;
      mDrag = NULL;
   }

   ///////////////////////////////////////////////
   void BuoyancyAction::Register(dtPhysics::PhysicsObject& po)
   {
      delete mBuoyancy;
      mBuoyancy = NULL;
      // There is no reason to do this if the physics object is not dynamic.
      if (po.GetBodyWrapper() != NULL && po.GetMechanicsType() == MechanicsType::DYNAMIC)
      {
         palBody& body = po.GetBodyWrapper()->GetPalBody();
         mBuoyancy = dynamic_cast<palFakeBuoyancy*>(palFactory::GetInstance()->CreateObject("palFakeBuoyancy"));
         assert(mBuoyancy != NULL);
         mDrag = dynamic_cast<palLiquidDrag*>(palFactory::GetInstance()->CreateObject("palLiquidDrag"));
         assert(mDrag != NULL);

         mDrag->Init(&body,200.2, 0.47, 0.998232);
         mBuoyancy->Init(&body, 998.232, mDrag);
         dtPhysics::PhysicsActComp* pac = dynamic_cast<dtPhysics::PhysicsActComp*>(po.GetUserData());
         if (pac != NULL)
         {
            pac->SetActionUpdateCallback(PhysicsActComp::ActionUpdateCallback(this, &BuoyancyAction::Update));
         }
      }
   }

   ///////////////////////////////////////////////
   palWaterHeightQuery* BuoyancyAction::GetWaterHeightQuery()
   {
      if (mBuoyancy != NULL)
      {
         mBuoyancy->GetWaterHeightQuery();
      }
      return NULL;
   }

   ///////////////////////////////////////////////
   void BuoyancyAction::SetWaterHeightQuery(palWaterHeightQuery* query)
   {
      if (mBuoyancy != NULL)
      {
         mBuoyancy->SetWaterHeightQuery(query);
      }
   }
   ///////////////////////////////////////////////
   void BuoyancyAction::Update(float dt)
   {
      if (mBuoyancy != NULL)
      {
         mBuoyancy->Apply(dt);
      }
   }

} /* namespace dtPhysics */

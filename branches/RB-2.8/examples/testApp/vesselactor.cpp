/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "vesselactor.h"
#include "surfacevesselactorcomponent.h"

#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/buoyancyaction.h>
#include <dtActors/watergridactor.h>
#include <dtActors/engineactorregistry.h>
#include "wateractorheightquery.h"
#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>

namespace dtExample
{

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   VesselActor::VesselActor()
   {}

   VesselActor::~VesselActor()
   {}

   void VesselActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();

      dtCore::RefPtr<SurfaceVesselActorComponent> comp = new SurfaceVesselActorComponent;
      AddComponent(*comp);
   }
   void VesselActor::OnEnteredWorld()
   {
      RegisterForMessages(dtGame::MessageType::INFO_MAP_CHANGE_LOAD_END, dtUtil::MakeFunctor(&VesselActor::OnMapLoaded, this));
      RegisterForMessages(dtGame::MessageType::INFO_MAPS_OPENED, dtUtil::MakeFunctor(&VesselActor::OnMapLoaded, this));
   }

   void VesselActor::OnMapLoaded(const dtGame::MapMessage& /*mapMessage*/)
   {
      if (mBuoyancyAction == NULL)
      {
         mBuoyancyAction = new dtPhysics::BuoyancyAction();
         mBuoyancyAction->Register(*GetComponent<dtPhysics::PhysicsActComp>()->GetMainPhysicsObject());
         dtActors::WaterGridActorProxy* waterGrid = NULL;
         GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::WATER_GRID_ACTOR_TYPE, waterGrid);
         if (waterGrid != NULL)
         {
            mBuoyancyAction->SetWaterHeightQuery(new WaterActorHeightQuery(*waterGrid->GetDrawable<dtActors::WaterGridActor>()));
         }
      }
   }


}

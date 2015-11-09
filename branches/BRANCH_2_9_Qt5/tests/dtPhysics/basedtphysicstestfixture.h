/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 */

#ifndef TESTS_DTPHYSICS_BASEDTPHYSICSTESTFIXTURE_H_
#define TESTS_DTPHYSICS_BASEDTPHYSICSTESTFIXTURE_H_

#include "../dtGame/basegmtests.h"
#include <dtPhysics/physicscomponent.h>
#include <dtPhysics/palphysicsworld.h>

extern const std::vector<std::string>& GetPhysicsEngineList();

namespace dtPhysics
{
   class BaseDTPhysicsTestFixture : public dtGame::BaseGMTestFixture
   {

   public:
      typedef dtGame::BaseGMTestFixture BaseClass;

      void GetRequiredLibraries(NameVector& names) override
      {
         static const std::string dtPhysicsLib("dtPhysics");
         names.push_back(dtPhysicsLib);
      }

      void tearDown() override
      {
         mPhysicsComp = NULL;
         BaseClass::tearDown();
      }

   protected:

      void ChangeEngine(const std::string& engine)
      {
         try
         {
            dtGame::GMComponent* comp = mGM->GetComponentByName(PhysicsComponent::DEFAULT_NAME);
            if (comp != NULL)
            {
               mGM->RemoveComponent(*comp);
            }
            mPhysicsComp = NULL;

            dtCore::RefPtr<PhysicsWorld> world = new PhysicsWorld(engine);
            world->Init();

            mPhysicsComp = new PhysicsComponent(*world, false);

            mGM->AddComponent(*mPhysicsComp, dtGame::GameManager::ComponentPriority::NORMAL);
         }
         catch (dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }
      dtCore::RefPtr<dtPhysics::PhysicsComponent> mPhysicsComp;
   };

}

#endif /* TESTS_DTPHYSICS_BASEDTPHYSICSTESTFIXTURE_H_ */

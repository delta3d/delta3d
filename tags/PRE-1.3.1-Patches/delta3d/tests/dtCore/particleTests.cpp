/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute 
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
 * @author Allen Danklefsen
 */
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/globals.h>
#include <osgParticle/ModularEmitter>
#include <dtCore/particlesystem.h>   // for testing the Default particle template methods

namespace dtTest
{
   /// unit tests for dtCore::Button
   class ParticleTest : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ParticleTest );
      CPPUNIT_TEST( TestTemplates );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();
         void TestTemplates();
      private:
         dtCore::RefPtr<dtCore::ParticleSystem> pSystem;
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::ParticleTest );

using namespace dtTest;

void ParticleTest::setUp()
{
   pSystem = new dtCore::ParticleSystem();
   pSystem->LoadFile(dtCore::GetDeltaDataPathList() + "/effects/smoke.osg");
}

void ParticleTest::tearDown()
{}

void ParticleTest::TestTemplates()
{
   const std::string STRING_ONE = "Layer 0";
   const std::string STRING_TWO = "Layer 1";
   const float ChangeOne = 15.0f;
   const float ChangeTwo = 25.0f;
   
   CPPUNIT_ASSERT_MESSAGE("First test if the particle system is valid", pSystem);

   // Get a particle node we know exist!
   dtCore::ParticleLayer& particleLayer = *pSystem->GetSingleLayer(STRING_ONE);
   CPPUNIT_ASSERT_MESSAGE("Check if it can find a layer we know exist.", pSystem->GetSingleLayer(STRING_ONE) != NULL );

   // Check if we can find a node that doesnt exist
   CPPUNIT_ASSERT_MESSAGE("Found a template that didnt exist!", 
                           pSystem->GetSingleLayer(STRING_TWO) == NULL );

   // make sure we can get the file multiple times with the same result
   CPPUNIT_ASSERT_MESSAGE("Didnt find the same value for a known system",
                           particleLayer.GetParticleSystem().getDefaultParticleTemplate().getAngle() == 
                           pSystem->GetSingleLayer(STRING_ONE)->GetParticleSystem().getDefaultParticleTemplate().getAngle());

   // Change two numbers, re-feed them in
   particleLayer.GetParticleSystem().getDefaultParticleTemplate().setLifeTime(ChangeOne);
   particleLayer.GetParticleSystem().getDefaultParticleTemplate().setMass(ChangeTwo);
}

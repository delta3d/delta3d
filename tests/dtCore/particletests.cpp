/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
*
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author Allen Danklefsen, Chris Rodgers
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <osg/Geode>
#include <osgParticle/ModularEmitter>
#include <dtUtil/datapathutils.h>
#include <dtCore/particlesystem.h>   // for testing the Default particle template methods

namespace dtTest
{
   /// unit tests for dtCore::Button
   class ParticleTest : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(ParticleTest);
      CPPUNIT_TEST(TestProperties);
      CPPUNIT_TEST(TestTemplates);
      CPPUNIT_TEST(TestClone);
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();
         void TestProperties();
         void TestClone();
         void TestTemplates();
      private:
         dtCore::RefPtr<dtCore::ParticleSystem> pSystem;
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION(dtTest::ParticleTest);

using namespace dtTest;

void ParticleTest::setUp()
{
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList() + ";" + dtUtil::GetDeltaRootPath() + "/examples/data/;");
   pSystem = new dtCore::ParticleSystem();
   std::string smokeFile = dtUtil::FindFileInPathList("Particles/smoke.osg");
   CPPUNIT_ASSERT(!smokeFile.empty());
   pSystem->LoadFile(smokeFile, true);
}

void ParticleTest::tearDown()
{}

void ParticleTest::TestProperties()
{
   const std::string LAYER_0 = "Layer 0";
   const std::string LAYER_FAKE = "Layer Fake";
   const dtCore::ParticleSystem* psConst = pSystem.get();

   CPPUNIT_ASSERT_MESSAGE("Particle System should be enabled by default.",
      psConst->IsEnabled() );
   pSystem->SetEnabled( false );
   CPPUNIT_ASSERT( ! psConst->IsEnabled() );

   CPPUNIT_ASSERT_MESSAGE("Particle System should NOT be in parent-relative space by default.",
      ! psConst->IsParentRelative() );
   pSystem->SetParentRelative( true );
   CPPUNIT_ASSERT( psConst->IsParentRelative() );

   CPPUNIT_ASSERT_MESSAGE( "Particle System should have loaded some layers.",
      ! psConst->GetAllLayers().empty() );

   CPPUNIT_ASSERT_MESSAGE( "Particle System should be able to load a single layer.",
      psConst->GetSingleLayer(LAYER_0) != NULL );

   CPPUNIT_ASSERT_MESSAGE( "Particle System should return NULL if a layer is not found.",
      psConst->GetSingleLayer(LAYER_FAKE) == NULL );
}

void ParticleTest::TestClone()
{
   std::string smokeFile = pSystem->GetFilename();
   CPPUNIT_ASSERT(!smokeFile.empty());

   osg::Node* clonedPS = pSystem->GetLoadedParticleSystemRoot();
   dtCore::RefPtr<osg::Node> originalPS = pSystem->GetCachedOriginalParticleSystemRoot();
   CPPUNIT_ASSERT(clonedPS != NULL);
   CPPUNIT_ASSERT(originalPS.valid());
   CPPUNIT_ASSERT(originalPS != clonedPS);

   pSystem->LoadFile(smokeFile, false);

   clonedPS = pSystem->GetLoadedParticleSystemRoot();

   CPPUNIT_ASSERT(clonedPS != NULL);
   CPPUNIT_ASSERT(pSystem->GetCachedOriginalParticleSystemRoot() == NULL);

   pSystem->LoadFile(smokeFile, true);

   CPPUNIT_ASSERT_MESSAGE("The original should be the same again because it should be cached.",
            originalPS == pSystem->GetCachedOriginalParticleSystemRoot());

   dtCore::ParticleSystem::LayerList origLayers;
   dtCore::RefPtr<osgParticle::ParticleSystemUpdater> origUpdater;
   dtCore::ParticleSystem::ParseParticleLayers(*originalPS, origLayers, origUpdater);

   CPPUNIT_ASSERT_EQUAL(pSystem->GetAllLayers().size(), origLayers.size());
   CPPUNIT_ASSERT(pSystem->GetParticleSystemUpdater() != NULL);
   CPPUNIT_ASSERT(origUpdater.valid());
   CPPUNIT_ASSERT_EQUAL(pSystem->GetParticleSystemUpdater()->getNumParticleSystems(), origUpdater->getNumParticleSystems());

   for(unsigned j = 0; j < origUpdater->getNumParticleSystems(); ++j)
   {
      const osgParticle::ParticleSystem* oldPS = origUpdater->getParticleSystem(j);
      CPPUNIT_ASSERT(oldPS != NULL);
      CPPUNIT_ASSERT_MESSAGE("The new ParticleSystemUpdater should not contain any of the old particle systems",
         pSystem->GetParticleSystemUpdater()->getParticleSystemIndex(oldPS) ==
         pSystem->GetParticleSystemUpdater()->getNumParticleSystems());
   }

   dtCore::ParticleSystem::LayerList::const_iterator i, iend, origI, origEnd;
   i = pSystem->GetAllLayers().begin();
   iend = pSystem->GetAllLayers().end();
   origI = origLayers.begin();
   origEnd = origLayers.end();
   for (; i != iend; ++i)//, ++origI)
   {
      CPPUNIT_ASSERT_MESSAGE("The original list of layers should match the new one, but the lists are not the same size.",
               origI != origEnd);
      const dtCore::ParticleLayer& layer = *i;
      const osgParticle::ParticleSystem& ps = layer.GetParticleSystem();
      CPPUNIT_ASSERT_MESSAGE("The ParticleSystemUpdater should have the new particle system",
               pSystem->GetParticleSystemUpdater()->getParticleSystemIndex(&ps) <
               pSystem->GetParticleSystemUpdater()->getNumParticleSystems());
      CPPUNIT_ASSERT_MESSAGE("The Geode should have the new particle system",
               layer.GetGeode().getDrawableIndex(&ps) < layer.GetGeode().getNumDrawables());

      for(unsigned j = 0; j < origUpdater->getNumParticleSystems(); ++j)
      {
         const osgParticle::ParticleSystem* oldPS = origUpdater->getParticleSystem(j);
         CPPUNIT_ASSERT(oldPS != NULL);
         CPPUNIT_ASSERT_MESSAGE("The new Geode should not contain any of the old particle systems",
            layer.GetGeode().getDrawableIndex(oldPS) == layer.GetGeode().getNumDrawables());
      }

      CPPUNIT_ASSERT(layer.GetModularEmitter().getParticleSystem() == &ps);
      CPPUNIT_ASSERT(layer.GetProgram().getParticleSystem() == &ps);

      const dtCore::ParticleLayer& origLayer = *origI;
      CPPUNIT_ASSERT_EQUAL(origLayer.GetLayerName(), layer.GetLayerName());
      CPPUNIT_ASSERT_EQUAL(origLayer.IsFluidProgram(), layer.IsFluidProgram());
      CPPUNIT_ASSERT_EQUAL(origLayer.IsModularProgram(), layer.IsModularProgram());
   }
}

void ParticleTest::TestTemplates()
{
   const std::string STRING_ONE = "Layer 0";
   const std::string STRING_TWO = "Layer 1";
   const float ChangeOne = 15.0f;
   const float ChangeTwo = 25.0f;
   const dtCore::ParticleSystem* psConst = pSystem.get();

   CPPUNIT_ASSERT_MESSAGE("First test if the particle system is valid", pSystem);

   // Get a particle node we know exist!
   dtCore::ParticleLayer& particleLayer = *pSystem->GetSingleLayer(STRING_ONE);
   CPPUNIT_ASSERT_MESSAGE("Check if it can find a layer we know exist.", pSystem->GetSingleLayer(STRING_ONE) != NULL );

   // Check if we can find a node that does not exist
   CPPUNIT_ASSERT_MESSAGE("Found a template that did not exist!",
                           psConst->GetSingleLayer(STRING_TWO) == NULL );

   // make sure we can get the file multiple times with the same result
   CPPUNIT_ASSERT_MESSAGE("Did not find the same value for a known system.",
                           particleLayer.GetParticleSystem().getDefaultParticleTemplate().getAngle() ==
                           psConst->GetSingleLayer(STRING_ONE)->GetParticleSystem().getDefaultParticleTemplate().getAngle());

   // Change two numbers, re-feed them in
   particleLayer.GetParticleSystem().getDefaultParticleTemplate().setLifeTime(ChangeOne);
   particleLayer.GetParticleSystem().getDefaultParticleTemplate().setMass(ChangeTwo);
}

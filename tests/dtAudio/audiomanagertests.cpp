/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
* @author William E. Johnson II
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/audiomanager.h>

class AudioManagerTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(AudioManagerTests);

      CPPUNIT_TEST(TestInitialize);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void TestInitialize();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AudioManagerTests);

void AudioManagerTests::setUp()
{
   if(dtAudio::AudioManager::GetInstance().IsInitialized())
   {
      dtAudio::AudioManager::GetInstance().Destroy();
   }
}

void AudioManagerTests::tearDown()
{
   if(dtAudio::AudioManager::GetInstance().IsInitialized())
   {
      dtAudio::AudioManager::GetInstance().Destroy();
   }
}

void AudioManagerTests::TestInitialize()
{
   CPPUNIT_ASSERT(!dtAudio::AudioManager::GetInstance().IsInitialized());

   dtAudio::AudioManager::Instantiate();

   CPPUNIT_ASSERT(dtAudio::AudioManager::GetInstance().IsInitialized());

   dtAudio::AudioManager::Destroy();

   CPPUNIT_ASSERT(!dtAudio::AudioManager::GetInstance().IsInitialized());
}

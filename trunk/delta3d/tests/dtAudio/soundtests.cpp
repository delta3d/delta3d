/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 *
 * David Guthrie
 */

#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/audiomanager.h>

namespace dtAudio
{
   class SoundTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(SoundTests);

         CPPUNIT_TEST(TestInitialize);
         CPPUNIT_TEST(TestPitch);

      CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();

      void TestInitialize();
      void TestPitch();
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(SoundTests);

   void SoundTests::setUp()
   {
      if (!AudioManager::IsInitialized())
      {
         AudioManager::Instantiate();
      }
   }

   void SoundTests::tearDown()
   {
      if (AudioManager::IsInitialized())
      {
         AudioManager::GetInstance().Destroy();
      }
   }

   void SoundTests::TestInitialize()
   {
      dtCore::RefPtr<Sound> sound = AudioManager::GetInstance().NewSound();
      try
      {
         CPPUNIT_ASSERT(sound.valid());
         CPPUNIT_ASSERT(sound->GetActive());
         CPPUNIT_ASSERT(!sound->IsInitialized());
         CPPUNIT_ASSERT(!sound->IsListenerRelative());
         CPPUNIT_ASSERT(!sound->IsLooping());
         CPPUNIT_ASSERT(!sound->IsPaused());
         CPPUNIT_ASSERT(!sound->IsPlaying());
         CPPUNIT_ASSERT(sound->IsStopped());

         sound->SetInitialized(true);
         CPPUNIT_ASSERT(sound->IsInitialized());
         AudioManager::GetInstance().FreeSound(sound.release());
      }
      catch(...)
      {
         AudioManager::GetInstance().FreeSound(sound.release());
         throw;
      }
  }

   void SoundTests::TestPitch()
   {
      dtCore::RefPtr<Sound> sound = AudioManager::GetInstance().NewSound();
      try
      {
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The pitch should default 1.0", 1.0f, sound->GetPitch(), 0.001f);
         sound->SetPitch(0.01f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01f, sound->GetPitch(), 0.00001f);
         sound->SetPitch(1.5f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5f, sound->GetPitch(), 0.00001f);
         sound->SetPitch(128.0f);
         sound->SetPitch(128.0f);
         sound->SetPitch(128.0f);
         sound->SetPitch(128.0f);
         CPPUNIT_ASSERT_MESSAGE("The sound pitch must be greater than or equal to 2.0f. "
                  "It should be 128, but some implementations, won't do > 2.0.  The code should handle that internally.",
                  sound->GetPitch() >= 2.0f);
         AudioManager::GetInstance().FreeSound(sound.release());
      }
      catch(...)
      {
         AudioManager::GetInstance().FreeSound(sound.release());
         throw;
      }
   }
}

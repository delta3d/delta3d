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

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/audiomanager.h>

namespace dtAudio
{
   class SoundTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(SoundTests);
         // commented out because the load test will run forever on some systems.
         //CPPUNIT_TEST(LoadTest);
         CPPUNIT_TEST(TestInitialize);
         CPPUNIT_TEST(TestPitch);
         CPPUNIT_TEST(TestState);
      CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();

      void LoadTest();
      void TestInitialize();
      void TestPitch();
      void TestState(); 

   private:
      std::string mTestSoundPath;   
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(SoundTests);

   void SoundTests::setUp()
   {
      std::string unitTestDataFilePath = getenv("DELTA_ROOT");
      unitTestDataFilePath += "/tests/data/";
      mTestSoundPath = unitTestDataFilePath + "Sounds/silence.wav";

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

   //allocate and play a tremendous number of Sounds 
   //and see how Delta + Creative handle it
   void SoundTests::LoadTest()
   {
      std::vector<dtCore::RefPtr<Sound> > sounds;

      for (unsigned int i = 0; i < 999999; ++i)
      {         
         dtCore::RefPtr<Sound> nextSound = AudioManager::GetInstance().NewSound();
         sounds.push_back(nextSound);

         nextSound->LoadFile(mTestSoundPath.c_str());
         nextSound->SetLooping(true);
         if (! nextSound->PlayImmediately())
         {
            std::cout << "\n\nYour sound hardware supports " << i 
               << " sound sources at a time.\n\n";
            break;
         }
      }

      for (unsigned int i = 0; i < sounds.size(); ++i)
      {
         AudioManager::GetInstance().FreeSound(sounds[i].get());
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

   void SoundTests::TestState()
   {
      dtCore::RefPtr<Sound> sound = AudioManager::GetInstance().NewSound();
      dtCore::RefPtr<Sound> customSound = AudioManager::GetInstance().NewSound();

      //store all state variables for Sounds at startup:
      ALfloat sndGain = sound->GetGain();      
      ALfloat sndPitch = sound->GetPitch();      
      ALfloat sndOffset = sound->GetPlayTimeOffset();      
      ALfloat sndMaxDis = sound->GetMaxDistance();
      ALfloat sndRolloff = sound->GetRolloffFactor();
      ALfloat sndMinGain = sound->GetMinGain();
      ALfloat sndMaxGain = sound->GetMaxGain();
      bool sndListRel = sound->IsListenerRelative();
      
      ALfloat cSndGain = customSound->GetGain();
      ALfloat cSndPitch = customSound->GetPitch();
      ALfloat cSndOffset = customSound->GetPlayTimeOffset();
      ALfloat cSndMaxDis = customSound->GetMaxDistance();
      ALfloat cSndRolloff = customSound->GetRolloffFactor();
      ALfloat cSndMinGain = customSound->GetMinGain();
      ALfloat cSndMaxGain = customSound->GetMaxGain();
      bool cSndListRel = customSound->IsListenerRelative();
      
      //sound source shouldn't have been allocated yet
      //(only allocates when played):
      CPPUNIT_ASSERT(AL_NONE == sound->GetSource());
      CPPUNIT_ASSERT(AL_NONE == customSound->GetSource());

      //Haven't loaded a buffer, verify that:
      CPPUNIT_ASSERT(AL_NONE == sound->GetBuffer());
      CPPUNIT_ASSERT(AL_NONE == customSound->GetBuffer());

      sound->LoadFile(mTestSoundPath.c_str());
      customSound->LoadFile(mTestSoundPath.c_str());

      CPPUNIT_ASSERT(sound->GetBuffer());
      CPPUNIT_ASSERT(customSound->GetBuffer());

      //Verify that the sound DOES have its source deallocated after a Stop
      sound->PlayImmediately();
      sound->StopImmediately();
      sound->PlayImmediately();
      sound->StopImmediately();
      CPPUNIT_ASSERT(AL_NONE == sound->GetSource());

      // clear any errors.
      alGetError();
      //Create a custom source and ensure that its original source DOESN'T ever
      //ever get deallocated after a Stop
      ALuint cSrc;
      alGenSources(1, &cSrc);
      CPPUNIT_ASSERT(AL_NO_ERROR == alGetError());

      customSound->SetSource(cSrc);
      customSound->PlayImmediately();
      customSound->StopImmediately();
      customSound->PlayImmediately();
      customSound->StopImmediately();
      CPPUNIT_ASSERT(cSrc == customSound->GetSource());

      //Sounds have now been toyed about with a bit.  
      //Verify that all state variables are still correct. 
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndGain, sound->GetGain(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndPitch, sound->GetPitch(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndOffset, sound->GetPlayTimeOffset(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndMaxDis, sound->GetMaxDistance(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndRolloff, sound->GetRolloffFactor(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndMinGain, sound->GetMinGain(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndMaxGain, sound->GetMaxGain(), 0.000001);
      CPPUNIT_ASSERT(sndListRel == sound->IsListenerRelative());

      CPPUNIT_ASSERT_DOUBLES_EQUAL(cSndGain, customSound->GetGain(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cSndPitch, customSound->GetPitch(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cSndOffset, customSound->GetPlayTimeOffset(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cSndMaxDis, customSound->GetMaxDistance(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cSndRolloff, customSound->GetRolloffFactor(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cSndMinGain, customSound->GetMinGain(), 0.000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cSndMaxGain, customSound->GetMaxGain(), 0.000001);
      CPPUNIT_ASSERT(cSndListRel == customSound->IsListenerRelative());

      //verify listener relative does what's expected:
      sound->SetListenerRelative(true);
      customSound->SetListenerRelative(true);      
      sound->PlayImmediately();
      customSound->PlayImmediately();
      sound->StopImmediately();
      customSound->StopImmediately();
      CPPUNIT_ASSERT(sound->IsListenerRelative());
      CPPUNIT_ASSERT(customSound->IsListenerRelative());
      sound->SetListenerRelative(false);
      customSound->SetListenerRelative(false);      
      sound->PlayImmediately();
      customSound->PlayImmediately();
      sound->StopImmediately();
      customSound->StopImmediately();
      CPPUNIT_ASSERT(!sound->IsListenerRelative());
      CPPUNIT_ASSERT(!customSound->IsInitialized());      

      //Add 200 to all the values just to see if dtAudio can handle any bad values gracefully.
      sound->SetGain(sound->GetGain() + 200.0f);
      sound->SetPitch(sound->GetPitch() + 200.0f);
      sound->SetPlayTimeOffset(sound->GetPlayTimeOffset() + 200.0f);
      sound->SetMaxDistance(20000.0f);
      sound->SetRolloffFactor(2000.0f);
      sound->SetMinGain(sound->GetMinGain() + 200.0f);
      sound->SetMaxGain(sound->GetMaxGain() + 200.0f);
      
      sound->PlayImmediately();
      sound->StopImmediately();

      CPPUNIT_ASSERT(sound->GetGain() <= 1.0f);
      CPPUNIT_ASSERT(sound->GetPitch() <= 128.0f);
      CPPUNIT_ASSERT(sound->GetPlayTimeOffset() <= sound->GetDurationOfPlay());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(20000.0f, sound->GetMaxDistance(), 0.0000001);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(2000.0f, sound->GetRolloffFactor(), 0.000001);
      CPPUNIT_ASSERT(sound->GetMinGain() <= 1.0f);
      CPPUNIT_ASSERT(sound->GetMaxGain() <= 1.0f);

      AudioManager::GetInstance().FreeSound(sound.release());
   }
}

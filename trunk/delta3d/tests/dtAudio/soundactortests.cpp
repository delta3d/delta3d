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
* Chris Rodgers
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtABC/application.h>

#include <dtActors/engineactorregistry.h>

#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>
#include <dtAudio/soundactor.h>

#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtCore/project.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/floatactorproperty.h>

#include <dtGame/gamemanager.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>

#include <osg/io_utils>

extern dtABC::Application& GetGlobalApplication();


///////////////////////////////////////////////////////////////////////////////
// TEST OBJECT
///////////////////////////////////////////////////////////////////////////////
class SoundActorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(SoundActorTests);
      CPPUNIT_TEST(TestProperties);
      CPPUNIT_TEST(TestTimedPlay);
   CPPUNIT_TEST_SUITE_END();


   public:
      void setUp();
      void tearDown();

      // Helper Methods
      void TickSystem(unsigned int milliseconds);

      // Test Methods
      void TestProperties();
      void TestTimedPlay();

   private:
      static const std::string LIBRARY_TEST_GAME_ACTOR;
      static const std::string LIBRARY_DT_AUDIO;

      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      dtCore::RefPtr<dtAudio::SoundActor> mSndActor;
};

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(SoundActorTests);

///////////////////////////////////////////////////////////////////////////////
const std::string SoundActorTests::LIBRARY_TEST_GAME_ACTOR("testGameActorLibrary");
const std::string SoundActorTests::LIBRARY_DT_AUDIO("dtAudio");

///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::setUp()
{
   try
   {
      dtCore::Project::GetInstance().SetContext("data");
      dtCore::Scene* scene = new dtCore::Scene();
      mGameManager = new dtGame::GameManager(*scene);
      mGameManager->SetApplication(GetGlobalApplication());
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
      mGameManager->LoadActorRegistry(LIBRARY_TEST_GAME_ACTOR);
      mGameManager->LoadActorRegistry(LIBRARY_DT_AUDIO);

      dtAudio::AudioManager::Instantiate();

      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();

      dtCore::RefPtr<const dtCore::ActorType> actorType =
         mGameManager->FindActorType("dtcore.Environment","Sound Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",actorType.valid());

      mGameManager->CreateActor(*actorType, mSndActor);
      CPPUNIT_ASSERT_MESSAGE("Could not create sound actor proxy.",mSndActor.valid());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::tearDown()
{
   mSndActor = NULL;

   dtCore::System::GetInstance().SetPause(false);
   dtCore::System::GetInstance().Stop();

   if (mGameManager.valid())
   {
      mGameManager->DeleteAllActors();
      mGameManager->UnloadActorRegistry(LIBRARY_TEST_GAME_ACTOR);
      mGameManager = NULL;
   }

   dtAudio::AudioManager::Destroy();
}

///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::TickSystem(unsigned int milliseconds)
{
   dtCore::AppSleep(milliseconds);
   dtCore::System::GetInstance().Step();
}

///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::TestProperties()
{
   try
   {
      dtAudio::SoundActor* sndActor = mSndActor.get();

      // Get the tested properties.
      // Random Sound Effect
      // Offset Time
      const dtCore::BooleanActorProperty* propRandom = static_cast<const dtCore::BooleanActorProperty*>
         (sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_PLAY_AS_RANDOM ));
      const dtCore::FloatActorProperty* propOffsetTime = static_cast<const dtCore::FloatActorProperty*>
         (sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_INITIAL_OFFSET_TIME ));
      const dtCore::FloatActorProperty* propRandTimeMax = static_cast<const dtCore::FloatActorProperty*>
         (sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_MAX_RANDOM_TIME ));
      const dtCore::FloatActorProperty* propRandTimeMin = static_cast<const dtCore::FloatActorProperty*>
         (sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_MIN_RANDOM_TIME ));

      // Make sure the correct properties exist on the proxy.
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a random flag property.",
         propRandom != NULL);
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a initial-play-time-offset property.",
         propOffsetTime != NULL);
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a random-maximum-time property.",
         propRandTimeMax != NULL);
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a random-minimum-time property.",
         propRandTimeMin != NULL);
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a direction property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_DIRECTION ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a gain property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_GAIN ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a listener-relative flag property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_LISTENER_RELATIVE ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a looping flag property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_LOOPING ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a max distance property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_MAX_DISTANCE ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a max gain property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_MAX_GAIN ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a pith property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_PITCH ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a rolloff factor property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_ROLLOFF_FACTOR ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a sound-file property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_SOUND_EFFECT ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a velocity property.",
         sndActor->GetProperty( dtAudio::SoundActor::PROPERTY_VELOCITY ) != NULL );

      // Check the default values.
      CPPUNIT_ASSERT_MESSAGE("Sound should not be random by default",
         ! propRandom->GetValue() );
      CPPUNIT_ASSERT_MESSAGE("Sound should a time offset of 0 by default",
         propOffsetTime->GetValue() == 0.0f );
      CPPUNIT_ASSERT_MESSAGE("Sound max random time is 30 by default",
         propRandTimeMax->GetValue() == dtAudio::SoundActor::DEFAULT_RANDOM_TIME_MAX );
      CPPUNIT_ASSERT_MESSAGE("Sound min random time is 5 by default",
         propRandTimeMin->GetValue() == dtAudio::SoundActor::DEFAULT_RANDOM_TIME_MIN );

      float testFloat = 0.73f;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetGain(), sndActor->GetGain(), 0.01f);
      sndActor->SetGain(testFloat);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetSound()->GetGain(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetGain(), 0.01f);

      //Change the float to make sure all the properties differ.
      testFloat += 0.1f;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetMaxGain(), sndActor->GetMaxGain(), 0.01f);
      sndActor->SetMaxGain(testFloat);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetSound()->GetMaxGain(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetMaxGain(), 0.01f);

      testFloat = 0.9f;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetPitch(), sndActor->GetPitch(), 0.01f);
      sndActor->SetPitch(testFloat);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetSound()->GetPitch(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetPitch(), 0.01f);

      testFloat = 1.7f;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetMaxDistance(), sndActor->GetMaxDistance(), 0.01f);
      sndActor->SetMaxDistance(testFloat);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetSound()->GetMaxDistance(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetMaxDistance(), 0.01f);

      testFloat = 0.88f;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetRolloffFactor(), sndActor->GetRolloffFactor(), 0.01f);
      sndActor->SetRolloffFactor(testFloat);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetSound()->GetRolloffFactor(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, sndActor->GetRolloffFactor(), 0.01f);

      CPPUNIT_ASSERT_EQUAL(sndActor->GetSound()->IsLooping(), sndActor->IsLooping());
      sndActor->SetLooping(false);
      CPPUNIT_ASSERT_EQUAL(false, sndActor->GetSound()->IsLooping());
      CPPUNIT_ASSERT_EQUAL(false, sndActor->IsLooping());

      CPPUNIT_ASSERT_EQUAL(sndActor->GetSound()->IsListenerRelative(), sndActor->IsListenerRelative());
      sndActor->SetListenerRelative(true);
      CPPUNIT_ASSERT_EQUAL(true, sndActor->GetSound()->IsListenerRelative());
      CPPUNIT_ASSERT_EQUAL(true, sndActor->IsListenerRelative());

      CPPUNIT_ASSERT_THROW_MESSAGE("Play should throw an exception since no resource was set.", sndActor->Play(), dtUtil::Exception);

      dtCore::ResourceDescriptor rd("Sounds:silence.wav");
      sndActor->SetSoundResource(rd);

      sndActor->SetDirection(osg::Vec3(0.1, 0.3, 0.9));
      sndActor->SetVelocity(osg::Vec3(1.2, 3.3, -1.6));

      sndActor->Play();

      osg::Vec3 vec;
      sndActor->GetSound()->GetDirection(vec);
      CPPUNIT_ASSERT_EQUAL(vec, sndActor->GetDirection());
      sndActor->GetSound()->GetVelocity(vec);
      CPPUNIT_ASSERT_EQUAL(vec, sndActor->GetVelocity());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetGain(), sndActor->GetGain(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetMaxGain(), sndActor->GetMaxGain(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetPitch(), sndActor->GetPitch(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetMaxDistance(), sndActor->GetMaxDistance(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(sndActor->GetSound()->GetRolloffFactor(), sndActor->GetRolloffFactor(), 0.01f);
      CPPUNIT_ASSERT_EQUAL(sndActor->GetSound()->IsLooping(), sndActor->IsLooping());
      CPPUNIT_ASSERT_EQUAL(sndActor->GetSound()->IsListenerRelative(), sndActor->IsListenerRelative());

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::TestTimedPlay()
{
   try
   {
      // Test loading a sound.
      dtCore::ResourceDescriptor rd("Sounds:silence.wav");
      mSndActor->SetSoundResource(rd);
      dtAudio::Sound* sound = mSndActor->GetSound();

      // --- Ensure the proxy returns the same sound object.
      CPPUNIT_ASSERT_MESSAGE("Sound proxy should return the same sound object contained in the sound actor.",
         mSndActor->GetSound() == sound );

      // --- Ensure he sound does not play when simply loaded.
      dtCore::System::GetInstance().Step(); // Sends sound commands to Audio Manager.
      CPPUNIT_ASSERT_MESSAGE( "Sound should not be playing yet",
         ! sound->IsPlaying() );

      // Test adding to the game manager.
      const float offsetTime = 0.1f;

      //Clear all the randomness.
      mSndActor->SetMinRandomTime(0.0);
      mSndActor->SetMaxRandomTime(0.0);
      mSndActor->SetOffsetTime(offsetTime);
      sound->SetLooping(true);
      mGameManager->AddActor(*mSndActor, false, false);

      double simTime = dtCore::System::GetInstance().GetSimulationTime();
      dtCore::System::GetInstance().Step();
      double newSimTime = dtCore::System::GetInstance().GetSimulationTime();
      if ((newSimTime - simTime) < offsetTime)
      {
         CPPUNIT_ASSERT_MESSAGE("Sound should not be playing yet.",
                                 !sound->IsPlaying());
      }
      else
      {
         CPPUNIT_ASSERT_MESSAGE("Sound should be playing now.",
                                 sound->IsPlaying());
      }
 
      // --- Stop the sound
      sound->Stop();
      dtCore::System::GetInstance().Step(); // Sends sound commands to Audio Manager.
      CPPUNIT_ASSERT_MESSAGE( "Sound should now be Stopped",
         ! sound->IsPlaying() );

      // --- Start the sound to test stopping on removal from the system.
      sound->Play();
      dtCore::System::GetInstance().Step(); // Sends sound commands to Audio Manager.
      CPPUNIT_ASSERT_MESSAGE( "Sound should be playing again.",
         sound->IsPlaying() );

      // Test removing from the game manager.
      mGameManager->DeleteActor( *mSndActor );
      dtCore::System::GetInstance().Step(); // Removes actor from world and sends commands to Audio Manager.
      // such as stop.
      CPPUNIT_ASSERT_MESSAGE( "Sound should have been stopped when the actor was removed from the world",
         ! sound->IsPlaying() );

      // Test adding with a time offset of 0.
      mSndActor->SetOffsetTime( 0.0f );
      mGameManager->AddActor( *mSndActor, false, false );
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE( "Sound be playing.",
         sound->IsPlaying() );

      // --- Remove again
      mGameManager->DeleteActor( *mSndActor );
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE( "Sound should have been stopped when the actor was removed from the world again.",
         ! sound->IsPlaying() );

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


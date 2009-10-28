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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/soundactorproxy.h>
#include <dtActors/engineactorregistry.h>

#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>

#include <dtGame/gamemanager.h>

#include <dtDAL/enginepropertytypes.h>

#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <dtABC/application.h>

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

      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      dtCore::RefPtr<dtAudio::SoundActorProxy> mProxy;
};

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(SoundActorTests);

///////////////////////////////////////////////////////////////////////////////
const std::string SoundActorTests::LIBRARY_TEST_GAME_ACTOR("testGameActorLibrary");

///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::setUp()
{
   try
   {
      dtCore::Scene* scene = new dtCore::Scene();
      mGameManager = new dtGame::GameManager(*scene);
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      mGameManager->LoadActorRegistry(LIBRARY_TEST_GAME_ACTOR);

      dtAudio::AudioManager::Instantiate();

      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();

      dtCore::RefPtr<const dtDAL::ActorType> actorType =
         mGameManager->FindActorType("dtcore.Environment","Sound Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",actorType.valid());

      mGameManager->CreateActor(*actorType, mProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create sound actor proxy.",mProxy.valid());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::tearDown()
{
   mProxy = NULL;

   dtCore::System::GetInstance().SetPause(false);
   dtCore::System::GetInstance().Stop();

   dtAudio::AudioManager::Destroy();

   if(mGameManager.valid())
   {
      mGameManager->DeleteAllActors();
      mGameManager->UnloadActorRegistry(LIBRARY_TEST_GAME_ACTOR);
      mGameManager = NULL;
   }
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
      const dtAudio::SoundActorProxy* proxy = mProxy.get();

      // Get the tested properties.
      // Random Sound Effect
      // Offset Time
      const dtDAL::BooleanActorProperty* propRandom = static_cast<const dtDAL::BooleanActorProperty*>
         (proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_PLAY_AS_RANDOM ));
      const dtDAL::FloatActorProperty* propOffsetTime = static_cast<const dtDAL::FloatActorProperty*>
         (proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_INITIAL_OFFSET_TIME ));
      const dtDAL::FloatActorProperty* propRandTimeMax = static_cast<const dtDAL::FloatActorProperty*>
         (proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_MAX_RANDOM_TIME ));
      const dtDAL::FloatActorProperty* propRandTimeMin = static_cast<const dtDAL::FloatActorProperty*>
         (proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_MIN_RANDOM_TIME ));

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
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_DIRECTION ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a gain property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_GAIN ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a listener-relative flag property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_LISTENER_RELATIVE ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a looping flag property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_LOOPING ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a max distance property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_MAX_DISTANCE ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a max gain property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_MAX_GAIN ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a min gain property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_MIN_GAIN ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a pith property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_PITCH ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a rolloff factor property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_ROLLOFF_FACTOR ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a sound-file property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_SOUND_EFFECT ) != NULL );
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a velocity property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_VELOCITY ) != NULL );

      // Check the default values.
      CPPUNIT_ASSERT_MESSAGE("Sound should not be random by default",
         ! propRandom->GetValue() );
      CPPUNIT_ASSERT_MESSAGE("Sound should a time offset of 0 by default",
         propOffsetTime->GetValue() == 0.0f );
      CPPUNIT_ASSERT_MESSAGE("Sound max random time is 30 by default",
         propRandTimeMax->GetValue() == dtAudio::SoundActorProxy::DEFAULT_RANDOM_TIME_MAX );
      CPPUNIT_ASSERT_MESSAGE("Sound min random time is 5 by default",
         propRandTimeMin->GetValue() == dtAudio::SoundActorProxy::DEFAULT_RANDOM_TIME_MIN );
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
      mProxy->LoadFile( dtCore::GetDeltaRootPath() + "/tests/data/Sounds/silence.wav" );
      dtAudio::SoundActor* soundActor = NULL;
      mProxy->GetActor( soundActor );
      dtAudio::Sound* sound = soundActor->GetSound();

      // --- Ensure the proxy returns the same sound object.
      CPPUNIT_ASSERT_MESSAGE("Sound proxy should return the same sound object contained in the sound actor.",
         mProxy->GetSound() == sound );

      // --- Ensure he sound does not play when simply loaded.
      dtCore::System::GetInstance().Step(); // Sends sound commands to Audio Manager.
      CPPUNIT_ASSERT_MESSAGE( "Sound should not be playing yet",
         ! sound->IsPlaying() );

      // Test adding to the game manager.
      const float offsetTime = 0.1f;
      mProxy->SetOffsetTime(offsetTime);
      sound->SetLooping( true );
      mGameManager->AddActor( *mProxy, false, false );

      dtCore::Timer timer;
      dtCore::System::GetInstance().Step();
      if (timer.ElapsedSeconds() < offsetTime)
      {
         CPPUNIT_ASSERT_MESSAGE("Sound should not be playing yet.",
                                 sound->IsPlaying() == false );
      }
 
      /*TickSystem(20);
      if (timer.ElapsedSeconds() < offsetTime)
      {
         CPPUNIT_ASSERT_MESSAGE("Sound should still not be playing yet.",
                                sound->IsPlaying() == false);
      }

      TickSystem(81);
      if (timer.ElapsedSeconds() > offsetTime)
      {
         CPPUNIT_ASSERT_MESSAGE("Sound should now be playing.",
                                sound->IsPlaying() == true);
      }*/

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
      mGameManager->DeleteActor( *mProxy );
      dtCore::System::GetInstance().Step(); // Removes actor from world and sends commands to Audio Manager.
      // such as stop.
      CPPUNIT_ASSERT_MESSAGE( "Sound should have been stopped when the actor was removed from the world",
         ! sound->IsPlaying() );

      // Test adding with a time offset of 0.
      mProxy->SetOffsetTime( 0.0f );
      mGameManager->AddActor( *mProxy, false, false );
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE( "Sound be playing.",
         sound->IsPlaying() );

      // --- Remove again
      mGameManager->DeleteActor( *mProxy );
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE( "Sound should have been stopped when the actor was removed from the world again.",
         ! sound->IsPlaying() );

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


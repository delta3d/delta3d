/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Chris Rodgers
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/soundactorproxy.h>
#include <dtActors/engineactorregistry.h>

#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>

#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>

#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <dtDAL/namedparameter.h>

#include <dtUtil/stringutils.h>
#include <dtABC/application.h>

#include <vector>

extern dtABC::Application& GetGlobalApplication();

#ifdef DELTA_WIN32
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

///////////////////////////////////////////////////////////////////////////////
// TEST OBJECT
///////////////////////////////////////////////////////////////////////////////
class SoundActorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(SoundActorTests);
      CPPUNIT_TEST(TestProperties);
   CPPUNIT_TEST_SUITE_END();


   public:
      void setUp();
      void tearDown();

      void TestProperties();

   private:
      static const std::string LIBRARY_TEST_GAME_ACTOR;

      dtCore::RefPtr<dtGame::GameManager> mGameManager;
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
      dtAudio::AudioManager::GetInstance().Config(/*AudioConfigData(32)*/);

      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void SoundActorTests::tearDown()
{
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
void SoundActorTests::TestProperties()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actorType =
         mGameManager->FindActorType("dtcore.Environment","Sound Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",actorType.valid());

      dtCore::RefPtr<dtAudio::SoundActorProxy> proxy;
      mGameManager->CreateActor(*actorType, proxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create sound actor proxy.",proxy.valid());

      // Get the tested properties.
      // Random Sound Effect
      // Offset Time
      dtDAL::BooleanActorProperty* propRandom = static_cast<dtDAL::BooleanActorProperty*>
         (proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_PLAY_AS_RANDOM ));
      dtDAL::IntActorProperty* propOffsetTime = static_cast<dtDAL::IntActorProperty*>
         (proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_INITIAL_OFFSET_TIME ));
      dtDAL::IntActorProperty* propRandTimeMax = static_cast<dtDAL::IntActorProperty*>
         (proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_MAX_RANDOM_TIME ));
      dtDAL::IntActorProperty* propRandTimeMin = static_cast<dtDAL::IntActorProperty*>
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
      CPPUNIT_ASSERT_MESSAGE("Sound actor should have a min distance property.",
         proxy->GetProperty( dtAudio::SoundActorProxy::PROPERTY_MIN_DISTANCE ) != NULL );
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
         propOffsetTime->GetValue() == 0 );
      CPPUNIT_ASSERT_MESSAGE("Sound max random time is 30 by default",
         propRandTimeMax->GetValue() == 30 );
      CPPUNIT_ASSERT_MESSAGE("Sound min random time is 5 by default",
         propRandTimeMin->GetValue() == 5 );

      // Test loading a sound.
      proxy->LoadFile( "Sounds/silence.wav" );
      dtAudio::Sound* sound = static_cast<dtAudio::SoundActor&>(proxy->GetGameActor()).GetSound();

      dtCore::System::GetInstance().Step(); // Sends sound commands to Audio Manager.

      CPPUNIT_ASSERT_MESSAGE( "Sound should not be playing yet",
         ! sound->IsPlaying() );

      // Test adding to the game manager.
      mGameManager->AddActor( *proxy, false, false );
      dtCore::System::GetInstance().Step(); // Sends create message.
      CPPUNIT_ASSERT_MESSAGE( "Sound should not be playing yet",
         ! sound->IsPlaying() );
      // --- Start the sound
      sound->SetLooping( true );
      sound->Play();
      dtCore::System::GetInstance().Step(); // Sends sound commands to Audio Manager.
      CPPUNIT_ASSERT_MESSAGE( "Sound should now be playing",
         sound->IsPlaying() );

      // Test removing from the game manager.
      mGameManager->DeleteActor( *proxy );
      dtCore::System::GetInstance().Step(); // Removes actor from world and sends commands to Audio Manager,
                                            // such as stop.
      dtCore::System::GetInstance().Step(); // Removes actor from world and sends commands to Audio Manager,
      // such as stop.
      CPPUNIT_ASSERT_MESSAGE( "Sound should have been stopped when the actor was removed from the world",
         ! sound->IsPlaying() );
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

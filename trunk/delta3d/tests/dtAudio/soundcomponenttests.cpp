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
 * Erik Johnson
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/soundactorproxy.h>
#include <dtActors/engineactorregistry.h>

#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>
#include <dtAudio/soundcomponent.h>
#include <dtAudio/soundinfo.h>
#include <dtGame/gamemanager.h>

#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <vector>



////////////////////////////////////////////////////////////////////////////////
// TEST OBJECT
////////////////////////////////////////////////////////////////////////////////
class SoundComponentTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(SoundComponentTests);
      CPPUNIT_TEST(TestSoundManagement);
      CPPUNIT_TEST(TestSoundActorManagement);
      CPPUNIT_TEST(TestSoundCommands);
   CPPUNIT_TEST_SUITE_END();


   public:
      void setUp();
      void tearDown();

      // Utility Methods
      void CreateSoundActors( int proxyTotal, bool addToGM );
      int GetGMSoundActorCount() const;

      // Test Methods
      void TestSoundManagement();
      void TestSoundActorManagement();
      void TestSoundCommands();

   private:
      static const std::string LIBRARY_TEST_GAME_ACTOR;

      dtCore::RefPtr<dtGame::GameManager> mGM;
      dtCore::RefPtr<dtAudio::SoundComponent> mSndComp;
      dtAudio::SoundComponent::SoundProxyRefArray mSndProxyArray;
      dtCore::RefPtr<const dtDAL::ActorType> mSndActorType;
};

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(SoundComponentTests);

////////////////////////////////////////////////////////////////////////////////
const std::string SoundComponentTests::LIBRARY_TEST_GAME_ACTOR("testGameActorLibrary");

////////////////////////////////////////////////////////////////////////////////
void SoundComponentTests::setUp()
{
   try
   {
      dtCore::Scene* scene = new dtCore::Scene();
      mGM = new dtGame::GameManager(*scene);
      mGM->LoadActorRegistry(LIBRARY_TEST_GAME_ACTOR);

      mSndActorType = mGM->FindActorType("dtcore.Environment","Sound Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",mSndActorType.valid());

      dtAudio::AudioManager::Instantiate();

      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();

      mSndComp = new dtAudio::SoundComponent("TestSoundComponent");
      mGM->AddComponent( *mSndComp, dtGame::GameManager::ComponentPriority::NORMAL );
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void SoundComponentTests::tearDown()
{
   try
   {
      dtCore::System::GetInstance().SetPause(false);
      dtCore::System::GetInstance().Stop();

      mSndProxyArray.clear();
      mSndComp = NULL;

      mGM->DeleteAllActors();
      mGM->UnloadActorRegistry(LIBRARY_TEST_GAME_ACTOR);
      mGM = NULL;

      dtAudio::AudioManager::Destroy();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void SoundComponentTests::CreateSoundActors( int proxyTotal, bool addToGM )
{
   mSndProxyArray.reserve( proxyTotal );

   dtCore::RefPtr<dtAudio::SoundActorProxy> curProxy;
   for( int i = 0; i < proxyTotal; ++i )
   {
      mGM->CreateActor( *mSndActorType, curProxy );
      mSndProxyArray.push_back( curProxy.get() );

      if( addToGM )
      {
         mGM->AddActor( *curProxy, false, false );
      }

      curProxy = NULL;
   }
}

////////////////////////////////////////////////////////////////////////////////
int SoundComponentTests::GetGMSoundActorCount() const
{
   typedef std::vector<dtDAL::ActorProxy*> ProxyArray;
   ProxyArray proxies;
   mGM->FindActorsByType( *mSndActorType, proxies );
   return int(proxies.size());
}

////////////////////////////////////////////////////////////////////////////////
void SoundComponentTests::TestSoundManagement()
{
   try
   {
      using namespace dtAudio;
      const std::string unitTestDataFilePath = dtCore::GetDeltaRootPath() + "/tests/data/";

      const std::string testSoundFile = unitTestDataFilePath + "Sounds/silence.wav";
      const std::string testSoundFile2 = unitTestDataFilePath + "Sounds/silence2.wav";
      const std::string fakeSoundFile("FAKE.wav");
      const std::string name1(testSoundFile);
      const std::string name2(testSoundFile2);
      const std::string name3("Sound_3");
      const std::string name4("Sound_4");
      const std::string name5("Sound_5");
      const std::string name6("Sound_6");
      Sound* sound1 = mSndComp->AddSound( testSoundFile ); // DEFAULT
      Sound* sound2 = mSndComp->AddSound( testSoundFile2, SoundType::SOUND_TYPE_WORLD_EFFECT );
      Sound* sound3 = mSndComp->AddSound( testSoundFile, name3 ); // DEFAULT
      Sound* sound4 = mSndComp->AddSound( testSoundFile, name4, SoundType::SOUND_TYPE_VOICE );
      Sound* sound5 = AudioManager::GetInstance().NewSound();
      Sound* sound6 = AudioManager::GetInstance().NewSound();
      sound5->LoadFile(testSoundFile.c_str());
      sound6->LoadFile(testSoundFile.c_str());

      // Test addition success.
      CPPUNIT_ASSERT( sound1 != NULL );
      CPPUNIT_ASSERT( sound2 != NULL );
      CPPUNIT_ASSERT( sound3 != NULL );
      CPPUNIT_ASSERT( sound4 != NULL );
      CPPUNIT_ASSERT( mSndComp->AddSound( *sound5, name5 ) ); // DEFAULT
      CPPUNIT_ASSERT( mSndComp->AddSound( *sound6, name6, SoundType::SOUND_TYPE_MUSIC ) );

      // Setup the sounds
      sound1->SetLooping( true );
      sound2->SetLooping( true );
      sound3->SetLooping( true );
      sound4->SetLooping( true );
      sound5->SetLooping( true );
      sound6->SetLooping( true );

      // Ensure the sounds are different.
      CPPUNIT_ASSERT( sound1 != sound2 );
      CPPUNIT_ASSERT( sound1 != sound3 );
      CPPUNIT_ASSERT( sound1 != sound4 );
      CPPUNIT_ASSERT( sound1 != sound5 );
      CPPUNIT_ASSERT( sound1 != sound6 );
      CPPUNIT_ASSERT( sound2 != sound3 );
      CPPUNIT_ASSERT( sound2 != sound4 );
      CPPUNIT_ASSERT( sound2 != sound5 );
      CPPUNIT_ASSERT( sound2 != sound6 );
      CPPUNIT_ASSERT( sound3 != sound4 );
      CPPUNIT_ASSERT( sound3 != sound5 );
      CPPUNIT_ASSERT( sound3 != sound6 );
      CPPUNIT_ASSERT( sound4 != sound5 );
      CPPUNIT_ASSERT( sound4 != sound6 );
      CPPUNIT_ASSERT( sound5 != sound6 );

      // Test adding the files again under the same names and conditions.
      CPPUNIT_ASSERT( mSndComp->AddSound( testSoundFile ) == NULL );
      CPPUNIT_ASSERT( mSndComp->AddSound( testSoundFile2, SoundType::SOUND_TYPE_WORLD_EFFECT ) == NULL );
      CPPUNIT_ASSERT( mSndComp->AddSound( testSoundFile, name3 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->AddSound( testSoundFile, name4, SoundType::SOUND_TYPE_VOICE ) == NULL );
      CPPUNIT_ASSERT( ! mSndComp->AddSound( *sound5, name5 ) );
      CPPUNIT_ASSERT( ! mSndComp->AddSound( *sound6, name6, SoundType::SOUND_TYPE_MUSIC ) );
      CPPUNIT_ASSERT( mSndComp->AddSound( fakeSoundFile ) == NULL );

      // Ensure the sounds can be accessed again.
      CPPUNIT_ASSERT( mSndComp->GetSound( name1 ) == sound1 );
      CPPUNIT_ASSERT( mSndComp->GetSound( name2 ) == sound2 );
      CPPUNIT_ASSERT( mSndComp->GetSound( name3 ) == sound3 );
      CPPUNIT_ASSERT( mSndComp->GetSound( name4 ) == sound4 );
      CPPUNIT_ASSERT( mSndComp->GetSound( name5 ) == sound5 );
      CPPUNIT_ASSERT( mSndComp->GetSound( name6 ) == sound6 );
      CPPUNIT_ASSERT( mSndComp->GetSound( fakeSoundFile ) == NULL );

      // Test access to Sound Info objects containing the Sounds
      const SoundInfo* info1 = mSndComp->GetSoundInfo( name1 );
      const SoundInfo* info2 = mSndComp->GetSoundInfo( name2 );
      const SoundInfo* info3 = mSndComp->GetSoundInfo( name3 );
      const SoundInfo* info4 = mSndComp->GetSoundInfo( name4 );
      const SoundInfo* info5 = mSndComp->GetSoundInfo( name5 );
      const SoundInfo* info6 = mSndComp->GetSoundInfo( name6 );
      CPPUNIT_ASSERT( info1 != NULL );
      CPPUNIT_ASSERT( info2 != NULL );
      CPPUNIT_ASSERT( info3 != NULL );
      CPPUNIT_ASSERT( info4 != NULL );
      CPPUNIT_ASSERT( info5 != NULL );
      CPPUNIT_ASSERT( info6 != NULL );
      CPPUNIT_ASSERT( mSndComp->GetSoundInfo(fakeSoundFile) == NULL );

      // Test the info objects.
      CPPUNIT_ASSERT( &info1->GetSound() == sound1 );
      CPPUNIT_ASSERT( &info2->GetSound() == sound2 );
      CPPUNIT_ASSERT( &info3->GetSound() == sound3 );
      CPPUNIT_ASSERT( &info4->GetSound() == sound4 );
      CPPUNIT_ASSERT( &info5->GetSound() == sound5 );
      CPPUNIT_ASSERT( &info6->GetSound() == sound6 );
      CPPUNIT_ASSERT( info1->GetType() == SoundType::SOUND_TYPE_DEFAULT );
      CPPUNIT_ASSERT( info2->GetType() == SoundType::SOUND_TYPE_WORLD_EFFECT );
      CPPUNIT_ASSERT( info3->GetType() == SoundType::SOUND_TYPE_DEFAULT );
      CPPUNIT_ASSERT( info4->GetType() == SoundType::SOUND_TYPE_VOICE );
      CPPUNIT_ASSERT( info5->GetType() == SoundType::SOUND_TYPE_DEFAULT );
      CPPUNIT_ASSERT( info6->GetType() == SoundType::SOUND_TYPE_MUSIC );

      // Test accessing sounds by type.
      SoundComponent::SoundArray soundArray;
      mSndComp->GetSoundsByType( SoundType::SOUND_TYPE_DEFAULT, soundArray );
      CPPUNIT_ASSERT( soundArray.size() == 3 );
      soundArray.clear();

      mSndComp->GetSoundsByType( SoundType::SOUND_TYPE_WORLD_EFFECT, soundArray );
      CPPUNIT_ASSERT( soundArray.size() == 1 );
      soundArray.clear();

      mSndComp->GetSoundsByType( SoundType::SOUND_TYPE_VOICE, soundArray );
      CPPUNIT_ASSERT( soundArray.size() == 1 );
      soundArray.clear();

      mSndComp->GetSoundsByType( SoundType::SOUND_TYPE_MUSIC, soundArray );
      CPPUNIT_ASSERT( soundArray.size() == 1 );
      soundArray.clear();

      // Test removing single sound by name.
      CPPUNIT_ASSERT( mSndComp->RemoveSound( name2 ) );
      CPPUNIT_ASSERT( ! mSndComp->RemoveSound( name2 ) );
      CPPUNIT_ASSERT( mSndComp->RemoveSound( name4 ) );
      CPPUNIT_ASSERT( ! mSndComp->RemoveSound( "Fake" ) );
      CPPUNIT_ASSERT( mSndComp->GetSound( name1 ) != NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name2 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name3 ) != NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name4 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name5 ) != NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name6 ) != NULL );

      // Clear all remaining sounds.
      mSndComp->RemoveAllSounds();
      CPPUNIT_ASSERT( mSndComp->GetSound( name1 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name2 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name3 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name4 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name5 ) == NULL );
      CPPUNIT_ASSERT( mSndComp->GetSound( name6 ) == NULL );
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void SoundComponentTests::TestSoundActorManagement()
{
   try
   {
      dtAudio::SoundComponent::SoundArray soundArray;

      // Create Sound Actors.
      int soundActorCount = 5;
      CreateSoundActors( soundActorCount, true );
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT( GetGMSoundActorCount() == soundActorCount );
      CPPUNIT_ASSERT( mSndComp->GetSoundActorContainedCount() == 0 );

      mSndComp->GetSoundActorSounds( soundArray );
      CPPUNIT_ASSERT( int(soundArray.size()) == soundActorCount );
      soundArray.clear();


      // Sound Actor Evacuation.
      mSndComp->RemoveSoundActorsFromWorld();
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT( GetGMSoundActorCount() == 0 );
      CPPUNIT_ASSERT( mSndComp->GetSoundActorContainedCount() == soundActorCount );

      mSndComp->GetSoundActorSounds( soundArray );
      CPPUNIT_ASSERT( int(soundArray.size()) == 0 );
      soundArray.clear();


      // Sound Actor Insertion.
      mSndComp->AddSoundActorsToWorld();
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT( GetGMSoundActorCount() == soundActorCount );
      CPPUNIT_ASSERT( mSndComp->GetSoundActorContainedCount() == 0 );

      mSndComp->GetSoundActorSounds( soundArray );
      CPPUNIT_ASSERT( int(soundArray.size()) == soundActorCount );
      soundArray.clear();


      // Sound Actor Deletion.
      mSndComp->RemoveSoundActorsFromWorld();
      mSndComp->ClearSoundActorArray();
      dtCore::System::GetInstance().Step();
      mSndComp->GetSoundActorSounds( soundArray );
      CPPUNIT_ASSERT( GetGMSoundActorCount() == 0 );
      CPPUNIT_ASSERT( mSndComp->GetSoundActorContainedCount() == 0 );
      CPPUNIT_ASSERT( soundArray.size() == 0 );
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void SoundComponentTests::TestSoundCommands()
{
   using namespace dtAudio;
   using namespace dtCore;

   const std::string testSoundFile = dtCore::GetDeltaRootPath() + "/tests/data/Sounds/silence.wav";
   const std::string name1("Sound_1");
   const std::string name2("Sound_2");
   const std::string name3("Sound_3");
   const std::string name4("Sound_4");
   const std::string name5("Sound_5");
   Sound* sound1 = mSndComp->AddSound( testSoundFile, name1, SoundType::SOUND_TYPE_WORLD_EFFECT );
   Sound* sound2 = mSndComp->AddSound( testSoundFile, name2, SoundType::SOUND_TYPE_MUSIC );
   Sound* sound3 = mSndComp->AddSound( testSoundFile, name3, SoundType::SOUND_TYPE_WORLD_EFFECT );
   Sound* sound4 = mSndComp->AddSound( testSoundFile, name4, SoundType::SOUND_TYPE_MUSIC );
   Sound* sound5 = mSndComp->AddSound( testSoundFile, name5, SoundType::SOUND_TYPE_WORLD_EFFECT );

   // Test addition success.
   CPPUNIT_ASSERT( sound1 != NULL );
   CPPUNIT_ASSERT( sound2 != NULL );
   CPPUNIT_ASSERT( sound3 != NULL );
   CPPUNIT_ASSERT( sound4 != NULL );
   CPPUNIT_ASSERT( sound5 != NULL );

   // Setup the sound objects.
   sound1->SetLooping( true );
   sound2->SetLooping( true );
   sound3->SetLooping( true );
   sound4->SetLooping( true );
   sound5->SetLooping( true );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( ! sound1->IsPlaying() );
   CPPUNIT_ASSERT( ! sound2->IsPlaying() );
   CPPUNIT_ASSERT( ! sound3->IsPlaying() );
   CPPUNIT_ASSERT( ! sound4->IsPlaying() );
   CPPUNIT_ASSERT( ! sound5->IsPlaying() );

   // Test single commands.
   // --- By Object
   mSndComp->DoSoundCommand( SoundCommand::SOUND_COMMAND_PLAY, *sound4 );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( ! sound1->IsPlaying() );
   CPPUNIT_ASSERT( ! sound2->IsPlaying() );
   CPPUNIT_ASSERT( ! sound3->IsPlaying() );
   CPPUNIT_ASSERT( sound4->IsPlaying() );
   CPPUNIT_ASSERT( ! sound5->IsPlaying() );

   // --- By Name
   mSndComp->DoSoundCommand( SoundCommand::SOUND_COMMAND_PLAY, name3 );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( ! sound1->IsPlaying() );
   CPPUNIT_ASSERT( ! sound2->IsPlaying() );
   CPPUNIT_ASSERT( sound3->IsPlaying() );
   CPPUNIT_ASSERT( sound4->IsPlaying() );
   CPPUNIT_ASSERT( ! sound5->IsPlaying() );
   
   // Test playing by type.
   // --- Play all World Effects
   SoundComponent::SoundArray soundArray;
   mSndComp->GetSoundsByType( SoundType::SOUND_TYPE_WORLD_EFFECT, soundArray );
   CPPUNIT_ASSERT( soundArray.size() == 3 );
   mSndComp->DoSoundCommand( SoundCommand::SOUND_COMMAND_PLAY, soundArray );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( sound1->IsPlaying() );
   CPPUNIT_ASSERT( ! sound2->IsPlaying() );
   CPPUNIT_ASSERT( sound3->IsPlaying() ); // This should still be playing.
   CPPUNIT_ASSERT( sound4->IsPlaying() );
   CPPUNIT_ASSERT( sound5->IsPlaying() );

   // --- Pause all Music
   soundArray.clear();
   mSndComp->GetSoundsByType( SoundType::SOUND_TYPE_MUSIC, soundArray );
   CPPUNIT_ASSERT( soundArray.size() == 2 );
   mSndComp->DoSoundCommand( SoundCommand::SOUND_COMMAND_PAUSE, soundArray );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( sound1->IsPlaying() );
   CPPUNIT_ASSERT( ! sound2->IsPlaying() ); // This should still be NOT playing.
   CPPUNIT_ASSERT( sound3->IsPlaying() );
   CPPUNIT_ASSERT( ! sound4->IsPlaying() );
   CPPUNIT_ASSERT( sound5->IsPlaying() );

   CPPUNIT_ASSERT( ! sound2->IsPaused() ); // This should still be NOT playing.
   CPPUNIT_ASSERT( sound4->IsPaused() );

   // --- Play all Music and pause all World Effects.
   soundArray.clear();
   mSndComp->GetSoundsByType( SoundType::SOUND_TYPE_MUSIC, soundArray );
   CPPUNIT_ASSERT( soundArray.size() == 2 );
   mSndComp->DoSoundCommand( SoundCommand::SOUND_COMMAND_PLAY, soundArray );
   soundArray.clear();
   mSndComp->PauseAllSoundsByType( SoundType::SOUND_TYPE_WORLD_EFFECT );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( sound2->IsPlaying() );
   CPPUNIT_ASSERT( sound4->IsPlaying() );

   CPPUNIT_ASSERT( sound1->IsPaused() );
   CPPUNIT_ASSERT( sound3->IsPaused() );
   CPPUNIT_ASSERT( sound5->IsPaused() );

   CPPUNIT_ASSERT( ! sound1->IsStopped() );
   CPPUNIT_ASSERT( ! sound2->IsStopped() );
   CPPUNIT_ASSERT( ! sound3->IsStopped() );
   CPPUNIT_ASSERT( ! sound4->IsStopped() );
   CPPUNIT_ASSERT( ! sound5->IsStopped() );

   // Test convenience methods
   mSndComp->Pause( name2 );
   mSndComp->Play( name3 );
   mSndComp->Stop( name4 );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( ! sound1->IsStopped() );
   CPPUNIT_ASSERT( ! sound2->IsStopped() );
   CPPUNIT_ASSERT( ! sound3->IsStopped() );
   CPPUNIT_ASSERT( sound4->IsStopped() );
   CPPUNIT_ASSERT( ! sound5->IsStopped() );

   CPPUNIT_ASSERT( sound1->IsPaused() );
   CPPUNIT_ASSERT( sound2->IsPaused() );
   CPPUNIT_ASSERT( sound5->IsPaused() );

   CPPUNIT_ASSERT( sound3->IsPlaying() );


   // Test stopping sounds by type.
   mSndComp->StopAllSoundsByType( SoundType::SOUND_TYPE_WORLD_EFFECT );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( sound1->IsStopped() );
   CPPUNIT_ASSERT( ! sound2->IsStopped() ); // MUSIC
   CPPUNIT_ASSERT( sound3->IsStopped() );
   CPPUNIT_ASSERT( sound4->IsStopped() ); // Should still be stopped.
   CPPUNIT_ASSERT( sound5->IsStopped() );

   // Play all sounds
   soundArray.clear();
   mSndComp->GetAllSounds( soundArray );
   CPPUNIT_ASSERT( soundArray.size() == 5 );
   mSndComp->DoSoundCommand( SoundCommand::SOUND_COMMAND_PLAY, soundArray );
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( sound1->IsPlaying() );
   CPPUNIT_ASSERT( sound2->IsPlaying() );
   CPPUNIT_ASSERT( sound3->IsPlaying() );
   CPPUNIT_ASSERT( sound4->IsPlaying() );
   CPPUNIT_ASSERT( sound5->IsPlaying() );

   // Pause all sounds.
   mSndComp->PauseAllSounds();
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( sound1->IsPaused() );
   CPPUNIT_ASSERT( sound2->IsPaused() );
   CPPUNIT_ASSERT( sound3->IsPaused() );
   CPPUNIT_ASSERT( sound4->IsPaused() );
   CPPUNIT_ASSERT( sound5->IsPaused() );

   // Pause all sounds.
   mSndComp->StopAllSounds();
   System::GetInstance().Step(); // Let Audio Manager process command.
   CPPUNIT_ASSERT( sound1->IsStopped() );
   CPPUNIT_ASSERT( sound2->IsStopped() );
   CPPUNIT_ASSERT( sound3->IsStopped() );
   CPPUNIT_ASSERT( sound4->IsStopped() );
   CPPUNIT_ASSERT( sound5->IsStopped() );
}

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
 * @author William E. Johnson II
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/audiomanager.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>

class AudioManagerTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(AudioManagerTests);
      CPPUNIT_TEST(TestInitialize);
      CPPUNIT_TEST(TestInitializeCustomContext);
      CPPUNIT_TEST(TestInitializeCustomContextNoShutdown);
      CPPUNIT_TEST(TestPausing);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestInitialize();
   void TestInitializeCustomContext();
   void TestInitializeCustomContextNoShutdown();
   void TestPausing();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AudioManagerTests);

void AudioManagerTests::setUp()
{
   if (dtAudio::AudioManager::IsInitialized())
   {
      dtAudio::AudioManager::GetInstance().Destroy();
   }
}

void AudioManagerTests::tearDown()
{
   if (dtAudio::AudioManager::IsInitialized())
   {
      dtAudio::AudioManager::GetInstance().Destroy();
   }
}

void AudioManagerTests::TestInitialize()
{
   CPPUNIT_ASSERT(!dtAudio::AudioManager::IsInitialized());

   dtAudio::AudioManager::Instantiate();

   CPPUNIT_ASSERT(dtAudio::AudioManager::IsInitialized());

   dtAudio::AudioManager::Destroy();

   CPPUNIT_ASSERT(!dtAudio::AudioManager::IsInitialized());
}

void CreateDeviceAndContext(ALCdevice*& device, ALCcontext*& context)
{
   device = alcOpenDevice("");
   CPPUNIT_ASSERT(device);

   context = alcCreateContext(device, NULL);
   CPPUNIT_ASSERT(context);
}

void AudioManagerTests::TestInitializeCustomContext()
{
   ALCdevice* device = NULL;
   ALCcontext* context = NULL;
   CreateDeviceAndContext(device, context);

   dtAudio::AudioManager::Instantiate("joe", device, context, true);

   CPPUNIT_ASSERT(dtAudio::AudioManager::IsInitialized());

   CPPUNIT_ASSERT(dtAudio::AudioManager::GetInstance().GetContext() == context);
   CPPUNIT_ASSERT(dtAudio::AudioManager::GetInstance().GetDevice() == device);

   dtAudio::AudioManager::GetInstance().Destroy();

   // This test doesn't seem to be valid in all implementations.
   //CPPUNIT_ASSERT(!alcMakeContextCurrent(context));
   CPPUNIT_ASSERT(alcGetCurrentContext() == NULL);
}

void AudioManagerTests::TestInitializeCustomContextNoShutdown()
{
   ALCdevice* device = NULL;
   ALCcontext* context = NULL;
   CreateDeviceAndContext(device, context);

   dtAudio::AudioManager::Instantiate("joe", device, context, false);

   CPPUNIT_ASSERT(dtAudio::AudioManager::IsInitialized());

   CPPUNIT_ASSERT(dtAudio::AudioManager::GetInstance().GetContext() == context);
   CPPUNIT_ASSERT(dtAudio::AudioManager::GetInstance().GetDevice() == device);

   dtAudio::AudioManager::GetInstance().Destroy();

   CPPUNIT_ASSERT(alcMakeContextCurrent(context));
   CPPUNIT_ASSERT(alcMakeContextCurrent(NULL));
   alcDestroyContext(context);
   CPPUNIT_ASSERT(alcCloseDevice(device));
}

void AudioManagerTests::TestPausing()
{
   try
   {
      using namespace dtAudio;

      // Init the Audio Manager.
      ALCdevice* device = NULL;
      ALCcontext* context = NULL;
      CreateDeviceAndContext(device, context);
      AudioManager::Instantiate("joe", device, context, false);
      CPPUNIT_ASSERT(AudioManager::IsInitialized());

      // Get the Audio Manager.
      AudioManager& am = AudioManager::GetInstance();

      const std::string unitTestDataFilePath = dtUtil::GetDeltaRootPath() + "/tests/data/";
      const std::string testSoundFile = unitTestDataFilePath + "Sounds/silence.wav";

      Sound* sound1 = am.NewSound();
      Sound* sound2 = am.NewSound();
      Sound* sound3 = am.NewSound();
      sound1->LoadFile(testSoundFile.c_str());
      sound2->LoadFile(testSoundFile.c_str());
      sound3->LoadFile(testSoundFile.c_str());
      
      // Loop all sounds for test purposes.
      sound1->SetLooping(true);
      sound2->SetLooping(true);
      sound3->SetLooping(true);

      // Test a few sounds in playing and stopped states.
      sound1->Play();
      sound2->Play();
      CPPUNIT_ASSERT(sound1->IsPlaying());
      CPPUNIT_ASSERT(sound2->IsPlaying());
      CPPUNIT_ASSERT( ! sound3->IsPaused());
      CPPUNIT_ASSERT( ! sound3->IsPlaying());
      CPPUNIT_ASSERT(sound3->IsStopped());

      // Ensure pausing sounds only affects sounds that were playing
      // and not those that were stopped.
      am.PauseSounds();
      CPPUNIT_ASSERT(sound1->IsPaused());
      CPPUNIT_ASSERT(sound2->IsPaused());
      CPPUNIT_ASSERT( ! sound3->IsPaused());
      CPPUNIT_ASSERT( ! sound3->IsPlaying());
      CPPUNIT_ASSERT(sound3->IsStopped());

      // Unpause only sounds that are in a paused state
      // and not in a stopped state.
      am.UnPauseSounds();
      CPPUNIT_ASSERT(sound1->IsPlaying());
      CPPUNIT_ASSERT(sound2->IsPlaying());
      CPPUNIT_ASSERT( ! sound3->IsPaused());
      CPPUNIT_ASSERT( ! sound3->IsPlaying());
      CPPUNIT_ASSERT(sound3->IsStopped());

      // Prepare for the next test of calling Unpause.
      // All sounds should not be playing before that test.
      am.PauseSounds();
      CPPUNIT_ASSERT(sound1->IsPaused());
      CPPUNIT_ASSERT(sound2->IsPaused());
      CPPUNIT_ASSERT( ! sound3->IsPaused());
      CPPUNIT_ASSERT( ! sound3->IsPlaying());
      CPPUNIT_ASSERT(sound3->IsStopped());

      // Start up sound 3.
      sound3->Play();
      CPPUNIT_ASSERT(sound1->IsPaused());
      CPPUNIT_ASSERT(sound2->IsPaused());
      CPPUNIT_ASSERT(sound3->IsPlaying());

      // Unpause currently paused ones, without affecting
      // the sound that is currently playing.
      am.UnPauseSounds();
      CPPUNIT_ASSERT(sound1->IsPlaying());
      CPPUNIT_ASSERT(sound2->IsPlaying());
      CPPUNIT_ASSERT(sound3->IsPlaying());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

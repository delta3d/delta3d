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

#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAudio/audiomanager.h>

class AudioManagerTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(AudioManagerTests);
      CPPUNIT_TEST(TestInitialize);
      CPPUNIT_TEST(TestInitializeCustomContext);
      CPPUNIT_TEST(TestInitializeCustomContextNoShutdown);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestInitialize();
   void TestInitializeCustomContext();
   void TestInitializeCustomContextNoShutdown();
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

/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/shadermanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>

#include <cppunit/extensions/HelperMacros.h>

std::string GetTestsDir();

class GameApplicationLoaderTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GameApplicationLoaderTests);

      CPPUNIT_TEST(TestProperties);
      CPPUNIT_TEST(TestCleanup);

   CPPUNIT_TEST_SUITE_END();

public:
   std::string projectContext;

   void setUp()
   {
      projectContext = GetTestsDir() + dtUtil::FileUtils::PATH_SEPARATOR + "data" + dtUtil::FileUtils::PATH_SEPARATOR + "ProjectContext";
      mGameApplicationLoader = new dtGame::GameApplicationLoader(0, NULL);
   }
   
   void tearDown()
   {
      delete mGameApplicationLoader;
      mGameApplicationLoader = NULL;
      mLogger = NULL;
   }

   void TestProperties()
   {
      std::string name = "client";
      
      CPPUNIT_ASSERT(mGameApplicationLoader->GetGameLibraryName().empty());
      mGameApplicationLoader->SetGameLibraryName(name);
      CPPUNIT_ASSERT(mGameApplicationLoader->GetGameLibraryName() == name);
   }

   void TestCleanup()
   {
      dtUtil::SetDataFilePathList(projectContext);
      dtCore::ShaderManager::GetInstance().LoadShaderDefinitions("Shaders/TestShaderDefinitions.xml",false);

      CPPUNIT_ASSERT(dtCore::ShaderManager::GetInstance().GetNumShaderGroupPrototypes() > 0);
      dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager(*new dtCore::Scene);
      gm->AddComponent(*new dtGame::DefaultMessageProcessor(), dtGame::GameManager::ComponentPriority::HIGHEST);
      mGameApplicationLoader->SetGameManager(gm);

      delete mGameApplicationLoader;
      mGameApplicationLoader = NULL;

      CPPUNIT_ASSERT_MESSAGE("The shader manager should cleared on app shutdown", 
               dtCore::ShaderManager::GetInstance().GetNumShaderGroupPrototypes() == 0);

      CPPUNIT_ASSERT_MESSAGE("The GM should have been shutdown.",
               gm->GetComponentByName(dtGame::DefaultMessageProcessor::DEFAULT_NAME) == NULL);
   }

private:
   dtUtil::Log* mLogger;

   dtGame::GameApplicationLoader* mGameApplicationLoader;

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameApplicationLoaderTests);


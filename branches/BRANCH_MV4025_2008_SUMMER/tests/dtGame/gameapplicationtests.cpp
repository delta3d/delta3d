/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <dtUtil/log.h>
#include <dtUtil/macros.h>
#include <dtUtil/fileutils.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/shadermanager.h>
#include <dtCore/globals.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>

#include <cppunit/extensions/HelperMacros.h>

class GameApplicationTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GameApplicationTests);

      CPPUNIT_TEST(TestProperties);
      CPPUNIT_TEST(TestCleanup);

   CPPUNIT_TEST_SUITE_END();

public:
   static const std::string TESTS_DIR;
   static const std::string projectContext;

   void setUp()
   {
      mGameApplication = new dtGame::GameApplication(0, NULL);
   }
   
   void tearDown()
   {
      mGameApplication = NULL;
   }

   void TestProperties()
   {
      std::string name = "client";
      
      CPPUNIT_ASSERT(mGameApplication->GetGameLibraryName().empty());
      mGameApplication->SetGameLibraryName(name);
      CPPUNIT_ASSERT(mGameApplication->GetGameLibraryName() == name);
   }

   void TestCleanup()
   {
      dtCore::SetDataFilePathList(projectContext);
      dtCore::ShaderManager::GetInstance().LoadShaderDefinitions("Shaders/TestShaderDefinitions.xml",false);

      CPPUNIT_ASSERT(dtCore::ShaderManager::GetInstance().GetNumShaderGroupPrototypes() > 0);
      dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager(*new dtCore::Scene);
      gm->AddComponent(*new dtGame::DefaultMessageProcessor(), dtGame::GameManager::ComponentPriority::HIGHEST);
      mGameApplication->SetGameManager(*gm);

      mGameApplication = NULL;

      CPPUNIT_ASSERT_MESSAGE("The shader manager should cleared on app shutdown", 
               dtCore::ShaderManager::GetInstance().GetNumShaderGroupPrototypes() == 0);

      CPPUNIT_ASSERT_MESSAGE("The GM should have been shutdown.",
               gm->GetComponentByName(dtGame::DefaultMessageProcessor::DEFAULT_NAME) == NULL);
   }

private:
   dtUtil::Log* mLogger;

   dtCore::RefPtr<dtGame::GameApplication> mGameApplication;

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameApplicationTests);


const std::string GameApplicationTests::TESTS_DIR(dtCore::GetDeltaRootPath()+dtUtil::FileUtils::PATH_SEPARATOR+"tests");
const std::string GameApplicationTests::projectContext(GameApplicationTests::TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "data" + dtUtil::FileUtils::PATH_SEPARATOR + "ProjectContext");

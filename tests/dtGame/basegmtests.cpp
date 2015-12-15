/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 */

#include "basegmtests.h"

#include <dtCore/system.h>
#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtABC/application.h>
#include <dtGame/exceptionenum.h>

extern dtABC::Application& GetGlobalApplication();

std::string GetTestsDir();

namespace dtGame
{
   const std::string BaseGMTestFixture::mTestGameActorLibrary = "testGameActorLibrary";
   const std::string BaseGMTestFixture::mTestActorLibrary     = "testActorLibrary";

   BaseGMTestFixture::BaseGMTestFixture(): mLogger(NULL) {}
   BaseGMTestFixture::~BaseGMTestFixture() {}

   void BaseGMTestFixture::setUp()
   {
      try
      {
         dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
         mLogger = &dtUtil::Log::GetInstance("GM Test Fixture");

         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mGM = new dtGame::GameManager(*GetGlobalApplication().GetScene());
         CPPUNIT_ASSERT_THROW_MESSAGE("Trying to get the application when it's NULL should fail.",
               mGM->GetApplication(), dtGame::GeneralGameManagerException);
         mGM->SetApplication(GetGlobalApplication());
         try
         {
            CPPUNIT_ASSERT(&GetGlobalApplication() == &mGM->GetApplication());
         }
         catch (const dtUtil::Exception&)
         {
            CPPUNIT_FAIL("The application should not be NULL.");
         }

         CPPUNIT_ASSERT(!mGM->IsShuttingDown());

         mDefMsgProc = new DefaultMessageProcessor;
         mTestComp = new TestComponent;

         mGM->AddComponent(*mDefMsgProc, GameManager::ComponentPriority::HIGHEST);
         mGM->AddComponent(*mTestComp, GameManager::ComponentPriority::NORMAL);


         NameVector libraries;
         GetRequiredLibraries(libraries);
         NameVector::const_iterator i,iend;
         i = libraries.begin();
         iend = libraries.end();
         for (; i != iend; ++i)
         {
            mGM->LoadActorRegistry(*i);
         }
         mTestComp->reset();
         const std::string PROJECT_CONTEXT = GetTestsDir() + dtUtil::FileUtils::PATH_SEPARATOR + "data" + dtUtil::FileUtils::PATH_SEPARATOR + "ProjectContext";
         mGM->SetProjectContext(PROJECT_CONTEXT);
         dtCore::Project::GetInstance().AddContext(dtUtil::GetDeltaRootPath() + "/examples/data");
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("Error: ") + e.What());
      }
      catch (const std::exception& ex)
      {
         CPPUNIT_FAIL(std::string("Error: ") + ex.what());
      }
   }

   ////////////////////////////////////////////////////////////////////////
   void BaseGMTestFixture::tearDown()
   {
      dtCore::System::GetInstance().SetPause(false);
      dtCore::System::GetInstance().Stop();
      if (mGM.valid())
      {
         try
         {
            mGM->DeleteAllActors(true);

            mGM->Shutdown();
            CPPUNIT_ASSERT(mGM->IsShuttingDown());

            NameVector libraries;
            GetRequiredLibraries(libraries);
            NameVector::const_iterator i,iend;
            i = libraries.begin();
            iend = libraries.end();
            for (; i != iend; ++i)
            {
               mGM->UnloadActorRegistry(*i);
            }

            mGM = NULL;
            mDefMsgProc  = NULL;
            mTestComp    = NULL;
         }
         catch(const dtUtil::Exception& e)
         {
            CPPUNIT_FAIL((std::string("Error: ") + e.ToString()).c_str());
         }
      }

   }
}


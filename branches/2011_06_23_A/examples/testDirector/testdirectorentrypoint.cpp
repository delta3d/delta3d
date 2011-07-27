/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * MG
 */

#include "testdirectorentrypoint.h"
#include "directorcomponent.h"

#include <dtUtil/datapathutils.h>
#include <dtCore/deltawin.h>
#include <dtDAL/project.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/exceptionenum.h>
#include <dtAudio/audiomanager.h>
#include <dtDirector/director.h>

extern "C" TEST_DIRECTOR_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new TestDirector;
}

////////////////////////////////////////////////////////////////////////////////
extern "C" TEST_DIRECTOR_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

////////////////////////////////////////////////////////////////////////////////
TestDirector::TestDirector()
   : dtGame::GameEntryPoint() 
{
}

////////////////////////////////////////////////////////////////////////////////
TestDirector::~TestDirector()
{
   if (dtAudio::AudioManager::GetInstance().IsInitialized())
   {
      dtAudio::AudioManager::Destroy();
   }

}

////////////////////////////////////////////////////////////////////////////////
void TestDirector::Initialize(dtGame::GameApplication& app, int argc, char** argv)
{

}

////////////////////////////////////////////////////////////////////////////////
void TestDirector::OnStartup(dtGame::GameApplication& app)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" + dtUtil::GetDeltaRootPath() + "/examples/data" + ";"); 

   // The game manager will hold a ref pointer to all gm components
   DirectorComponent* directorComponent = new DirectorComponent();
   app.GetGameManager()->AddComponent(*directorComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   try
   {
      // Ideally this would come from a config file...

      // Get the screen size
      int x, y, width, height;
      app.GetWindow()->GetPosition(x, y, width, height);
      app.GetWindow()->SetPosition(x, y, 1024, 768);
      app.GetWindow()->SetWindowTitle("TestDirector");

      std::string context = dtUtil::GetDeltaRootPath() + "/examples/data/demoMap";
      dtDAL::Project::GetInstance().SetContext(context, true);
    
      std::vector<std::string> mapNames;
      mapNames.push_back("TestDirector");
      mapNames.push_back("TestDirectorScript");

      app.GetGameManager()->ChangeMapSet(mapNames);
      //app.GetGameManager()->ChangeMap("TestDirector");

   }
   catch (const dtUtil::Exception& e)
   {
      LOG_ERROR("Can't find the project context or load the map. Exception follows.");
      e.LogException(dtUtil::Log::LOG_ERROR);
   }
}

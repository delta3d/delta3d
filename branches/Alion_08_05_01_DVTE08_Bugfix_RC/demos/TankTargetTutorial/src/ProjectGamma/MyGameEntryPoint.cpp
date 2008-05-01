/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation & MOVES Institute
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */
#include <sstream> 
 
#include "MyGameEntryPoint.h"
#include "HUDComponent.h"
#include "TankActor.h"

#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/gameapplication.h>
#include <dtGame/exceptionenum.h>

#include <dtCore/camera.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/keyboard.h>
#include <dtCore/globals.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>

#include <dtDAL/project.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtABC/application.h>

#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>

//////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new MyGameEntryPoint;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

//////////////////////////////////////////////////////////////////////////
MyGameEntryPoint::~MyGameEntryPoint()
{
}

//////////////////////////////////////////////////////////////////////////
void MyGameEntryPoint::Initialize(dtGame::GameApplication& app, int argc, char **argv)
         throw(dtUtil::Exception)
{
   osg::ArgumentParser parser(&argc, argv);

   parser.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   parser.getApplicationUsage()->addCommandLineOption("--mapName", "The name of the map to load in. This must be a map that is located within the project path specified");

   if (parser.read("-h") || parser.read("--help") || parser.read("-?") || parser.read("--?") ||
      parser.argc() == 0)
   {
      parser.getApplicationUsage()->write(std::cerr);
      throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, "Command Line Error.", __FILE__, __LINE__);
   }
   
   mMapName.reserve(512);
   
   if (!parser.read("--mapName", mMapName))
   {
      mMapName = "mapone";
   }
   
}

//////////////////////////////////////////////////////////////////////////
void MyGameEntryPoint::OnStartup(dtGame::GameApplication& app)
{
   // init our file path so it can find GUI Scheme
   // add extra data paths here if you need them
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());   

   // Add Component - DefaultMessageProcessor 
   dtGame::DefaultMessageProcessor *dmp = new dtGame::DefaultMessageProcessor("DefaultMessageProcessor");
   app.GetGameManager()->AddComponent(*dmp,dtGame::GameManager::ComponentPriority::HIGHEST);

   // Load the map we created in STAGE.
   dtDAL::Project::GetInstance().SetContext("StageProject");
   app.GetGameManager()->ChangeMap("mapone");

   // TUTORIAL - ADD YOUR HUD COMPONENT HERE
   
   // offset our camera a little back and above the tank.
   dtCore::Transform tx(0.0f, 0.7f, 2.2f, 0.0f, 0.0f, 0.0f);
   app.GetCamera()->SetTransform(tx); 
   app.GetScene()->UseSceneLight(true);

   // Attach our camera to the tank from the map
   std::vector<dtDAL::ActorProxy*> tanks;
   app.GetGameManager()->FindActorsByName("HoverTank", tanks);
   if (tanks.size() > 0)
   {
      tanks[0]->GetActor()->AddChild(app.GetCamera());
   }

   // Allow the fly motion model to move the camera around independent of the tank.
   dtCore::FlyMotionModel *fmm = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse(), false);
   fmm->SetMaximumFlySpeed(15);
   fmm->SetTarget(app.GetCamera());

   app.GetWindow()->SetWindowTitle("Tutorial");
}

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
#include "TankActor.h"

#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>

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
void MyGameEntryPoint::OnStartup(dtGame::GameApplication& app)
{
   // Init our file path so it can find GUI Scheme. Add extra data paths here if you need them
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList() + 
      ";" + dtCore::GetDeltaRootPath() + "/examples/data;"); 
   std::cout << "Path list is: " << dtCore::GetDataFilePathList() <<  std::endl;

   // TUTORIAL - SET CONTEXT AND LOAD MAP HERE 
   //dtDAL::Project::GetInstance().SetContext("StageProject");
   //app.GetGameManager()->ChangeMap("mapone");

   // TUTORIAL - (OPTIONAL) ADD A FLYMOTIONMODEL HERE 
   //dtCore::FlyMotionModel *fmm = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse(), false);
   //fmm->SetMaximumFlySpeed(15);
   //fmm->SetTarget(app.GetCamera());


   app.GetWindow()->SetWindowTitle("Tutorial");
   app.GetGameManager()->DebugStatisticsTurnOn(true, true, 15, true);
   // Tell the system to use a base scene light. With your own lighting, set this to false.
   app.GetScene()->UseSceneLight(true);
}

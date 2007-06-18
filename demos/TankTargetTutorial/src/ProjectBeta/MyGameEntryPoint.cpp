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
void MyGameEntryPoint::OnStartup(dtGame::GameManager &gameManager)
{
   dtABC::Application& app = gameManager.GetApplication();

   // init our file path so it can find GUI Scheme
   // add extra data paths here if you need them
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());   

   // TUTORIAL - SET CONTEXT AND LOAD MAP HERE 

   // offset our camera a little back and above the tank.
   dtCore::Transform tx(0.0f, 0.7f, 2.2f, 0.0f, 0.0f, 0.0f);
   app.GetCamera()->SetTransform(tx); 
   gameManager.GetScene().UseSceneLight(true);

   // TUTORIAL - FIND YOUR HOVER TANK AND ADD CAMERA AS A CHILD HERE 

   // TUTORIAL - (OPTIONAL) ADD A FLYMOTIONMODEL HERE 

   app.GetWindow()->SetWindowTitle("Tutorial");
}

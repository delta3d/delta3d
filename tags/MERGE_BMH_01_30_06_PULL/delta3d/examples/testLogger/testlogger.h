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
* @author Matthew W. Campbell
*/
#ifndef DELTA_TESTLOGGER_H
#define DELTA_TESTLOGGER_H

#include <dtABC/application.h>
#include <dtGame/logstatus.h>
#include <dtGame/message.h>

namespace dtCore
{
   class OrbitMotionModel;
}

namespace dtGame
{
   class ClientGameManager;
   class GameActorProxy;
   class ServerLoggerComponent;
   class LogController;
}

class LoggerApplication : public dtABC::Application
{         
   public:
      enum CameraMode {
         CAM_FREE_FLY = 0,
         CAM_FOLLOW_PLAYER = 1,
      };
   
      LoggerApplication();
      virtual ~LoggerApplication();      
      void Config();
      void KeyPressed(dtCore::Keyboard *keyBoard, Producer::KeyboardKey key,
         Producer::KeyCharacter character);
         
      void SetCameraMode(CameraMode mode);      
      void OnReceivedStatus(const dtGame::LogStatus &newStatus); 
      void OnReceivedRejection(const dtGame::Message &newMessage); 

   protected:
      void SetupScene();
      void PreFrame(const double deltaFrameTime);
      
      dtCore::RefPtr<dtGame::GameActorProxy> CreateNewMovingActor
         (const std::string &meshName, float velocity, float turnRate, bool bSetLocation);
      
      float Random(float min,float max) 
      { 
         return min + (max-min)*float(rand())/float(RAND_MAX); 
      }

private:
      dtCore::RefPtr<dtGame::ClientGameManager> mClientGM;
      dtCore::RefPtr<dtGame::LogController> mLogController;
    
      bool mPlayerIsValid; // set to hide him during playback and such.  
      dtCore::RefPtr<dtGame::GameActorProxy> mPlayer;
      dtCore::RefPtr<dtCore::OrbitMotionModel> mMotionModel;

      CameraMode mCameraMode;      
      bool mKeyIsPressed;
      double mSimSpeedFactor;
};
      
#endif 

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
#ifndef DELTA_TESTAAR
#define DELTA_TESTAAR

#include <dtABC/application.h>
#include <dtGame/logstatus.h>
#include <dtGame/message.h>

namespace dtActors
{
   class TaskActorProxy;
}

namespace dtCore
{
   class FlyMotionModel;
}

namespace dtGame
{
   class ClientGameManager;
   class GameActorProxy;
   class ServerLoggerComponent;
   class LogController;
   class TaskComponent;
}

/**
 * Exceptions that may be thrown by the demo.
 */
class AppException : public dtUtil::Enumeration
{
   DECLARE_ENUM(AppException);
   public:
      static AppException INIT_ERROR;
   private:
      AppException(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
};

/**
 * This Delta3D application tests the after review support of Delta3D.
 * After action review consists of two major components, task (objective)
 * tracking, and record/playback.  Therefore, this application tests these 
 * two primary components.
 */
class AARApplication : public dtABC::Application
{         
   public:
      
      /**
       * Constructs the test application.
       */       
      AARApplication();

   protected:

      /**
       * Destroys the test application.
       */
      virtual ~AARApplication();      
   
   public:   

      /**
       * Configures the application and prepares it to start the game/simulation.
       */
      void Config();
     
      /**
       * Called when a keyboard key is pressed.
       * @param keyBoard The keyboard object.
       * @param key The key that was pressed.
       * @param character Character pressed if the key represented an ASCII character.
       */     
      bool HandleKeyPressed(dtCore::Keyboard *keyBoard, Producer::KeyboardKey key,
         Producer::KeyCharacter character);
      
      /**
       * Slot method called from the log controller when a status message is received
       * from the logging component.
       * @param newStatus Contains the status object received from the logging component.
       */
      void OnReceivedStatus(const dtGame::LogStatus &newStatus); 
      
      /**
       * Slot method called from the log controller when a rejection message is sent 
       * from the logging component to the log controller.
       * @param The rejection message.  Note, this has the causing message in it as well.
       */
      void OnReceivedRejection(const dtGame::Message &newMessage);
      
      /**
       * Displays the current active tasks and their status to the console.
       */
      void PrintTasks();       

   protected:
      
      /**
       * Creates our initial scene.
       */
      void SetupScene();
      
      /**
       * Creates all the demo tasks.
       */
      void SetupTasks();
      
      /**
       * Called by the Delta3D system each time a frame is about to be rendered.
       * @deltaFrameTime The tick time between frames.
       */
      void PreFrame(const double deltaFrameTime);
      
      /**
       * Helper method to create a new actor to put in the scene.
       */
      dtCore::RefPtr<dtGame::GameActorProxy> CreateNewMovingActor(
         const std::string &meshName, float velocity, float turnRate, bool bSetLocation);
      
      /**
       * Helper method to calculate a random value between min and max.
       */
      float Random(float min, float max) 
      { 
         return min + (max-min)*float(rand())/float(RAND_MAX); 
      }
      
      /**
       * Helper method that creates an actor with random movement behavior.
       */
      void PlaceActor();

private:
      dtCore::RefPtr<dtGame::ClientGameManager> mClientGM;
      dtCore::RefPtr<dtGame::LogController> mLogController;
      dtCore::RefPtr<dtGame::TaskComponent> mTaskComponent;
      
      //Our tasks and objectives
      dtCore::RefPtr<dtActors::TaskActorProxy> mRootTask;
      dtCore::RefPtr<dtActors::TaskActorProxy> mObjectPlacementTask;
    
      bool mPlayerIsValid; // set to hide him during playback and such.  
      dtCore::RefPtr<dtGame::GameActorProxy> mPlayer;
      dtCore::RefPtr<dtCore::FlyMotionModel> mMotionModel;

      bool mKeyIsPressed;
      double mSimSpeedFactor;
};
      
#endif 

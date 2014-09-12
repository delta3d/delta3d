/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

#ifndef DELTA_TEST_APP_INPUT
#define DELTA_TEST_APP_INPUT

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "export.h"
#include "testappconstants.h"
#include "testappgamestates.h"
#include <dtGame/baseinputcomponent.h>
#include <dtCore/motionmodel.h>
#include <dtCore/refptr.h>
#include <dtCore/transformable.h>
#include <dtCore/transformableactorproxy.h>
#include <dtGame/defaultgroundclamper.h>


namespace dtInspectorQt { class InspectorQt; }

namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT InputComponent : public dtGame::BaseInputComponent
   {
      public:
         typedef dtGame::BaseInputComponent BaseClass;

         static const dtUtil::RefString DEFAULT_NAME;
         static const dtUtil::RefString DEFAULT_ATTACH_ACTOR_NAME;
         static const dtUtil::RefString DEFAULT_PLAYER_START_NAME;
         static const dtUtil::RefString DEFAULT_TERRAIN_NAME;

         // Constructor
         InputComponent();

         /*override*/ bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

         /*override*/ bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

         bool HandleMotionModelKey(int key);

         void HandleGameStateChange(
            dtGame::GameState::Type& newState,
            dtGame::GameState::Type& oldState);

         // General message handler
         virtual void ProcessMessage(const dtGame::Message& message);

         virtual void OnAddedToGM();

         void Update(float simTimeDelta, float realTimeDelta);

      protected:

         /// Destructor
         virtual ~InputComponent();

         dtCore::TransformableActorProxy* GetActorByName(const std::string& name);
         dtCore::Transformable* GetDrawableByName(const std::string& name);

         void SetCameraToPlayerStart();

         bool SetCameraPivot(const std::string& actorName);

         void SendTransitionMessage(const dtExample::Transition& transition);
         
         void SendUIMessage(const std::string& uiName, const dtGame::MessageType& messageType);
         void SendUIToggleMessage(const std::string& uiName);
         void SendUIVisibilityMessage(const std::string& uiName, bool visible);

         void SendMotionModelChangedMessage(const dtExample::MotionModelType& motionModelType);

         void SetMotionModel(const dtExample::MotionModelType& motionModelType);

         void IncrementTime(float numSeconds);
         
         void DoGroundClamping(float simTime);

      private:
         float mTimeOffset;

         bool mClampCameraEnabled;
      
         bool mMotionModelsEnabled;
         const dtExample::MotionModelType* mMotionModelMode;
         const dtExample::MotionModelType* mMotionModelMode_Previous;
         dtCore::RefPtr<dtCore::MotionModel> mMotionModel;

         dtCore::RefPtr<dtCore::Transformable> mCamera;
         dtCore::RefPtr<dtCore::Transformable> mCameraPivot;
         dtCore::RefPtr<dtCore::Transformable> mGroundClampedXformable;

         // A reference to an actor is need for ground clamping.
         // Use a transformable actor to hold the camera since clamping
         // on the camera object will not work on it directly.
         dtCore::RefPtr<dtCore::TransformableActorProxy> mGroundClampedObject;

         dtCore::RefPtr<dtGame::DefaultGroundClamper> mGroundClamper;

#if defined(USE_INSPECTOR)
         dtInspectorQt::InspectorQt* mInspector;
#endif

         std::string mAttachActorName;

   };

} // END - namespace dtExample

#endif

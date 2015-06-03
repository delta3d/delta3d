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
#include <dtActors/watergridactor.h>
#include <dtABC/beziercontroller.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtInspectorQt
{
   class InspectorQt;
}

namespace dtRender
{
   class SceneManager;
}



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT InputComponent : public dtGame::BaseInputComponent
   {
      public:
         typedef dtGame::BaseInputComponent BaseClass;

         static const dtUtil::RefString DEFAULT_ATTACH_ACTOR_NAME;
         static const dtUtil::RefString DEFAULT_PLAYER_START_NAME;
         static const dtUtil::RefString DEFAULT_TERRAIN_NAME;

         // Constructor
         InputComponent();

         void SetMotionModelEnabled(bool enabled);
         bool IsMotionModelEnabled() const;

         /*override*/ bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta);

         /*override*/ bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

         /*override*/ bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

         bool HandleMotionModelKey(int key);

         void HandleGameStateChange(dtGame::GameState::Type& newState);

         // General message handler
         virtual void ProcessMessage(const dtGame::Message& message);

         virtual void OnAddedToGM();

         void Update(float simTimeDelta, float realTimeDelta);

         void ToggleFireworks();

         void ToggleAIWaypointDrawMode();

         void TogglePhysicsDrawMode();

         void ReloadShaders();

         void SetLampIntensity(float intensity);

         void SetSeaState(dtActors::WaterGridActor::SeaState&);
         dtActors::WaterGridActor::SeaState& GetSeaState() const;
         
         dtActors::WaterGridActor::ChopSettings& GetWaterChop() const;
         void SetWaterChop(dtActors::WaterGridActor::ChopSettings&);

         void SetAmbience(float amt);
         float GetAmbience();

         void SetLuminance(float amt);
         float GetLuminance();

      protected:

         /// Destructor
         virtual ~InputComponent();

         bool AttachToBezierController(const dtCore::UniqueId&);
         void DetachFromController();

         dtCore::TransformableActorProxy* GetActorByName(const std::string& name);
         dtCore::TransformableActorProxy* GetActorById(const dtCore::UniqueId& id);
         dtCore::Transformable* GetDrawableByName(const std::string& name);
         dtCore::Transformable* GetDrawableById(const dtCore::UniqueId& id);

         void SetCameraToPlayerStart();

         bool SetCameraPivotByName(const std::string& actorName);
         bool SetCameraPivotById(const dtCore::UniqueId& id);
         bool SetCameraPivot(dtCore::Transformable* drawable);

         void SendTransitionMessage(const dtExample::Transition& transition);
         
         void SendUIMessage(const std::string& uiName, const dtGame::MessageType& messageType);
         void SendUIToggleMessage(const std::string& uiName);
         void SendUIVisibilityMessage(const std::string& uiName, bool visible);

         void SendMotionModelChangedMessage(const dtExample::MotionModelType& motionModelType);

         void SetMotionModel(const dtExample::MotionModelType& motionModelType);

         void IncrementTime(float numSeconds);

         void IncrementMotionModelSpeed(float increment);

         void SetMotionModelSpeed(
            const dtExample::MotionModelType& motionModelType,
            dtCore::MotionModel& motionModel, float speed);
         
         void DoGroundClamping(float simTime);

         dtRender::SceneManager* GetSceneManager();
         
         // TEMP:
         // This is a temporary method for attaching the banner geometry to the plane.
         void AttachBanner();

      private:
         float mTimeOffset;
         float mLampIntensity;

         bool mClampCameraEnabled;
      
         bool mMotionModelsEnabled;
         float mMotionModelSpeed;
         const dtExample::MotionModelType* mMotionModelMode;
         const dtExample::MotionModelType* mMotionModelMode_Previous;
         dtCore::RefPtr<dtCore::MotionModel> mMotionModel;

         dtCore::RefPtr<dtCore::Transformable> mCamera;
         dtCore::RefPtr<dtCore::Transformable> mCameraPivot;
         dtCore::RefPtr<dtCore::Transformable> mGroundClampedXformable;
         dtCore::RefPtr<dtABC::BezierController> mCurrentController;

         // A reference to an actor is need for ground clamping.
         // Use a transformable actor to hold the camera since clamping
         // on the camera object will not work on it directly.
         dtCore::RefPtr<dtCore::TransformableActorProxy> mGroundClampedObject;

         dtCore::RefPtr<dtGame::DefaultGroundClamper> mGroundClamper;

#if defined(USE_INSPECTOR)
         dtInspectorQt::InspectorQt* mInspector;
#endif

         dtCore::UniqueId mAttachActorId;

   };

} // END - namespace dtExample

#endif

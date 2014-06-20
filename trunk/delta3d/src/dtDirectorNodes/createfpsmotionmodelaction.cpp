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
 * Author: Jeff P. Houde
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/createfpsmotionmodelaction.h>

#include <dtABC/application.h>

#include <dtCore/collisionmotionmodel.h>
#include <dtCore/collisioncategorydefaults.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/bitmaskactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   CreateFPSMotionModelAction::CreateFPSMotionModelAction()
      : ActionNode()
   {
      mActor         = "";
      mModelName     = "FPS Motion Model";
      mHeight        = 1.5f;
      mRadius        = 0.2f;
      mStepUpHeight  = 0.3f;
      mWalkSpeed     = 5.0f;
      mSidestepSpeed = 5.0f;
      mTurnSpeed     = 1.5f;
      mJumpSpeed     = 5.0f;
      mSlideSpeed    = 5.0f;
      mSmoothingSpeed= 20.0f;
      mSlideThreshold= 0.1f;
      mUseWASD       = true;
      mUseArrows     = true;
      mAllowJump     = true;
      mTorsoMask     = COLLISION_CATEGORY_MASK_OBJECT;
      mFeetMask      = COLLISION_CATEGORY_MASK_OBJECT;

      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   CreateFPSMotionModelAction::~CreateFPSMotionModelAction()
   {
      mCreatedMotionModel = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activated when the motion model could not be created."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetCurrentActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetCurrentActor),
         "", "The actor to attach the motion model to.");
      AddProperty(actorProp);

      dtCore::StringActorProperty* modelProp = new dtCore::StringActorProperty(
         "Motion Model", "Motion Model",
         dtCore::StringActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetModelName),
         dtCore::StringActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetModelName),
         "The name of the motion model.");
      AddProperty(modelProp);

      dtCore::FloatActorProperty* heightProp = new dtCore::FloatActorProperty(
         "Height", "Height",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetHeight),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetHeight),
         "The collision height of the motion model.");
      AddProperty(heightProp);

      dtCore::FloatActorProperty* radiusProp = new dtCore::FloatActorProperty(
         "Radius", "Radius",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetRadius),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetRadius),
         "The collision radius of the motion model.");
      AddProperty(radiusProp);

      dtCore::FloatActorProperty* stepProp = new dtCore::FloatActorProperty(
         "Step Height", "Step Height",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetStepUpHeight),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetStepUpHeight),
         "The step up height of the motion model.");
      AddProperty(stepProp);

      dtCore::FloatActorProperty* walkProp = new dtCore::FloatActorProperty(
         "Walk Speed", "Walk Speed",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetWalkSpeed),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetWalkSpeed),
         "The walk speed of the motion model.");
      AddProperty(walkProp);

      dtCore::FloatActorProperty* sideProp = new dtCore::FloatActorProperty(
         "Sidestep Speed", "Sidestep Speed",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetSidestepSpeed),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetSidestepSpeed),
         "The sidestep speed of the motion model.");
      AddProperty(sideProp);

      dtCore::FloatActorProperty* turnProp = new dtCore::FloatActorProperty(
         "Turn Speed", "Turn Speed",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetTurnSpeed),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetTurnSpeed),
         "The turn speed of the motion model.");
      AddProperty(turnProp);

      dtCore::FloatActorProperty* jumpSpeedProp = new dtCore::FloatActorProperty(
         "Jump Speed", "Jump Speed",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetJumpSpeed),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetJumpSpeed),
         "The jump speed.");
      AddProperty(jumpSpeedProp);

      dtCore::FloatActorProperty* slideSpeedProp = new dtCore::FloatActorProperty(
         "Slide Speed", "Slide Speed",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetSlideSpeed),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetSlideSpeed),
         "The speed in which the target will slide down a sloped surface.");
      AddProperty(slideSpeedProp);

      dtCore::FloatActorProperty* slideThresholdProp = new dtCore::FloatActorProperty(
         "Slide Threshold", "Slide Threshold",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetSlideThreshold),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetSlideThreshold),
         "The threshold in which the target will slide down a sloped surface.");
      AddProperty(slideThresholdProp);

      dtCore::FloatActorProperty* smoothingProp = new dtCore::FloatActorProperty(
         "Smoothing Speed", "Smoothing Speed",
         dtCore::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetSmoothingSpeed),
         dtCore::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetSmoothingSpeed),
         "The speed in which the target will smoothly lerp its position.");
      AddProperty(smoothingProp);

      dtCore::BooleanActorProperty* useWasdProp = new dtCore::BooleanActorProperty(
         "Use WASD Keys", "Use WASD Keys",
         dtCore::BooleanActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetUseWASD),
         dtCore::BooleanActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetUseWASD),
         "Allow WASD Keys for motion model movement.");
      AddProperty(useWasdProp);

      dtCore::BooleanActorProperty* useArrowsProp = new dtCore::BooleanActorProperty(
         "Use Arrow Keys", "Use Arrow Keys",
         dtCore::BooleanActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetUseArrows),
         dtCore::BooleanActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetUseArrows),
         "Allow Arrow Keys for motion model movement.");
      AddProperty(useArrowsProp);

      dtCore::BooleanActorProperty* jumpProp = new dtCore::BooleanActorProperty(
         "Allow Jump", "Allow Jump",
         dtCore::BooleanActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetAllowJump),
         dtCore::BooleanActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetAllowJump),
         "Allow jumping with the motion model.");
      AddProperty(jumpProp);

      dtCore::BitMaskActorProperty* torsoProp = new dtCore::BitMaskActorProperty(
         "Torso Collision", "Torso Collision",
         dtCore::BitMaskActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetTorsoCollisionMask),
         dtCore::BitMaskActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetTorsoCollisionMask),
         dtCore::BitMaskActorProperty::GetMaskListFuncType(this, &CreateFPSMotionModelAction::GetCollisionMaskList),
         "Torso collision bits for the motion model.");
      AddProperty(torsoProp);

      dtCore::BitMaskActorProperty* feetProp = new dtCore::BitMaskActorProperty(
         "Feet Collision", "Feet Collision",
         dtCore::BitMaskActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetFeetCollisionMask),
         dtCore::BitMaskActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetFeetCollisionMask),
         dtCore::BitMaskActorProperty::GetMaskListFuncType(this, &CreateFPSMotionModelAction::GetCollisionMaskList),
         "Feet collision bits for the motion model.");
      AddProperty(feetProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp));
      mValues.push_back(ValueLink(this, modelProp, false, false, true, false));
      mValues.push_back(ValueLink(this, heightProp, false, false, true, false));
      mValues.push_back(ValueLink(this, radiusProp, false, false, true, false));
      mValues.push_back(ValueLink(this, stepProp, false, false, true, false));
      mValues.push_back(ValueLink(this, walkProp, false, false, true, false));
      mValues.push_back(ValueLink(this, sideProp, false, false, true, false));
      mValues.push_back(ValueLink(this, turnProp, false, false, true, false));
      mValues.push_back(ValueLink(this, jumpSpeedProp, false, false, true, false));
      mValues.push_back(ValueLink(this, slideSpeedProp, false, false, true, false));
      mValues.push_back(ValueLink(this, slideThresholdProp, false, false, true, false));
      mValues.push_back(ValueLink(this, smoothingProp, false, false, true, false));
      mValues.push_back(ValueLink(this, useWasdProp, false, false, true, false));
      mValues.push_back(ValueLink(this, useArrowsProp, false, false, true, false));
      mValues.push_back(ValueLink(this, jumpProp, false, false, true, false));
      mValues.push_back(ValueLink(this, torsoProp, false, false, true, false));
      mValues.push_back(ValueLink(this, feetProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool CreateFPSMotionModelAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtABC::Application* app = dtABC::Application::GetInstance(0);
      std::string modelName = GetString("Motion Model");
      if (app && !modelName.empty())
      {
         dtCore::CollisionMotionModel* model = dtCore::CollisionMotionModel::GetInstance(modelName);
         if (model == NULL)
         {
            mCreatedMotionModel = new dtCore::CollisionMotionModel(
               GetFloat("Height"),
               GetFloat("Radius"),
               GetFloat("Step Height"),
               app->GetScene(),
               app->GetKeyboard(),
               app->GetMouse(),
               GetFloat("Walk Speed"),
               GetFloat("Turn Speed"),
               GetFloat("Sidestep Speed"),
               GetFloat("Jump Speed"),
               GetFloat("Slide Speed"),
               GetFloat("Slide Threshold"),
               GetBoolean("Allow Jump"),
               GetBoolean("Use WASD Keys"),
               GetBoolean("Use Arrow Keys"));

            model = mCreatedMotionModel.get();

            model->SetName(modelName);
            model->GetFPSCollider().SetCollisionBitsForTorso(GetUInt("Torso Collision"));
            model->GetFPSCollider().SetCollisionBitsForFeet(GetUInt("Feet Collision"));

            dtCore::ActorProxy* actor = GetActor("Actor");
            if (actor)
            {
               dtCore::Transformable* xformable = NULL;
               actor->GetDrawable(xformable);
               model->SetTarget(xformable);
            }

            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }

         if (model)
         {
            model->SetName(modelName);
            model->SetCanJump(GetBoolean("Allow Jump"));
            model->SetMaximumWalkSpeed(GetFloat("Walk Speed"));
            model->SetMaximumTurnSpeed(GetFloat("Turn Speed"));
            model->SetMaximumSidestepSpeed(GetFloat("Sidestep Speed"));
            model->SetUseWASD(GetBoolean("Use WASD Keys"));
            model->SetUseArrowKeys(GetBoolean("Use Arrow Keys"));
            model->GetFPSCollider().SetDimensions(GetFloat("Height"), GetFloat("Radius"), GetFloat("Step Height"));
            model->GetFPSCollider().SetJumpSpeed(GetFloat("Jump Speed"));
            model->GetFPSCollider().SetSlideSpeed(GetFloat("Slide Speed"));
            model->GetFPSCollider().SetSlideThreshold(GetFloat("Slide Threshold"));
            model->GetFPSCollider().SetSmoothingSpeed(GetFloat("Smoothing Speed"));
            model->GetFPSCollider().SetCollisionBitsForTorso(GetUInt("Torso Collision"));
            model->GetFPSCollider().SetCollisionBitsForFeet(GetUInt("Feet Collision"));
            model->SetScene(app->GetScene());

            dtCore::ActorProxy* actor = GetActor("Actor");
            if (actor)
            {
               dtCore::Transformable* xformable = NULL;
               actor->GetDrawable(xformable);
               model->SetTarget(xformable);
            }

            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }
      }

      ActivateOutput("Failed");
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId CreateFPSMotionModelAction::GetCurrentActor()
   {
      return mActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetModelName(const std::string& value)
   {
      mModelName = value;
      mName = mModelName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CreateFPSMotionModelAction::GetModelName() const
   {
      return mModelName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetHeight(float value)
   {
      mHeight = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetHeight() const
   {
      return mHeight;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetRadius(float value)
   {
      mRadius = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetRadius() const
   {
      return mRadius;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetStepUpHeight(float value)
   {
      mStepUpHeight = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetStepUpHeight() const
   {
      return mStepUpHeight;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetWalkSpeed(float value)
   {
      mWalkSpeed = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetWalkSpeed() const
   {
      return mWalkSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetSidestepSpeed(float value)
   {
      mSidestepSpeed = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetSidestepSpeed() const
   {
      return mSidestepSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetTurnSpeed(float value)
   {
      mTurnSpeed = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetTurnSpeed() const
   {
      return mTurnSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetJumpSpeed(float value)
   {
      mJumpSpeed = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetJumpSpeed() const
   {
      return mJumpSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetSlideSpeed(float value)
   {
      mSlideSpeed = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetSlideSpeed() const
   {
      return mSlideSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetSmoothingSpeed(float value)
   {
      mSmoothingSpeed = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetSmoothingSpeed() const
   {
      return mSmoothingSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetSlideThreshold(float value)
   {
      mSlideThreshold = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetSlideThreshold() const
   {
      return mSlideThreshold;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetUseWASD(bool value)
   {
      mUseWASD = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CreateFPSMotionModelAction::GetUseWASD() const
   {
      return mUseWASD;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetUseArrows(bool value)
   {
      mUseArrows = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CreateFPSMotionModelAction::GetUseArrows() const
   {
      return mUseArrows;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetAllowJump(bool value)
   {
      mAllowJump = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CreateFPSMotionModelAction::GetAllowJump() const
   {
      return mAllowJump;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetTorsoCollisionMask(unsigned int value)
   {
      mTorsoMask = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int CreateFPSMotionModelAction::GetTorsoCollisionMask() const
   {
      return mTorsoMask;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::SetFeetCollisionMask(unsigned int value)
   {
      mFeetMask = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int CreateFPSMotionModelAction::GetFeetCollisionMask() const
   {
      return mFeetMask;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::GetCollisionMaskList(std::vector<std::string>& names, std::vector<unsigned int>& values)
   {
      names.push_back("Proximity Trigger");
      values.push_back(COLLISION_CATEGORY_MASK_PROXIMITYTRIGGER);

      names.push_back("Camera");
      values.push_back(COLLISION_CATEGORY_MASK_CAMERA);

      names.push_back("Compass");
      values.push_back(COLLISION_CATEGORY_MASK_COMPASS);

      names.push_back("Infinite Terrain");
      values.push_back(COLLISION_CATEGORY_MASK_INFINITETERRAIN);

      names.push_back("ISector");
      values.push_back(COLLISION_CATEGORY_MASK_ISECTOR);

      names.push_back("Object");
      values.push_back(COLLISION_CATEGORY_MASK_OBJECT);

      names.push_back("Particle System");
      values.push_back(COLLISION_CATEGORY_MASK_PARTICLESYSTEM);

      names.push_back("Physical");
      values.push_back(COLLISION_CATEGORY_MASK_PHYSICAL);

      names.push_back("Point Axis");
      values.push_back(COLLISION_CATEGORY_MASK_POINTAXIS);

      names.push_back("Positional Light");
      values.push_back(COLLISION_CATEGORY_MASK_POSITIONALLIGHT);

      names.push_back("Spot Light");
      values.push_back(COLLISION_CATEGORY_MASK_SPOTLIGHT);

      names.push_back("Transformable");
      values.push_back(COLLISION_CATEGORY_MASK_TRANSFORMABLE);

      names.push_back("Listener");
      values.push_back(COLLISION_CATEGORY_MASK_LISTENER);

      names.push_back("Sound");
      values.push_back(COLLISION_CATEGORY_MASK_SOUND);

      names.push_back("Entity");
      values.push_back(COLLISION_CATEGORY_MASK_ENTITY);

      names.push_back("Terrain");
      values.push_back(COLLISION_CATEGORY_MASK_TERRAIN);

      names.push_back("Defaults");
      values.push_back(COLLISION_CATEGORY_MASK_ALLDEFAULTS);

      names.push_back("All");
      values.push_back(COLLISION_CATEGORY_MASK_ALL);
   }
}

////////////////////////////////////////////////////////////////////////////////

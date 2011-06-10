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

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/bitmaskactorproperty.h>

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
      mStepUpHeight  = 0.1f;
      mTheta         = 0.05f;
      mWalkSpeed     = 5.0f;
      mSidestepSpeed = 5.0f;
      mTurnSpeed     = 1.5f;
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
   }

   /////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void CreateFPSMotionModelAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetCurrentActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetCurrentActor),
         "", "The actor to attach the motion model to.");
      AddProperty(actorProp);

      dtDAL::StringActorProperty* modelProp = new dtDAL::StringActorProperty(
         "Motion Model", "Motion Model",
         dtDAL::StringActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetModelName),
         dtDAL::StringActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetModelName),
         "The name of the motion model.");
      AddProperty(modelProp);

      dtDAL::FloatActorProperty* heightProp = new dtDAL::FloatActorProperty(
         "Height", "Height",
         dtDAL::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetHeight),
         dtDAL::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetHeight),
         "The collision height of the motion model.");
      AddProperty(heightProp);

      dtDAL::FloatActorProperty* radiusProp = new dtDAL::FloatActorProperty(
         "Radius", "Radius",
         dtDAL::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetRadius),
         dtDAL::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetRadius),
         "The collision radius of the motion model.");
      AddProperty(radiusProp);

      dtDAL::FloatActorProperty* stepProp = new dtDAL::FloatActorProperty(
         "Step Height", "Step Height",
         dtDAL::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetStepUpHeight),
         dtDAL::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetStepUpHeight),
         "The step up height of the motion model.");
      AddProperty(stepProp);

      dtDAL::FloatActorProperty* thetaProp = new dtDAL::FloatActorProperty(
         "Theta", "Theta",
         dtDAL::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetTheta),
         dtDAL::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetTheta),
         "The theta of the motion model.");
      AddProperty(thetaProp);

      dtDAL::FloatActorProperty* walkProp = new dtDAL::FloatActorProperty(
         "Walk Speed", "Walk Speed",
         dtDAL::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetWalkSpeed),
         dtDAL::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetWalkSpeed),
         "The walk speed of the motion model.");
      AddProperty(walkProp);

      dtDAL::FloatActorProperty* sideProp = new dtDAL::FloatActorProperty(
         "Sidestep Speed", "Sidestep Speed",
         dtDAL::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetSidestepSpeed),
         dtDAL::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetSidestepSpeed),
         "The sidestep speed of the motion model.");
      AddProperty(sideProp);

      dtDAL::FloatActorProperty* turnProp = new dtDAL::FloatActorProperty(
         "Turn Speed", "Turn Speed",
         dtDAL::FloatActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetTurnSpeed),
         dtDAL::FloatActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetTurnSpeed),
         "The turn speed of the motion model.");
      AddProperty(turnProp);

      dtDAL::BooleanActorProperty* useWasdProp = new dtDAL::BooleanActorProperty(
         "Use WASD Keys", "Use WASD Keys",
         dtDAL::BooleanActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetUseWASD),
         dtDAL::BooleanActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetUseWASD),
         "Allow WASD Keys for motion model movement.");
      AddProperty(useWasdProp);

      dtDAL::BooleanActorProperty* useArrowsProp = new dtDAL::BooleanActorProperty(
         "Use Arrow Keys", "Use Arrow Keys",
         dtDAL::BooleanActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetUseArrows),
         dtDAL::BooleanActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetUseArrows),
         "Allow Arrow Keys for motion model movement.");
      AddProperty(useArrowsProp);

      dtDAL::BooleanActorProperty* jumpProp = new dtDAL::BooleanActorProperty(
         "Allow Jump", "Allow Jump",
         dtDAL::BooleanActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetAllowJump),
         dtDAL::BooleanActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetAllowJump),
         "Allow jumping with the motion model.");
      AddProperty(jumpProp);

      dtDAL::BitMaskActorProperty* torsoProp = new dtDAL::BitMaskActorProperty(
         "Torso Collision", "Torso Collision",
         dtDAL::BitMaskActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetTorsoCollisionMask),
         dtDAL::BitMaskActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetTorsoCollisionMask),
         dtDAL::BitMaskActorProperty::GetMaskListFuncType(this, &CreateFPSMotionModelAction::GetCollisionMaskList),
         "Torso collision bits for the motion model.");
      AddProperty(torsoProp);

      dtDAL::BitMaskActorProperty* feetProp = new dtDAL::BitMaskActorProperty(
         "Feet Collision", "Feet Collision",
         dtDAL::BitMaskActorProperty::SetFuncType(this, &CreateFPSMotionModelAction::SetFeetCollisionMask),
         dtDAL::BitMaskActorProperty::GetFuncType(this, &CreateFPSMotionModelAction::GetFeetCollisionMask),
         dtDAL::BitMaskActorProperty::GetMaskListFuncType(this, &CreateFPSMotionModelAction::GetCollisionMaskList),
         "Feet collision bits for the motion model.");
      AddProperty(feetProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp));
      mValues.push_back(ValueLink(this, modelProp, false, false, true, false));
      mValues.push_back(ValueLink(this, heightProp, false, false, true, false));
      mValues.push_back(ValueLink(this, radiusProp, false, false, true, false));
      mValues.push_back(ValueLink(this, stepProp, false, false, true, false));
      mValues.push_back(ValueLink(this, thetaProp, false, false, true, false));
      mValues.push_back(ValueLink(this, walkProp, false, false, true, false));
      mValues.push_back(ValueLink(this, sideProp, false, false, true, false));
      mValues.push_back(ValueLink(this, turnProp, false, false, true, false));
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
         if (!model)
         {
            model = new dtCore::CollisionMotionModel(
               GetFloat("Height"),
               GetFloat("Radius"),
               GetFloat("Step Height"),
               GetFloat("Theta"),
               app->GetScene(),
               app->GetKeyboard(),
               app->GetMouse(),
               GetFloat("Walk Speed"),
               GetFloat("Turn Speed"),
               GetFloat("Sidestep Speed"),
               GetBoolean("Use WASD Keys"),
               GetBoolean("Use Arrow Keys"));

            if (model)
            {
               model->SetName(modelName);
               model->SetCanJump(GetBoolean("Allow Jump"));
               model->GetFPSCollider().SetCollisionBitsForTorso(GetUInt("Torso Collision"));
               model->GetFPSCollider().SetCollisionBitsForFeet(GetUInt("Feet Collision"));
               model->SetScene(app->GetScene());

               dtDAL::ActorProxy* proxy = GetActor("Actor");
               if (proxy)
               {
                  dtCore::Transformable* actor = NULL;
                  proxy->GetActor(actor);
                  model->SetTarget(actor);
               }

               return ActionNode::Update(simDelta, delta, input, firstUpdate);
            }
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
   void CreateFPSMotionModelAction::SetTheta(float value)
   {
      mTheta = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float CreateFPSMotionModelAction::GetTheta() const
   {
      return mTheta;
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

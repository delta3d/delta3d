#include "civilianaiactorcomponent.h"
//#include "testapputils.h"
#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtGame/invokable.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtCore/propertymacros.h>
#include <dtAI/waypointinterface.h>
#include <dtAI/aiactorregistry.h>
#include <dtPhysics/raycast.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/physicsactcomp.h>

#include <dtAnim/animationtransitionplanner.h>
#include <dtAnim/walkrunblend.h>
#include <dtAnim/animationhelper.h>
#include <dtGame/deadreckoninghelper.h>
#include <dtGame/drpublishingactcomp.h>
#include <dtGame/basemessages.h>
#include <dtUtil/functor.h>
#include <dtUtil/mathdefines.h>


#include <dtAnim/posesequence.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>

namespace dtExample
{

   const dtGame::ActorComponent::ACType CivilianAIActorComponent::TYPE( new dtCore::ActorType("CivilianAI", "ActorComponents", "", dtGame::ActorComponent::BaseActorComponentType));

   static const std::string LOG_NAME("civilianaiactorcomponent.cpp");

   ////////////////////////////////////////////////////////////////////////
   CivilianAIActorComponent::CivilianAIActorComponent(const ACType& type)
   : dtGame::ActorComponent(type)
   , mAnimationWalkSpeed(0.79f)
   , mAnimationRunSpeed(1.4f)
   , mAnimationCrawlSpeed(0.2f)
   , mAnimationLowWalkSpeed(0.4f)
   , mStepHeight(0.3f)
   , mMaxIncline(70.0f)
   , mLookAtCameraRange(15.0f)
   , mEntityIndex(0)
   , mIgnoreRotation()
   , mEnablePoseMeshes(false)
   , mHasDestination()
   , mHasArrived()
   , mLookedAtNearTarget()
   , mWalkSpeed(0.79f)
   , mRotationSpeed(90.0f)
   , mHPR()
   , mAIInterface(NULL)
   , mCharacterController(NULL)
   , mDestinationWaypoint()
   , mCurrentWaypoint(NULL)
   , mWaypointPath()
   {
      SetClassName("dtExample.CivilianAIActorComponent");
      // Things run on many threads here, so initialize the logger up front.
      dtUtil::Log::GetInstance(LOG_NAME);
   }

   ////////////////////////////////////////////////////////////////////////
   CivilianAIActorComponent::~CivilianAIActorComponent()
   {

   }

   ////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      const dtUtil::RefString CIV_GROUP("Civilian");
      typedef dtCore::PropertyRegHelper<CivilianAIActorComponent&, CivilianAIActorComponent> RegHelperType;
      RegHelperType propReg(*this, this, CIV_GROUP);

      DT_REGISTER_PROPERTY(WalkSpeed, "The walking speed.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(RotationSpeed, "The speed the character turns when changing direction.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(StepHeight, "The maximum height this character can step up.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(MaxIncline, "The maximum incline in degrees the character can climb..", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(LookAtCameraRange, "The maximum range at which to look at the camera, currently when sitting or standing", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AnimationWalkSpeed, "The inherent speed of the walk animation.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AnimationRunSpeed, "The inherent speed of the run animation.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AnimationCrawlSpeed, "The inherent speed of the crawl animation.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AnimationLowWalkSpeed, "The inherent speed of the low walk animation.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(EnablePoseMeshes, "Enables the use of the pose meshes, allowing characters to look at specific targets.", RegHelperType, propReg);
   }

   ////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();
      dtGame::GameActorProxy* owner = NULL;
      GetOwner(owner);

      RegisterForTick();

      owner->RegisterForMessagesAboutSelf(dtGame::MessageType::INFO_TIMER_ELAPSED, dtUtil::MakeFunctor(&CivilianAIActorComponent::OnLookAtTimer, this));

      dtAnim::AnimationTransitionPlanner* planner = owner->GetComponent<dtAnim::AnimationTransitionPlanner>();
      planner->SignalAnimationsTransitioning.connect_slot(this, &CivilianAIActorComponent::OnAnimationsTransitioning);
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnAddedToActor(dtCore::BaseActorObject& actor)
   {
      mTransformable = actor.GetDrawable<dtCore::Transformable>();
      SetName(actor.GetName());
      if (!GetOwner()->HasComponent(dtAnim::AnimationTransitionPlanner::TYPE))
      {
         GetOwner()->AddComponent(* new dtAnim::AnimationTransitionPlanner);
      }
      if (!GetOwner()->HasComponent(dtGame::DeadReckoningActorComponent::TYPE))
      {
         GetOwner()->AddComponent(* new dtGame::DeadReckoningActorComponent);
      }
      if (!GetOwner()->HasComponent(dtGame::DRPublishingActComp::TYPE))
      {
         GetOwner()->AddComponent(* new dtGame::DRPublishingActComp);
      }

      dtPhysics::PhysicsActComp* pac = GetOwner()->GetComponent<dtPhysics::PhysicsActComp>();

      if (pac == NULL)
      {
         dtCore::RefPtr<dtPhysics::PhysicsActComp> physAC = new dtPhysics::PhysicsActComp();
         dtCore::RefPtr<dtPhysics::PhysicsObject> physicsObject = dtPhysics::PhysicsObject::CreateNew("Body");
         physicsObject->SetPrimitiveType(dtPhysics::PrimitiveType::CYLINDER);
         physicsObject->SetMechanicsType(dtPhysics::MechanicsType::KINEMATIC);
         physicsObject->SetCollisionGroup(6);
         physicsObject->SetMass(100.0f);
         dtPhysics::VectorType extents = dtPhysics::VectorType(1.8f, 0.2f, 0.0f);
         physicsObject->SetExtents(extents);
         dtPhysics::VectorType offset = dtPhysics::VectorType(0.0f, 0.0f, extents.x( )/ 2.0f);
         if (physicsObject->GetPrimitiveType() == dtPhysics::PrimitiveType::CAPSULE)
            offset.z() += extents.y() * 2.0f;
         physicsObject->SetOriginOffset(offset);
         physAC->AddPhysicsObject(*physicsObject);
         physAC->SetAutoCreateOnEnteringWorld(true);

         GetOwner()->AddComponent(*physAC);
         pac = physAC;
      }

      pac->SetPrePhysicsCallback(dtUtil::MakeFunctor(&CivilianAIActorComponent::PrePhysicsUpdate, this));
      pac->SetPostPhysicsCallback(dtUtil::MakeFunctor(&CivilianAIActorComponent::PostPhysicsUpdate, this));

      // transition planner should add this.
      GetOwner()->GetComponent<dtAnim::AnimationHelper>()->ModelLoadedSignal.connect_slot(this, &CivilianAIActorComponent::OnModelLoaded);

   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      mTransformable = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::HasDestination() const
   {
      return mHasDestination;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::IsAtDestination() const
   {
      return mHasArrived;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::ShouldMove() const
   {
      return !mWaypointPath.empty() && HasDestination();
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::PrePhysicsUpdate()
   {
      dtCore::Transform xform;
      mTransformable->GetTransform(xform);

      dtPhysics::PhysicsActComp* pac = GetOwner()->GetComponent<dtPhysics::PhysicsActComp>();

      if (pac != NULL && pac->GetMainPhysicsObject() != NULL)
      {
         // If the transform was set externally
         if (!xform.EpsilonEquals(mLastTransform, 0.01f))
         {
            pac->GetMainPhysicsObject()->SetTransformAsVisual(xform);
            osg::Vec3 newPos = pac->GetMainPhysicsObject()->GetTranslation();
            // Adjust the controller shape by the physics origin offset on the physics object
            newPos += pac->GetMainPhysicsObject()->GetOriginOffset();

            if (mCharacterController.valid())
               mCharacterController->Warp(newPos);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::PostPhysicsUpdate()
   {
      osg::Vec3 newPos;
      bool posValid = false;
      if (mCharacterController.valid())
      {
         newPos = mCharacterController->GetTranslation();
         posValid = true;
      }

      dtPhysics::PhysicsActComp* pac = GetOwner()->GetComponent<dtPhysics::PhysicsActComp>();

      if (posValid && pac != NULL && pac->GetMainPhysicsObject() != NULL)
      {
         // Adjust the controller shape by the physics origin offset on the physics object
         newPos -= pac->GetMainPhysicsObject()->GetOriginOffset();
         // Get the transform of the physics object
         pac->GetMainPhysicsObject()->SetTranslation(newPos);

         dtCore::Transform xform;
         // Move the physics object
         pac->GetMainPhysicsObject()->GetTransformAsVisual(xform);
         mTransformable->SetTransform(xform);
      }
   }


   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      PerformMove(tickMessage.GetDeltaSimTime());
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnLookAtTimer(const dtGame::TimerElapsedMessage& timerMsg)
   {
      dtGame::GameManager* gm = GetOwner<dtGame::GameActorProxy>()->GetGameManager();
      if (gm != NULL)
      {
         dtAnim::AnimationHelper* animHelper = NULL;
         GetOwner()->GetComponent(animHelper);
         // Reset the controller so that old controls do not conflict.
         dtAnim::PoseController* controller = animHelper->GetPoseController();

         dtAnim::AnimationTransitionPlanner* transitioner = NULL;
         GetOwner()->GetComponent(transitioner);

         // Look
         if (transitioner->GetStance() == dtAnim::BasicStanceEnum::STANDING ||
               transitioner->GetStance() == dtAnim::BasicStanceEnum::SITTING)
         {

            dtCore::Camera* camera = gm->GetApplication().GetCamera();
            if (camera != NULL)
            {
               dtCore::Transform xform, camxform;
               camera->GetTransform(camxform);
               mTransformable->GetTransform(xform);

               osg::Vec3 dist = camxform.GetTranslation() - xform.GetTranslation();
               if (!mLookedAtNearTarget && dist.length2() < mLookAtCameraRange * mLookAtCameraRange)
               {
                  controller->SetTarget(gm->GetApplication().GetCamera());
                  controller->SetTargetOffset(osg::Vec3(0.0f, 0.0f, 11.3f));
                  animHelper->SetPosesEnabled(true);
                  mLookedAtNearTarget = true;
                  gm->SetTimer("StopLooking", GetOwner<dtGame::GameActorProxy>(), 10.0f, false);
                  //LOG_ALWAYS(mTransformable->GetName() + " Starting to look.");
               }
               else if (timerMsg.GetTimerName() == "StopLooking")
               {
                  controller->SetTarget(NULL);
                  animHelper->SetPosesEnabled(false);
                  mLookedAtNearTarget = true;
                  //LOG_ALWAYS(mTransformable->GetName() + " Stopping looking.");
               }
               else if (mLookedAtNearTarget && dist.length2() > mLookAtCameraRange * mLookAtCameraRange)
               {
                  if (animHelper->GetPosesEnabled())
                  {
                     controller->SetTarget(NULL);
                     animHelper->SetPosesEnabled(false);
                  }
                  mLookedAtNearTarget = false;
                  //LOG_ALWAYS(mTransformable->GetName() + " Out of range reset.");
               }
            }
         }
         else
         {
            animHelper->SetPosesEnabled(false);
         }
      }

   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::PerformMove(float dt)
   {
      if (!mWaypointPath.empty() && mHasDestination)
      {
         // if we have gotten to the current waypoint
         if (IsAtWaypoint(mWaypointPath.front()))
         {
            mCurrentWaypoint = mWaypointPath.front();
            mWaypointPath.pop_front();
            LOGN_DEBUG(LOG_NAME, "Waypoint: " + dtUtil::ToString(mCurrentWaypoint->GetID()));
         }

         // if we have another waypoint to goto, goto it
         if (!mWaypointPath.empty())
         {
            ApplyStringPulling();
            MoveToPoint(dt, mWaypointPath.front());
         }
         else
         {
            mCharacterController->WalkClear();
            SetHasDestination(false);
            mHasArrived = true;
         }
      }
   }

   static const float ShapeDefaultHeight = 1.8f;


   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::Initialize()
   {
      if (!mTransformable.valid())
      {
         LOG_ERROR("Invalid transformable on actor.  Civilian AI cannot function, aborting init.");
         return;
      }

      dtAI::AIInterfaceActor* aiInterfaceActor = NULL;
      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);
      if (actor != NULL)
      {
         actor->GetGameManager()->FindActorByType(*dtAI::AIActorRegistry::AI_INTERFACE_ACTOR_TYPE, aiInterfaceActor);
      }

      if(aiInterfaceActor != NULL)
      {
         mAIInterface = aiInterfaceActor->GetAIInterface();
      }
      else
      {
         LOG_ERROR("Unable to find AIInterfaceActor in map.");
      }

      dtCore::Transform xform;
      mTransformable->GetTransform(xform);

      dtCore::RefPtr<dtPhysics::Geometry> charShape;
      dtPhysics::PhysicsActComp* pac = GetOwner()->GetComponent<dtPhysics::PhysicsActComp>();
      if (pac != NULL && pac->GetMainPhysicsObject() != NULL && pac->GetMainPhysicsObject()->GetNumGeometries() > 0)
      {
         charShape = pac->GetMainPhysicsObject()->GetGeometry(0);
      }

      if (!charShape.valid())
      {
         charShape = dtPhysics::Geometry::CreateCapsuleGeometry(xform, ShapeDefaultHeight, 0.2f, 100.0f);
      }
      mCharacterController = new dtPhysics::CharacterController(*charShape);
      mCharacterController->SetStepHeight(mStepHeight);
      mCharacterController->SetSkinWidth(pac->GetMainPhysicsObject()->GetSkinThickness());
      mCharacterController->SetMaxInclineAngle(mMaxIncline);
      mCharacterController->Init();

      PrePhysicsUpdate();
   }

   /////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnModelLoaded(dtAnim::AnimationHelper* animComp)
   {
      InitializeMotionBlendAnimations();
      dtAnim::AnimationHelper* animHelper = NULL;

      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);

      actor->GetComponent(animHelper);

      // Reset the controller so that old controls do not conflict.
      if(mEnablePoseMeshes)
      {

         dtAnim::PoseController* controller = animHelper->GetPoseController();
         if (controller != NULL)
         {
            controller->ClearPoseControls();

            controller->AddPoseControl("Poses_LeftEye", 0);
            controller->AddPoseControl("Poses_RightEye", 0);
            controller->AddPoseControl("Poses_Head", 1, true);
            controller->AddPoseControl("Poses_Torso", 2);
            dtGame::GameManager* gm = actor->GetGameManager();
            if(gm != NULL)
            {
               gm->SetTimer("LookAtTimer", actor, 1.0f, true);
            }
         }
      }

//      TestAppUtils util;
//      if (actor == NULL || ! util.GenerateTangentsForObject(*actor))
//      {
//         LOG_WARNING("Could not generate tangents for CivilianActor: " + GetName());
//      }
   }

   /////////////////////////////////////////////////////////////////
   void SearchAndRegisterAnimationOptions(dtAnim::SequenceMixer& seqMixer, const dtUtil::RefString& name, const std::vector<dtUtil::RefString>& options, dtAnim::BaseModelWrapper& wrapper)
   {
      dtCore::RefPtr<const dtAnim::Animatable> anim = seqMixer.GetRegisteredAnimation(name);
      if (anim != NULL) return;

      for (unsigned i = 0; anim == NULL && i < options.size(); ++i)
      {
         anim = seqMixer.GetRegisteredAnimation(options[i]);
      }

      if (anim != NULL)
      {
         //LOG_ALWAYS("Registering Animation named \"" + anim->GetName() + "\" in place of missing \"" + name + "\"");
         dtCore::RefPtr<dtAnim::Animatable> animClone = anim->Clone(&wrapper).get();
         animClone->SetName(name);

         seqMixer.RegisterAnimation(animClone);
      }
   }

   /////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::SetupWalkRunBlend(dtAnim::AnimationHelper* helper, const dtUtil::RefString& OpName,
         const std::vector<dtUtil::RefString>& nameWalkOptions, const std::string& newWalkAnimName,
         const std::vector<dtUtil::RefString>& nameRunOptions, const std::string& newRunAnimName,
         const std::vector<dtUtil::RefString>& nameStandOptions, const std::string& newStandAnimName,
         float walkSpeed, float runSpeed)
   {
      dtCore::RefPtr<dtAnim::WalkRunBlend> newWRBlend;
      if (GetOwner<dtGame::GameActorProxy>()->IsRemote())
      {
         newWRBlend = new dtAnim::WalkRunBlend(*GetOwner()->GetComponent<dtGame::DeadReckoningActorComponent>());
      }
      else
      {
         newWRBlend = new dtAnim::WalkRunBlend(*GetOwner()->GetComponent<dtGame::DRPublishingActComp>());
      }
      newWRBlend->SetName(OpName);

      dtAnim::SequenceMixer& seqMixer = helper->GetSequenceMixer();
      dtAnim::BaseModelWrapper* wrapper = helper->GetModelWrapper();

      SearchAndRegisterAnimationOptions(seqMixer, newWalkAnimName, nameWalkOptions, *wrapper);
      SearchAndRegisterAnimationOptions(seqMixer, newRunAnimName, nameRunOptions, *wrapper);
      SearchAndRegisterAnimationOptions(seqMixer, newStandAnimName, nameStandOptions, *wrapper);

      const dtAnim::Animatable* stand = seqMixer.GetRegisteredAnimation(newStandAnimName);
      const dtAnim::Animatable* walk = seqMixer.GetRegisteredAnimation(newWalkAnimName);
      const dtAnim::Animatable* run = seqMixer.GetRegisteredAnimation(newRunAnimName);

      if(stand != NULL && walk != NULL && run != NULL)
      {
         newWRBlend->SetAnimations(stand->Clone(wrapper).get(), walk->Clone(wrapper).get(), run->Clone(wrapper).get());
         newWRBlend->Setup(walkSpeed, runSpeed);
         seqMixer.RegisterAnimation(newWRBlend);
      }
      else
      {
         if (stand != NULL && walk != NULL)
         {
            newWRBlend->SetAnimations(stand->Clone(wrapper).get(), walk->Clone(wrapper).get(), NULL);
            newWRBlend->Setup(walkSpeed, runSpeed);
            seqMixer.RegisterAnimation(newWRBlend);
         }
         else if (walk != NULL)
         {
            // Can't do much right now with just a walk.
            dtCore::RefPtr<dtAnim::Animatable> walkClone = walk->Clone(wrapper).get();
            walkClone->SetName(OpName);
            seqMixer.RegisterAnimation(walkClone);
            LOGN_INFO(LOG_NAME, "Cannot load/find a motionless animation for: " + OpName);
         }
         else if (stand != NULL)
         {
            // Can't do much right now with just a stand.
            dtCore::RefPtr<dtAnim::Animatable> standClone = stand->Clone(wrapper).get();
            standClone->SetName(OpName);
            seqMixer.RegisterAnimation(standClone);
            LOGN_INFO(LOG_NAME, "Cannot load/find a walking animation for: " + OpName);
         }
         else
         {
            LOGN_INFO(LOG_NAME, "Cannot load any walk or run animations for: " + OpName);
         }
      }
   }


   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::InitializeMotionBlendAnimations()
   {
      std::vector<dtUtil::RefString> optionsWalk;
      std::vector<dtUtil::RefString> optionsRun;
      std::vector<dtUtil::RefString> optionsStand;

      dtUtil::RefString animationNamesW[4] = { "WalkReady", "Walk", "Walk Deployed", "" };
      dtUtil::RefString animationNamesR[4] = { "RunReady", "Run", "Run Deployed", "" };
      dtUtil::RefString animationNamesIdle[8] = { "Stand Deployed", "StandDeployed", "Stand", "Idle", "Idle_2", "StandReady", "Stand Ready", "" };

      optionsWalk.insert(optionsWalk.end(), &animationNamesW[0], &animationNamesW[3]);
      optionsRun.insert(optionsRun.end(), &animationNamesR[0], &animationNamesR[3]);
      optionsStand.insert(optionsStand.end(), &animationNamesIdle[0], &animationNamesIdle[7]);

      dtAnim::AnimationHelper* animActorComponent = NULL;
      GetOwner()->GetComponent(animActorComponent);

      SetupWalkRunBlend(animActorComponent, dtAnim::AnimationOperators::ANIM_WALK_READY, optionsWalk, "Walk Ready",
            optionsRun, "Run Ready",
            optionsStand, "Stand Ready",
            GetAnimationWalkSpeed(), GetAnimationRunSpeed());

      std::reverse(optionsWalk.begin(), optionsWalk.end());
      std::reverse(optionsRun.begin(), optionsRun.end());
      std::reverse(optionsStand.begin(), optionsStand.end());
      SetupWalkRunBlend(animActorComponent, dtAnim::AnimationOperators::ANIM_WALK_DEPLOYED, optionsWalk, "Walk Deployed",
            optionsRun, "Run Deployed",
            optionsStand, "Stand Deployed",
            GetAnimationWalkSpeed(), GetAnimationRunSpeed());


      dtUtil::RefString animationNamesCrawl[6] = { "Crawl Ready", "CrawlReady", "Crawl", "CrawlDeployed", "Crawl Deployed", "" };
      dtUtil::RefString animationNamesProne[6] = { "Prone Deployed", "ProneDeployed", "Prone", "ProneReady","Prone Ready", "" };

      optionsStand.clear();
      optionsWalk.clear();
      optionsRun.clear();
      optionsWalk.insert(optionsWalk.end(), &animationNamesCrawl[0], &animationNamesCrawl[5]);
      optionsStand.insert(optionsStand.end(), &animationNamesProne[0], &animationNamesProne[5]);

      SetupWalkRunBlend(animActorComponent, dtAnim::AnimationOperators::ANIM_CRAWL_READY, optionsWalk, "Crawl Ready",
            optionsRun, "",
            optionsStand, "Prone Ready",
            GetAnimationCrawlSpeed(), GetAnimationCrawlSpeed());

      std::reverse(optionsWalk.begin(), optionsWalk.end());
      std::reverse(optionsRun.begin(), optionsRun.end());
      std::reverse(optionsStand.begin(), optionsStand.end());
      SetupWalkRunBlend(animActorComponent, dtAnim::AnimationOperators::ANIM_CRAWL_DEPLOYED, optionsWalk, "Crawl Deployed",
            optionsRun, "",
            optionsStand, "Prone Deployed",
            GetAnimationCrawlSpeed(), GetAnimationCrawlSpeed());

      dtUtil::RefString animationNamesLowWalk[6] = { "Low Walk Ready", "LowWalkReady", "Low Walk", "LowWalkDeployed", "Low Walk Deployed", "" };
      dtUtil::RefString animationNamesCrouch[6] = { "Crouch Deployed", "CrouchDeployed", "Crouch", "CrouchReady","Crouch Ready", "" };

      optionsStand.clear();
      optionsWalk.clear();
      optionsRun.clear();
      optionsWalk.insert(optionsWalk.end(), &animationNamesLowWalk[0], &animationNamesLowWalk[5]);
      optionsStand.insert(optionsStand.end(), &animationNamesCrouch[0], &animationNamesCrouch[5]);

      SetupWalkRunBlend(animActorComponent, dtAnim::AnimationOperators::ANIM_LOW_WALK_READY, optionsWalk, "Low Walk Ready",
            optionsRun, "",
            optionsStand, "Kneel Ready",
            GetAnimationLowWalkSpeed(), GetAnimationLowWalkSpeed());

      std::reverse(optionsWalk.begin(), optionsWalk.end());
      std::reverse(optionsRun.begin(), optionsRun.end());
      std::reverse(optionsStand.begin(), optionsStand.end());
      SetupWalkRunBlend(animActorComponent, dtAnim::AnimationOperators::ANIM_LOW_WALK_DEPLOYED, optionsWalk, "Low Walk Deployed",
            optionsRun, "",
            optionsStand, "Kneel Deployed",
            GetAnimationLowWalkSpeed(), GetAnimationLowWalkSpeed());

   }

   ///////////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(CivilianAIActorComponent, float, AnimationWalkSpeed);
   DT_IMPLEMENT_ACCESSOR(CivilianAIActorComponent, float, AnimationRunSpeed);
   DT_IMPLEMENT_ACCESSOR(CivilianAIActorComponent, float, AnimationCrawlSpeed);
   DT_IMPLEMENT_ACCESSOR(CivilianAIActorComponent, float, AnimationLowWalkSpeed);
   DT_IMPLEMENT_ACCESSOR(CivilianAIActorComponent, float, StepHeight);
   DT_IMPLEMENT_ACCESSOR(CivilianAIActorComponent, float, MaxIncline);
   DT_IMPLEMENT_ACCESSOR(CivilianAIActorComponent, float, LookAtCameraRange);


   ///////////////////////////////////////////////////////////////////////////////////
   dtPhysics::CharacterController* CivilianAIActorComponent::GetCharacterController()
   {
      return mCharacterController.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////
   const dtPhysics::CharacterController* CivilianAIActorComponent::GetCharacterController() const
   {
      return mCharacterController.get();
   }


   ///////////////////////////////////////////////////////////////////////////////////
   float CivilianAIActorComponent::GetWalkSpeed() const
   {
      return mWalkSpeed;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::SetWalkSpeed(float mps)
   {
      mWalkSpeed = mps;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   float CivilianAIActorComponent::GetRotationSpeed() const
   {
      return mRotationSpeed;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::SetRotationSpeed(float mps)
   {
      mRotationSpeed = mps;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::RotateToHeading(float headingInRadians, float delta)
   {
      bool success = false;

      float rotation = GetHeading();
      float dR = headingInRadians - rotation;

      if (dR > 180.0f)
      {
         dR -= 360.0f;
      }
      else if (dR < -180.0f)
      {
         dR += 360.0f;
      }

      float change = delta * mRotationSpeed;
      if (dR > 0.0f)
      {
         if(dR > change)
         {
            rotation +=  change;
         }
         else
         {
            rotation += dR;
            success = true;
         }
      }
      else if (dR < 0.0f)
      {
         if(dR < -change)
         {
            rotation += -change;
         }
         else
         {
            rotation += dR;
            success = true;
         }
      }
      else
      {
         success = true;
      }

      SetHeading(rotation);
      return success;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::RotateToPoint(const osg::Vec3& targetPos, float delta)
   {
      osg::Vec3 ownPos = GetPosition();
      osg::Vec3 pVector = targetPos - ownPos;
      pVector[2] = 0.0f;

      pVector.normalize();

      float dir = osg::RadiansToDegrees(atan2(pVector[0], -pVector[1]));
      return RotateToHeading(dir, delta);
   }

   ///////////////////////////////////////////////////////////////////////////////////
   float CivilianAIActorComponent::GetHeading() const
   {
      dtCore::Transform xform;
      mTransformable->GetTransform(xform);
      return xform.GetRotation()[0] + 180.0f;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::SetHeading(float degrees)
   {	  
      dtCore::Transform xform;
      mTransformable->GetTransform(xform);
      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      hpr[0] = degrees - 180.0f;
      //LOG_ALWAYS("Rotation: " + dtUtil::ToString(hpr));
      xform.SetRotation(hpr);
      mTransformable->SetTransform(xform);
   }


   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::IsAtWaypoint(const dtAI::WaypointInterface* pWaypoint)
   {
      // a simple distance comparison to determine if we are within
      // range of a waypoint to be considered "at it"
      osg::Vec3 pos = GetPosition();
      osg::Vec3 wayPos = pWaypoint->GetPosition();

      float distToX = std::abs(pos[0] - wayPos[0]);
      float distToY = std::abs(pos[1] - wayPos[1]);

      if (mWaypointPath.size() == 1)
      {         
         return (distToX < 2.0f && distToY < 2.0f);
      }
      else
      {
         return (distToX < 1.0f && distToY < 1.0f);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::ApplyStringPulling()
   {
      return; // disabled
      while (mWaypointPath.size() > 2)
      {
         const dtAI::WaypointInterface* pNextWaypoint = *(++(mWaypointPath.begin()));
         std::vector<dtPhysics::RayCast::Report> hits;
         dtPhysics::RayCast ray;
         osg::Vec3 pos = GetPosition();
         osg::Vec3 potentialDest = pNextWaypoint->GetPosition();
         // Avoid a skipping a waypoint that is on an incline or decline greater than twice the step height
         // because the given path is more likely to be the best way
         if (dtUtil::Abs(potentialDest.z() - pos.z()) > mCharacterController->GetStepHeight() * 2.0f)
         {
            LOG_ALWAYS("Unit: " + mTransformable->GetName() + ", breaking out early from string pulling due to height change.");
            break;
         }
         // Raycast from a the step height
         pos.z() += mCharacterController->GetStepHeight();
         potentialDest.z() += mCharacterController->GetStepHeight();
         ray.SetOrigin(pos);
         ray.SetDirection(potentialDest - pos);
         ray.SetCollisionGroupFilter(1 << 0); // TODO Terrain typically group 0.  Need to fix this.
         dtPhysics::PhysicsWorld::GetInstance().TraceRay(ray, hits);
         if (hits.empty())
         {
            LOG_ALWAYS("Unit: " + mTransformable->GetName() + ", has clear path to waypoint: " + dtUtil::ToString(pNextWaypoint->GetID()) + " removing waypoint:" + dtUtil::ToString(mWaypointPath.front()->GetID()) + " from path.");
            mWaypointPath.pop_front();
         }
         else
         {
            break;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 CivilianAIActorComponent::GetPosition()
   {
      if (mCharacterController.valid())
      {
         return mCharacterController->GetTranslation();
      }
      return osg::Vec3();
   }

   ///////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 CivilianAIActorComponent::GetForward()
   {
      dtCore::Transform xform;
      mTransformable->GetTransform(xform);
      return xform.GetForwardVector();
   }


   ///////////////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::MoveToPoint(float dt, const dtAI::WaypointInterface* pWaypoint)
   {
      if(!mIgnoreRotation)
      {
         RotateToPoint(pWaypoint->GetPosition(), dt);
      }

      //LOGN_ALWAYS(LOG_NAME, "Walking to/from: " + dtUtil::ToString(pWaypoint->GetID()) + " " + dtUtil::ToString(pWaypoint->GetPosition()) + " " + dtUtil::ToString(GetPosition()));

      osg::Vec3 dir = pWaypoint->GetPosition() - GetPosition();
      dir[2] = 0.0f;

      if (dir.normalize() > FLT_EPSILON && mWalkSpeed > FLT_EPSILON)
      {
         osg::Vec3 vel =  dir * mWalkSpeed;
         //LOGN_ALWAYS("civilianaiactorcomponent.cpp", "Walk Vector: " + dtUtil::ToString(vel));
         mCharacterController->Walk(vel, 1.0f);
      }
      else
      {
         mCharacterController->WalkClear();
      }
   }


   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::SetDestByWaypointName(const std::string& waypointName)
   {
      bool success = false;

      dtAI::WaypointInterface* result = mAIInterface->GetWaypointByName(waypointName);
      if(result != NULL)
      {
         success = SetDestByWaypoint(result);
      }

      return success;
   }


   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::SetDestByClosestNamedWaypoint(const std::string& waypointName, const osg::Vec3& pos)
   {
      bool success = false;
      //LOG_ALWAYS("Get Closest Name Waypoint with name '" + waypointName);
      dtAI::WaypointInterface* result = mAIInterface->GetClosestNamedWaypoint(waypointName, pos, 2500.0f);
      if(result != NULL)
      {
         success = SetDestByWaypoint(result);
      }

      return success;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::SetDestByClosestWaypointWithType(const dtCore::ObjectType& type, const osg::Vec3& pos)
   {
      bool success = false;

      //UNIMPLEMENTED!!!

      //WaypointInterface* result = mAIInterface->GetWaypointByName(waypointName);
      //if(result != NULL)
      //{
      // success = SetDestByWaypoint(result);
      //}

      return success;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::SetDestByPosition(const osg::Vec3& dest)
   {
      bool success = false;

      dtAI::WaypointInterface* result = mAIInterface->GetClosestWaypoint(dest, 30.0f);
      if(result != NULL)
      {
         success = SetDestByWaypoint(result);
      }

      if(success)
      {
         mWaypointPath.push_back(&mDestinationWaypoint);
      }

      return success;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool CivilianAIActorComponent::SetDestByWaypoint(const dtAI::WaypointInterface* pWaypoint)
   {	  
      dtAI::PathFindResult hasPath = dtAI::NO_PATH; 

      if(mCurrentWaypoint == NULL)
      {
         //try to find one
         osg::Vec3 pos = GetPosition();
         mCurrentWaypoint = mAIInterface->GetClosestWaypoint(pos, 250.0f);

         //dtAI::AIPluginInterface::WaypointArray arrayToFill;
         //const float range = 30.0f;
         //mAIInterface->GetWaypointsAtRadius(pos, range, arrayToFill);

         //float bestDist2 = range + 1.0f;
         //std::vector<dtPhysics::RayCast::Report> hits;

         //// Get the position off the ground to get it a clear eyeshot.
         //pos.z() += mCharacterController->GetStepHeight();
         //// square it because I'll check dist squared.
         //bestDist2 *= bestDist2;
         //dtAI::AIPluginInterface::WaypointArray::iterator iter = arrayToFill.begin();
         //dtAI::AIPluginInterface::WaypointArray::iterator iterEnd = arrayToFill.end();
         //for(;iter != iterEnd; ++iter)
         //{
         //   dtAI::WaypointInterface* waypoint = *iter;
         //   if(waypoint != NULL)
         //   {
         //      //do ray cast for visibility
         //      osg::Vec3 wayPos = waypoint->GetPosition();
         //      // Get the position off the ground to get it a clear eyeshot.
         //      wayPos.z() += mCharacterController->GetStepHeight();
         //      osg::Vec3 direction = wayPos - pos;
         //      float dist2 = direction.length2();
         //      if (dist2 < bestDist2)
         //      {
         //         dtPhysics::RayCast ray;
         //         ray.SetOrigin(pos);
         //         ray.SetDirection(direction);
         //         ray.SetCollisionGroupFilter(1 << SimCore::CollisionGroup::GROUP_TERRAIN);
         //         hits.clear();
         //         dtPhysics::PhysicsWorld::GetInstance().TraceRay(ray, hits);
         //         if (hits.empty())
         //         {
         //            mCurrentWaypoint = waypoint;
         //            bestDist2 = dist2;
         //         }
         //      }
         //   }
         //}
      }

      if(mCurrentWaypoint != NULL)
      {
         dtAI::AIPluginInterface::ConstWaypointArray waypointArray;
         hasPath = mAIInterface->FindPath(mCurrentWaypoint->GetID(), pWaypoint->GetID(), waypointArray);

         if(hasPath == dtAI::PATH_FOUND)
         {
            mWaypointPath.clear();
            mWaypointPath.assign(waypointArray.begin(), waypointArray.end());

            mWaypointPath.push_front(mCurrentWaypoint);

            SetHasDestination(true);
            mHasArrived = false;
         }
      }
      else
      {
         LOG_ERROR("Actor: " + GetName() + " is unable to find a current waypoint.");
      }

      return hasPath != dtAI::NO_PATH;
   }


   void CivilianAIActorComponent::OnAnimationsTransitioning(dtAnim::AnimationTransitionPlanner& planner)
   {
      dtAnim::BasicStanceEnum& stance = planner.GetStance();

      if(stance == dtAnim::BasicStanceEnum::PRONE)
      {
         SetWalkSpeed(GetAnimationCrawlSpeed());
      }
      else if (stance == dtAnim::BasicStanceEnum::KNEELING)
      {
         SetWalkSpeed(GetAnimationLowWalkSpeed());
      }
      else if(stance == dtAnim::BasicStanceEnum::STANDING)
      {
         SetWalkSpeed(GetAnimationWalkSpeed());
      }
   }

   void CivilianAIActorComponent::SetHasDestination(bool hasDestination)
   {
      mHasDestination = hasDestination;
   }

   bool CivilianAIActorComponent::GetHasDestination() const
   {
      return mHasDestination;
   }

}


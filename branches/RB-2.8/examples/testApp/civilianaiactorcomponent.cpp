#include "civilianaiactorcomponent.h"
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

#include <dtAnim/animationtransitionplanner.h>
#include <dtAnim/walkrunblend.h>
#include <dtAnim/animationhelper.h>
#include <dtGame/deadreckoninghelper.h>
#include <dtGame/drpublishingactcomp.h>

namespace dtExample
{

   const dtGame::ActorComponent::ACType CivilianAIActorComponent::TYPE( new dtCore::ActorType("CivilianAI", "ActorComponents", "", dtGame::ActorComponent::BaseActorComponentType));

   ////////////////////////////////////////////////////////////////////////
   CivilianAIActorComponent::CivilianAIActorComponent(const ACType& type)
   : dtGame::ActorComponent(type)
   , mAnimationWalkSpeed(1.5f)
   , mAnimationRunSpeed(3.0f)
   , mAnimationCrawlSpeed(0.2f)
   , mAnimationLowWalkSpeed(0.4f)
   , mEntityIndex(0)
   , mIgnoreRotation(false)
   , mHasDestination(false)
   , mHasArrived(false)
   , mWalkSpeed(2.75f)
   , mRotationSpeed(1.0f)
   , mHPR()
   , mAIInterface(NULL)
   , mCharacterController(NULL)
   , mDestinationWaypoint()
   , mCurrentWaypoint(NULL)
   , mWaypointPath()
   {
      SetClassName("dtExample.CivilianAIActorComponent");
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
      DT_REGISTER_PROPERTY(AnimationWalkSpeed, "The inherent speed of the walk animation.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AnimationRunSpeed, "The inherent speed of the run animation.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AnimationCrawlSpeed, "The inherent speed of the crawl animation.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AnimationLowWalkSpeed, "The inherent speed of the low walk animation.", RegHelperType, propReg);
   }

   ////////////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();
      RegisterForTick();
      dtAnim::AnimationTransitionPlanner* planner = GetOwner()->GetComponent<dtAnim::AnimationTransitionPlanner>();
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
      if (!GetOwner()->HasComponent(dtGame::DeadReckoningHelper::TYPE))
      {
         GetOwner()->AddComponent(* new dtGame::DeadReckoningHelper);
      }
      if (!GetOwner()->HasComponent(dtGame::DRPublishingActComp::TYPE))
      {
         GetOwner()->AddComponent(* new dtGame::DRPublishingActComp);
      }

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
   void CivilianAIActorComponent::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      PerformMove(tickMessage.GetDeltaSimTime());

      if( mTransformable.valid() && mCharacterController.valid())
      {
         dtCore::Transform xform;
         mTransformable->GetTransform(xform);

         osg::Vec3 newPos = mCharacterController->GetTranslation();

         //is this required?
         newPos[2] -= 0.9;

         xform.SetTranslation(newPos);

         mTransformable->SetTransform(xform);
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

      osg::Vec3 offset = xform.GetTranslation();
      offset[2] += 2.5f; //so the character is centered at its origin
      xform.SetTranslation(offset);

      dtCore::RefPtr<dtPhysics::Geometry> charShape = dtPhysics::Geometry::CreateCapsuleGeometry(xform, 1.8f, 0.2f, 100.0f);
      mCharacterController = new dtPhysics::CharacterController(*charShape);
      mCharacterController->SetStepHeight(0.5f);
      mCharacterController->SetSkinWidth(0.04f);
      mCharacterController->SetMaxInclineAngle(70.0f);
      mCharacterController->Init();

      mCharacterController->Warp(offset);

   }

   /////////////////////////////////////////////////////////////////
   void CivilianAIActorComponent::OnModelLoaded(dtAnim::AnimationHelper* animComp)
   {
      InitializeMotionBlendAnimations();
   }

   /////////////////////////////////////////////////////////////////
   void SearchAndRegisterAnimationOptions(dtAnim::SequenceMixer& seqMixer, const dtUtil::RefString& name, const std::vector<dtUtil::RefString>& options, dtAnim::Cal3DModelWrapper& wrapper)
   {
      dtCore::RefPtr<const dtAnim::Animatable> anim = seqMixer.GetRegisteredAnimation(name);
      if (anim != NULL) return;

      for (unsigned i = 0; anim == NULL && i < options.size(); ++i)
      {
         anim = seqMixer.GetRegisteredAnimation(options[i]);
      }

      if (anim != NULL)
      {
         LOG_ALWAYS("Registering Animation named \"" + anim->GetName() + "\" in place of missing \"" + name + "\"");
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
         newWRBlend = new dtAnim::WalkRunBlend(*GetOwner()->GetComponent<dtGame::DeadReckoningHelper>());
      }
      else
      {
         newWRBlend = new dtAnim::WalkRunBlend(*GetOwner()->GetComponent<dtGame::DRPublishingActComp>());
      }
      newWRBlend->SetName(OpName);

      dtAnim::SequenceMixer& seqMixer = helper->GetSequenceMixer();
      dtAnim::Cal3DModelWrapper* wrapper = helper->GetModelWrapper();

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
            LOGN_WARNING("Human.cpp", "Cannot load/find a motionless animation for: " + OpName);
         }
         else if (stand != NULL)
         {
            // Can't do much right now with just a stand.
            dtCore::RefPtr<dtAnim::Animatable> standClone = stand->Clone(wrapper).get();
            standClone->SetName(OpName);
            seqMixer.RegisterAnimation(standClone);
            LOGN_WARNING("Human.cpp", "Cannot load/find a walking animation for: " + OpName);
         }
         else
         {
            LOGN_WARNING("Human.cpp", "Cannot load any walk or run animations for: " + OpName);
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

      if (dR > 0.0f)
      {
         if(dR > delta*90.0f)
         {
            rotation +=  delta*90.0f;
         }
         else
         {
            rotation += dR;
            success = true;
         }
      }
      else if (dR < 0.0f)
      {
         if(dR < -delta*90.0f)
         {
            rotation += -delta*90.0f;
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
      //temporarily commented out...
      return;

      if (mWaypointPath.size() < 2) return;

      do
      {

         const dtAI::WaypointInterface* pNextWaypoint = *(++(mWaypointPath.begin()));
         std::vector<dtPhysics::RayCast::Report> hits;
         dtPhysics::RayCast ray;
         osg::Vec3 pos = GetPosition();
         osg::Vec3 potentialDest = pNextWaypoint->GetPosition();
         pos.z() += mCharacterController->GetStepHeight();
         potentialDest.z() += mCharacterController->GetStepHeight();
         ray.SetOrigin(pos);
         ray.SetDirection(potentialDest - pos);
         ray.SetCollisionGroupFilter(1 << 0); // TODO Terrain typically group 0.  Need to fix this.
         dtPhysics::PhysicsWorld::GetInstance().TraceRay(ray, hits);
         if (hits.empty())
         {
            if(pNextWaypoint->GetID() == 24)
            {
               LOG_ALWAYS("Unit: " + GetName() + ", has clear path to waypoint: " + dtUtil::ToString(pNextWaypoint->GetID()) + " removing waypoint:" + dtUtil::ToString(mWaypointPath.front()->GetID()) + " from path.");
            }
            mWaypointPath.pop_front();
         }
         else
         {
            break;
         }
      }
      while (mWaypointPath.size() > 2);
   }

   ///////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 CivilianAIActorComponent::GetPosition()
   {
      dtCore::Transform xform;
      mTransformable->GetTransform(xform);
      return xform.GetTranslation();
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

      osg::Vec3 dir = pWaypoint->GetPosition() - GetPosition();
      dir[2] = 0.0f;
      dir.normalize();

      osg::Vec3 vel =  dir * /*GetHumanDrawable()->GetWalkAnimationSpeed();*/ mWalkSpeed;
      mCharacterController->Walk(vel, 1.0f);
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
      //wLOG_ALWAYS("Get Closest Name Waypoint with name '" + waypointName);
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
         SetWalkSpeed(1.0f);
      }
      else if (stance == dtAnim::BasicStanceEnum::KNEELING)
      {
         SetWalkSpeed(1.5f);
      }
      else if(stance == dtAnim::BasicStanceEnum::STANDING)
      {
         SetWalkSpeed(2.0f);
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


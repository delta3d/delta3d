#include "civilianaiactorcomponent.h"
#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtGame/invokable.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtCore/propertymacros.h>
#include <dtAI/waypointinterface.h>
#include <dtPhysics/raycast.h>
#include <dtPhysics/palphysicsworld.h>

#include <dtAnim/animationtransitionplanner.h>

namespace dtExample
{

   const dtGame::ActorComponent::ACType CivilianAIActorComponent::TYPE( new dtCore::ActorType("CivilianAI", "ActorComponents", "", dtGame::ActorComponent::BaseActorComponentType));

   ////////////////////////////////////////////////////////////////////////
   CivilianAIActorComponent::CivilianAIActorComponent(const ACType& type)
   : dtGame::ActorComponent(type)
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
      SetClassName("Blacklight.CivilianAIActorComponent");
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

      if( mTransformable.valid())
      {
         dtCore::Transform xform;
         mTransformable->GetTransform(xform);

         osg::Vec3 newPos = mCharacterController->GetTranslation();

         //is this required?
         newPos[2] -= 2.5f;

         xform.SetTranslation(newPos);

         mTransformable->SetTransform(xform);
      }

   }

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
   void CivilianAIActorComponent::Initialize(dtAI::AIPluginInterface& aiInterface)
   {
      mAIInterface = &aiInterface;

      if (!mTransformable.valid())
      {
         LOG_ERROR("Invalid transformable on actor.");
         return;
      }

      dtCore::Transform xform;
      mTransformable->GetTransform(xform);

      osg::Vec3 offset = xform.GetTranslation();
      offset[2] += 2.5f; //so the character is centered at its origin
      xform.SetTranslation(offset);

      dtCore::RefPtr<dtPhysics::Geometry> charShape = dtPhysics::Geometry::CreateCapsuleGeometry(xform, 5.0f, 0.7f, 100.0f);
      mCharacterController = new dtPhysics::CharacterController(*charShape);
      mCharacterController->SetStepHeight(0.5f);
      mCharacterController->SetSkinWidth(0.04f);
      mCharacterController->SetMaxInclineAngle(70.0f);
      mCharacterController->Init();

      mCharacterController->Warp(offset);

   }

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


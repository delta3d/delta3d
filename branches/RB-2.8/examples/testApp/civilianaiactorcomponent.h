#ifndef CivilianActor_H_
#define CivilianActor_H_

#include "export.h"

#include <dtCore/observerptr.h>

#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/waypointinterface.h>
#include <dtAI/waypoint.h>

#include <dtCore/propertymacros.h>

#include <dtGame/gameactorproxy.h>
#include <dtGame/basemessages.h>

#include <dtPhysics/charactercontroller.h>

namespace dtAnim
{
   class BasicStanceEnum;
   class AnimationTransitionPlanner;
}

namespace dtExample
{

   /////////////////////////////////////////////////////////
   class TEST_APP_EXPORT CivilianAIActorComponent : public dtGame::ActorComponent
   {
   public:
      typedef dtGame::ActorComponent BaseClass;

      static const ACType TYPE;

      CivilianAIActorComponent(const ACType& type = TYPE);

      virtual void Initialize();

      /*override*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);

      /**
       * Build the properties common to all platform objects
       */
      virtual void BuildPropertyMap();

      virtual void OnEnteredWorld();

      virtual bool ShouldMove() const;

      float GetWalkSpeed() const;
      void SetWalkSpeed(float metersPerSecond);

      float GetRotationSpeed() const;
      void SetRotationSpeed(float degreesPerSecond);

      float GetHeading() const;
      void SetHeading(float degrees);

      osg::Vec3 GetPosition();
      osg::Vec3 GetForward();

      bool HasDestination() const;
      bool IsAtDestination() const;

      bool IsAtWaypoint(const dtAI::WaypointInterface* pWaypoint);

      bool RotateToHeading(float headingInDegrees, float dt);
      bool RotateToPoint(const osg::Vec3& point, float dt);

      void MoveToPoint(float dt, const dtAI::WaypointInterface* pWaypoint);


      void SetHasDestination(bool hasDestination);
      bool GetHasDestination() const;

      bool SetDestByPosition(const osg::Vec3& pos);
      bool SetDestByWaypoint(const dtAI::WaypointInterface* pWaypoint);
      bool SetDestByWaypointName(const std::string& waypointName);
      bool SetDestByClosestNamedWaypoint(const std::string& waypointName, const osg::Vec3& pos);
      bool SetDestByClosestWaypointWithType(const dtCore::ObjectType& type, const osg::Vec3& pos);

      //DT_DECLARE_ACCESSOR_INLINE(std::string, ActorPrototypeName)
      //DT_DECLARE_ACCESSOR_INLINE(std::string, DirectorPrototypeName)
      DT_DECLARE_ACCESSOR_INLINE(unsigned, EntityIndex)
      DT_DECLARE_ACCESSOR_INLINE(bool, IgnoreRotation)

      dtPhysics::CharacterController* GetCharacterController();
      const dtPhysics::CharacterController* GetCharacterController() const;

      /*override*/ void OnAddedToActor(dtCore::BaseActorObject& /*actor*/);

      /*override*/ void OnRemovedFromActor(dtCore::BaseActorObject& /*actor*/);


   protected:
      virtual ~CivilianAIActorComponent();

      //private: temporarily commented out to aid refactor for derived marine

      void ApplyStringPulling();
      void PerformMove(float dt);

      void OnAnimationsTransitioning(dtAnim::AnimationTransitionPlanner& planner);

      bool mHasDestination;
      bool mHasArrived;

      float mWalkSpeed;
      float mRotationSpeed;

      osg::Vec3 mHPR;

      dtCore::ObserverPtr<dtAI::AIPluginInterface> mAIInterface;
      dtCore::RefPtr<dtPhysics::CharacterController> mCharacterController;
      dtCore::ObserverPtr<dtCore::Transformable> mTransformable;


      dtAI::Waypoint mDestinationWaypoint;
      const dtAI::WaypointInterface* mCurrentWaypoint;
      typedef std::list<const dtAI::WaypointInterface*> WaypointList;
      WaypointList mWaypointPath;

   };

}

#endif /* PROPELLEDVEHICLEACTOR_H_ */

/*
 * stanceplanner.h
 *
 *  Created on: Aug 24, 2014
 *      Author: david
 */

#ifndef STANCEPLANNER_H_
#define STANCEPLANNER_H_

#include <dtAnim/export.h>
#include <dtAnim/animatable.h>

#include <dtAI/statevariable.h>
#include <dtAI/operator.h>
#include <dtAI/planner.h>
#include <dtAI/plannerhelper.h>

#include <dtUtil/enumeration.h>
#include <dtUtil/refstring.h>
#include <dtUtil/getsetmacros.h>
#include <dtGame/actorcomponent.h>

#include <dtCore/sigslot.h>

namespace dtAI
{
   class WorldState;
   class Operator;
}

namespace dtAnim
{
   class AnimationHelper;
   class HumanOperator;

   class DT_ANIM_EXPORT BasicStanceEnum: public dtUtil::Enumeration
   {
      DECLARE_ENUM(BasicStanceEnum);
   public:
      static BasicStanceEnum IDLE;
      static BasicStanceEnum STANDING;
      static BasicStanceEnum SITTING;
      static BasicStanceEnum LYING;
      static BasicStanceEnum KNEELING;
      static BasicStanceEnum PRONE;

      // Used to help the planner determine distance.
      float GetCostValue() const;
   private:
      BasicStanceEnum(const std::string& name, float costValue);
      float mCostValue;
   };

   class DT_ANIM_EXPORT WeaponStateEnum: public dtUtil::Enumeration
   {
      DECLARE_ENUM(WeaponStateEnum)
               ;
   public:
      static WeaponStateEnum NO_WEAPON;
      static WeaponStateEnum STOWED;
      static WeaponStateEnum DEPLOYED;
      static WeaponStateEnum FIRING_POSITION;
   private:
      WeaponStateEnum(const std::string& name);
   };

   class DT_ANIM_EXPORT AnimationOperators
   {
   public:
      typedef std::map<dtUtil::RefString, dtAI::Operator*> NameOperMap;

      static const dtUtil::RefString ANIM_WALK_READY;
      static const dtUtil::RefString ANIM_WALK_DEPLOYED;
      static const dtUtil::RefString ANIM_LOW_WALK_READY;
      static const dtUtil::RefString ANIM_LOW_WALK_DEPLOYED;
      static const dtUtil::RefString ANIM_CRAWL_READY;
      static const dtUtil::RefString ANIM_CRAWL_DEPLOYED;
      static const dtUtil::RefString ANIM_SITTING_DEPLOYED;
      static const dtUtil::RefString ANIM_SITTING_READY;
      static const dtUtil::RefString ANIM_LYING_DEPLOYED;
      static const dtUtil::RefString ANIM_LYING_READY;
      static const dtUtil::RefString ANIM_STAND_TO_KNEEL;
      static const dtUtil::RefString ANIM_KNEEL_TO_STAND;
      static const dtUtil::RefString ANIM_STAND_TO_SIT;
      static const dtUtil::RefString ANIM_SIT_TO_STAND;
      static const dtUtil::RefString ANIM_SIT_TO_LIE;
      static const dtUtil::RefString ANIM_LIE_TO_SIT;
      static const dtUtil::RefString ANIM_PRONE_TO_KNEEL;
      static const dtUtil::RefString ANIM_KNEEL_TO_PRONE;
      static const dtUtil::RefString ANIM_DYING_STANDING;
      static const dtUtil::RefString ANIM_DYING_KNEELING;
      static const dtUtil::RefString ANIM_DYING_PRONE;
      static const dtUtil::RefString ANIM_DEAD_STANDING;
      static const dtUtil::RefString ANIM_DEAD_KNEELING;
      static const dtUtil::RefString ANIM_DEAD_PRONE;
      static const dtUtil::RefString ANIM_STANDING_ACTION;
      static const dtUtil::RefString ANIM_KNEELING_ACTION;
      static const dtUtil::RefString ANIM_PRONE_ACTION;
      static const dtUtil::RefString ANIM_LYING_ACTION;
      static const dtUtil::RefString ANIM_SITTING_ACTION;
      static const dtUtil::RefString OPER_DEPLOYED_TO_READY;
      static const dtUtil::RefString OPER_READY_TO_DEPLOYED;

      AnimationOperators(dtAI::PlannerHelper& plannerHelper);
      ~AnimationOperators();

   private:
      void CreateOperators();
      HumanOperator* AddOperator(const std::string& name);

      dtAI::PlannerHelper& mPlannerHelper;
      NameOperMap mOperators;
   };


   class DT_ANIM_EXPORT AnimationTransitionPlanner : public dtGame::ActorComponent
   {
   public:

      typedef dtGame::ActorComponent BaseClass;

      sigslot::signal1<AnimationTransitionPlanner&> SignalAnimationsTransitioning;

      /// All derived actor component types must pass this or a descendant as a parent type.
      static const dtGame::ActorComponent::ACType TYPE;


      /// The basic stance of the character, such as standing or kneeling
      static const dtUtil::RefString STATE_BASIC_STANCE;
      /// The state of the weapon such as non-existent, deployed, etc.
      static const dtUtil::RefString STATE_WEAPON;
      /// A flag for if the human is dead.
      static const dtUtil::RefString STATE_DEAD;
      /// A flag marking that the human is in a transition,  this makes sure it never ends in a transition.
      static const dtUtil::RefString STATE_TRANSITION;
      /// The number of completed actions while standing.  This counter increments every time an action should be completed.
      static const dtUtil::RefString STATE_STANDING_ACTION_COUNT;
      /// The number of completed actions while kneeling.  This counter increments every time an action should be completed.
      static const dtUtil::RefString STATE_KNEELING_ACTION_COUNT;
      /// The number of completed actions while sitting.  This counter increments every time an action should be completed.
      static const dtUtil::RefString STATE_SITTING_ACTION_COUNT;
      /// The number of completed actions while lying.  This counter increments every time an action should be completed.
      static const dtUtil::RefString STATE_LYING_ACTION_COUNT;
      /// The number of completed actions while kneeling.  This counter increments every time an action should be completed.
      static const dtUtil::RefString STATE_PRONE_ACTION_COUNT;
      /// flag marking that the person has been killed.
      static const dtUtil::RefString STATE_DYING;

      AnimationTransitionPlanner();

      /*override*/ void OnEnteredWorld();

      void Setup();

      const dtAnim::Animatable* ApplyOperatorAndGetAnimatable(const dtAI::Operator& op);

      void ExecuteAction(const dtUtil::RefString& animatableName, BasicStanceEnum& basicStance = BasicStanceEnum::STANDING);

      float GetRemainingCost(const dtAI::WorldState* pWS) const;
      bool IsDesiredState(const dtAI::WorldState* pWS) const;

      /// Checks the desired state to see if a new plan need to be generated, and if so generates it.
      void CheckAndUpdateAnimationState();

      /// Actually runs the planner update.
      void UpdatePlanAndAnimations();
      /// When the state is updated, this is called internally to update the plan.
      bool GenerateNewAnimationSequence();

      /// This exists for the sake of the unit tests.
      const dtAI::Planner::OperatorList& GetCurrentPlan();

      // Setting this will cause the animated character to play a death sequence.
      DT_DECLARE_ACCESSOR(bool, IsDead);
      DT_DECLARE_ACCESSOR(dtUtil::EnumerationPointer<BasicStanceEnum>, Stance);
      DT_DECLARE_ACCESSOR(dtUtil::EnumerationPointer<WeaponStateEnum>, WeaponState);
      /**
       * Sets the maximum amount of time per iteration the planner
       * can use to try and generate an animation sequence
       * @param time The new time to use
       */
      DT_DECLARE_ACCESSOR(double, MaxTimePerIteration);

      /*override*/ void BuildPropertyMap();

      virtual void OnAddedToActor(dtCore::BaseActorObject& actor);

      virtual void OnRemovedFromActor(dtCore::BaseActorObject& actor);

   protected:

      /*override*/ ~AnimationTransitionPlanner();

      unsigned GetExecutedActionCount(BasicStanceEnum& stance) const;

      void OnModelLoaded(AnimationHelper*);
      void OnModelUnloaded(AnimationHelper*);

      unsigned CheckActionState(const dtAI::WorldState* pWS, const std::string& stateName, unsigned desiredVal) const;

      /*override*/ void OnTickLocal(const dtGame::TickMessage& /*tickMessage*/);
      /*override*/ void OnTickRemote(const dtGame::TickMessage& /*tickMessage*/);

   private:

      // TEMP:
      void Update();

      // TEMP:
      void SetupAnimationHelper();

      dtAI::PlannerHelper mPlannerHelper;
      dtAI::Planner mPlanner;
      dtAI::Planner::OperatorList mCurrentPlan;
      dtAnim::AnimationOperators mAnimOperators;

      typedef std::map<BasicStanceEnum*, unsigned> ExecuteActionCountMap;
      ExecuteActionCountMap mExecutedActionCounts;
      std::string mSequenceId;
      bool mResetNextTick;

      // TEMP:
      bool mIsAnimHelperSetup;
   };


   class DT_ANIM_EXPORT BasicStanceState: public dtAI::IStateVariable
   {
   public:
      typedef BasicStanceEnum EnumValueType;

      BasicStanceState();
      ~BasicStanceState();

      dtAI::IStateVariable* Copy() const;

      BasicStanceEnum& GetValue() const;
      void SetValue(BasicStanceEnum& pStance);

      BasicStanceEnum& GetStance() const;
      void SetStance(BasicStanceEnum& newStance);

      /*override*/ const std::string ToString() const;

   private:
      BasicStanceEnum* mStance;
   };

   class DT_ANIM_EXPORT WeaponState: public dtAI::IStateVariable
   {
   public:
      typedef WeaponStateEnum EnumValueType;

      WeaponState();
      ~WeaponState();

      dtAI::IStateVariable* Copy() const;

      WeaponStateEnum& GetValue() const;

      void SetValue(WeaponStateEnum& pWeaponState);

      WeaponStateEnum& GetWeaponStateEnum() const;
      void SetWeaponStateEnum(WeaponStateEnum& newWeaponStateEnum);

      virtual const std::string ToString() const;

   private:
      WeaponStateEnum* mWeaponStateEnum;
   };

}


#endif /* STANCEPLANNER_H_ */

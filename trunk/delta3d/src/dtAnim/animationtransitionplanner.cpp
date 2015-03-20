/*
 * animationtransitionplanner.cpp
 *
 *  Created on: Aug 24, 2014
 *      Author: david
 */

#include <dtAnim/animationtransitionplanner.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/sequencemixer.h>
#include <dtGame/actorcomponentcontainer.h>
#include <dtAI/worldstate.h>
#include <dtAI/basenpcutils.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <dtCore/propertymacros.h>

#include <deque>

namespace dtAnim
{
   class HumanOperator;

   IMPLEMENT_ENUM(WeaponStateEnum);
   WeaponStateEnum WeaponStateEnum::NO_WEAPON("NO_WEAPON");
   WeaponStateEnum WeaponStateEnum::STOWED("STOWED");
   WeaponStateEnum WeaponStateEnum::DEPLOYED("DEPLOYED");
   WeaponStateEnum WeaponStateEnum::FIRING_POSITION("FIRING_POSITION");

   WeaponStateEnum::WeaponStateEnum(const std::string& name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   template <typename StateVarType>
   class EnumerationConditional: public dtAI::IConditional
   {
      public:
         typedef typename StateVarType::EnumValueType StateVarEnumType;

         EnumerationConditional(const dtUtil::RefString& pName, StateVarEnumType& pData): mName(pName), mData(pData) {}
         ~EnumerationConditional() {}

         /*virtual*/ const std::string& GetName() const
         {
            return mName;
         }

         /*virtual*/ bool Evaluate(const dtAI::WorldState* pWS)
         {
            const StateVarType* pStateVar;
            pWS->GetState(mName, pStateVar);
            if(pStateVar != NULL)
            {
               return pStateVar->GetValue() == mData;
            }
            return false;
         }

      private:
         dtUtil::RefString mName;
         StateVarEnumType& mData;
   };

   template <typename StateVarType>
   class EnumeratedEffect: public dtAI::IEffect
   {
   public:
      typedef typename StateVarType::EnumValueType StateVarEnumType;

      EnumeratedEffect(const dtUtil::RefString& pName, StateVarEnumType& pData): mName(pName), mData(pData){}

      const std::string& GetName() const
      {
         return mName;
      }

      bool Apply(const dtAI::Operator*, dtAI::WorldState* pWSIn) const
      {
         StateVarType* pStateVar;
         pWSIn->GetState(mName, pStateVar);
         if (pStateVar != NULL)
         {
            pStateVar->SetValue(mData);
         }
         return true;
      }

   protected:
      ~EnumeratedEffect(){}

   private:
      const dtUtil::RefString mName;
      StateVarEnumType& mData;
   };

   /**
    * Increments a numeric state variable.
    */
   template <typename StateVarType>
   class IncrementEffect: public dtAI::IEffect
   {
   public:
      IncrementEffect(const dtUtil::RefString& pName): mName(pName){}

      const std::string& GetName() const
      {
         return mName;
      }

      bool Apply(const dtAI::Operator*, dtAI::WorldState* pWSIn) const
      {
         StateVarType* pStateVar;
         pWSIn->GetState(mName, pStateVar);
         if(pStateVar != NULL)
         {
            pStateVar->Set(pStateVar->Get() + 1);
         }
         return true;
      }

   protected:
      ~IncrementEffect(){}

   private:
      const dtUtil::RefString mName;
   };

   class HumanOperator: public dtAI::Operator
   {
      public:
         typedef dtAI::IEffect EffectType;
         typedef std::vector<dtCore::RefPtr<EffectType> > EffectList;

         typedef EnumerationConditional<BasicStanceState> BasicStanceEnumConditional;
         typedef EnumerationConditional<WeaponState> WeaponStateEnumConditional;

         typedef EnumeratedEffect<BasicStanceState> BasicStanceEnumEffect;
         typedef EnumeratedEffect<WeaponState> WeaponStateEnumEffect;

         typedef IncrementEffect<dtAI::StateVar<unsigned> > UnsignedIntIncrementEffect;

      public:
         HumanOperator(const dtUtil::RefString& pName);

         void SetCost(float pcost);

         void AddEffect(EffectType* pEffect);

         void EnqueueReplacementAnim(const dtUtil::RefString& animName) const;

         bool GetNextReplacementAnim(dtUtil::RefString& animName, bool dequeue = true) const;

         bool Apply(const dtAI::Operator* oper, dtAI::WorldState* pWSIn) const;

      private:

         float mCost;
         EffectList mEffects;
         mutable std::deque<dtUtil::RefString> mReplacementQueue;
   };

   ////////////////////////////////////////////////////////////////////////////

   const dtUtil::RefString AnimationTransitionPlanner::STATE_BASIC_STANCE("BasicStanceState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_WEAPON("WeaponState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_DEAD("DeadState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_TRANSITION("TranstionState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_STANDING_ACTION_COUNT("StandingActionCountState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_KNEELING_ACTION_COUNT("KneelingActionCountState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_SITTING_ACTION_COUNT("SittingActionCountState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_LYING_ACTION_COUNT("LyingActionCountState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_PRONE_ACTION_COUNT("ProneActionCountState");
   const dtUtil::RefString AnimationTransitionPlanner::STATE_DYING("ShotState");

   /////////////////////////////////////////////////////////////////////////////
   AnimationTransitionPlanner::AnimationTransitionPlanner()
   : ActorComponent(TYPE)
   , mIsDead(false)
   , mStance(&BasicStanceEnum::STANDING)
   , mWeaponState(&dtAnim::WeaponStateEnum::STOWED)
   , mMaxTimePerIteration(0.5)
   , mPlannerHelper(
         dtAI::PlannerHelper::RemainingCostFunctor(this, &AnimationTransitionPlanner::GetRemainingCost),
         dtAI::PlannerHelper::DesiredStateFunctor(this, &AnimationTransitionPlanner::IsDesiredState)
         )
   , mAnimOperators(mPlannerHelper)
   , mResetNextTick(false)
   , mIsAnimHelperSetup(false)
   {
      std::vector<dtAnim::BasicStanceEnum*> basicStances = dtAnim::BasicStanceEnum::EnumerateType();
      for (unsigned i = 0; i < basicStances.size(); ++i)
      {
         mExecutedActionCounts.insert(std::make_pair(basicStances[i], 0U));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   AnimationTransitionPlanner::~AnimationTransitionPlanner()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      const dtUtil::RefString ATP_GROUP("Animation Transition Planner");
      typedef dtCore::PropertyRegHelper<AnimationTransitionPlanner> RegHelperType;
      RegHelperType propReg(*this, this, ATP_GROUP);

      DT_REGISTER_PROPERTY(Stance, "The target stance of the animated character.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(WeaponState, "The weapon and how the character is using it.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(IsDead, "If this character is dead.", RegHelperType, propReg);
   }


   ////////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::Setup()
   {
      dtAI::WorldState initialState;

      BasicStanceState* stanceState = new BasicStanceState();

      // This is a STAGE workaround to ensure the newly set stance is
      // the only pose considered, since a full plan is excessive.
      // This simply sets the set stance as the default so that STAGE
      // can get directly to the stance result.
      // TODO: Remove when STAGE is fixed and this component is more data driven and not so hard coded.
      if (IsInSTAGE())
      {
         stanceState->SetStance(*mStance);
      }
      else // Normal stance setup
      {
         stanceState->SetStance(BasicStanceEnum::STANDING);
      }

      initialState.AddState(STATE_BASIC_STANCE,         stanceState);

      WeaponState* weaponState = new WeaponState();
      weaponState->SetWeaponStateEnum(WeaponStateEnum::DEPLOYED);
      initialState.AddState(STATE_WEAPON,                weaponState);

      initialState.AddState(STATE_DEAD,                  new dtAI::StateVariable(false));

//         initialState.AddState(STATE_MOVING,                new dtAI::StateVariable(false));
      //Setting transition to true will make the planner generate the correct initial animation.
      initialState.AddState(STATE_TRANSITION,            new dtAI::StateVariable(true));
      initialState.AddState(STATE_STANDING_ACTION_COUNT, new dtAI::StateVar<unsigned>(0U));
      initialState.AddState(STATE_KNEELING_ACTION_COUNT, new dtAI::StateVar<unsigned>(0U));
      initialState.AddState(STATE_SITTING_ACTION_COUNT,    new dtAI::StateVar<unsigned>(0U));
      initialState.AddState(STATE_LYING_ACTION_COUNT,    new dtAI::StateVar<unsigned>(0U));
      initialState.AddState(STATE_PRONE_ACTION_COUNT,    new dtAI::StateVar<unsigned>(0U));
      initialState.AddState(STATE_DYING,                  new dtAI::StateVariable(false));

      mPlannerHelper.SetCurrentState(initialState);
   }

   ////////////////////////////////////////////////////////////////////////////
   float AnimationTransitionPlanner::GetRemainingCost(const dtAI::WorldState* pWS) const
   {
      float value = 1.0f;

      const WeaponState* weaponState;
      pWS->GetState(STATE_WEAPON, weaponState);
      WeaponStateEnum* effectiveWeaponState = &WeaponStateEnum::FIRING_POSITION;
      if (*mWeaponState != WeaponStateEnum::FIRING_POSITION)
      {
         effectiveWeaponState = &WeaponStateEnum::DEPLOYED;
      }

      if (weaponState->GetWeaponStateEnum() != *effectiveWeaponState)
      {
         value += 1.0;
      }

      float preactionValue = value;

      value += 2.0 * float(CheckActionState(pWS, STATE_STANDING_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::STANDING)->second));
      value += 2.0 * float(CheckActionState(pWS, STATE_KNEELING_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::KNEELING)->second));
      value += 2.0 * float(CheckActionState(pWS, STATE_SITTING_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::SITTING)->second));
      value += 2.0 * float(CheckActionState(pWS, STATE_PRONE_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::PRONE)->second));

      //Only add the stance difference if no actions need to be performed
      if (preactionValue == value)
      {
         const BasicStanceState* stanceState;
         pWS->GetState(STATE_BASIC_STANCE, stanceState);

         // Use a smaller number for here than the actions so that completing the final action
         // won't make the planner think it is no closer to its goal.
         value += dtUtil::Abs(stanceState->GetStance().GetCostValue() - mStance->GetCostValue());
      }

      const dtAI::StateVariable* deadState;
      pWS->GetState(STATE_DEAD, deadState);

      //dead is the same as the damage state being equal to destroyed.
      if (deadState->Get() != (GetIsDead()))
         value += 1.0;

      return value;
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned AnimationTransitionPlanner::CheckActionState(const dtAI::WorldState* pWS, const std::string& stateName, unsigned desiredVal) const
   {
      const dtAI::StateVar<unsigned>* actionState = NULL;
      pWS->GetState(stateName, actionState);
      if (actionState == NULL)
      {
         return 0U;
      }

      if (desiredVal < actionState->Get())
      {
         return 0U;
      }

      return desiredVal - actionState->Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool AnimationTransitionPlanner::IsDesiredState(const dtAI::WorldState* pWS) const
   {
      //If we are in a transition, we are not in the desired state.
      const dtAI::StateVariable* transState;
      pWS->GetState(STATE_TRANSITION, transState);
      if (transState->Get())
         return false;

      const dtAI::StateVariable* deadState;
      pWS->GetState(STATE_DEAD, deadState);

      //dead is the same as the damage state being equal to destroyed.
      if (deadState->Get() != (GetIsDead()))
         return false;

      //If we are dead, ignore any other changes.  Just let's be dead, shall we :-)
      if (deadState->Get() && GetIsDead())
         return true;

      const BasicStanceState* stanceState;
      pWS->GetState(STATE_BASIC_STANCE, stanceState);

      if (stanceState->GetStance() != *mStance)
         return false;

      const WeaponState* weaponState;
      pWS->GetState(STATE_WEAPON, weaponState);

      WeaponStateEnum* effectiveWeaponState = &WeaponStateEnum::FIRING_POSITION;
      if (*mWeaponState != WeaponStateEnum::FIRING_POSITION)
         effectiveWeaponState = &WeaponStateEnum::DEPLOYED;

      if (weaponState->GetWeaponStateEnum() != *effectiveWeaponState)
         return false;

//         const dtAI::StateVariable* movingState;
//         pWS->GetState(STATE_MOVING, movingState);

      //This requires that plans be made in one frame.
      //Moving is the same as the velocity > 0.
      // When standing, we use the same thing for both standing
//         if (stanceState->GetStance() != BasicStanceEnum::STANDING && movingState->Get() != !dtUtil::Equivalent(CalculateWalkingSpeed(), 0.0f))
//            return false;

      bool actionStateResult =
         0U == CheckActionState(pWS, STATE_STANDING_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::STANDING)->second) &&
         0U == CheckActionState(pWS, STATE_KNEELING_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::KNEELING)->second) &&
         0U == CheckActionState(pWS, STATE_SITTING_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::SITTING)->second) &&
         0U == CheckActionState(pWS, STATE_LYING_ACTION_COUNT, mExecutedActionCounts.find(&BasicStanceEnum::LYING)->second) &&
         0U == CheckActionState(pWS, STATE_PRONE_ACTION_COUNT,    mExecutedActionCounts.find(&BasicStanceEnum::PRONE)->second);

      if (!actionStateResult)
         { return false; }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////////
   const Animatable* AnimationTransitionPlanner::ApplyOperatorAndGetAnimatable(const dtAI::Operator& op)
   {
      AnimationHelper* animAC = GetOwner()->GetComponent<AnimationHelper>();
      SequenceMixer& seqMixer = animAC->GetSequenceMixer();
      op.Apply(mPlannerHelper.GetCurrentState());

      const Animatable* animatable = NULL;

      const HumanOperator* hOp = dynamic_cast<const HumanOperator*>(&op);
      dtUtil::RefString nextReplacementAnim;
      if (hOp != NULL && hOp->GetNextReplacementAnim(nextReplacementAnim))
      {
         animatable = seqMixer.GetRegisteredAnimation(nextReplacementAnim);
      }
      else
      {
         animatable = seqMixer.GetRegisteredAnimation(op.GetName());
      }

      return animatable;
   }

   ////////////////////////////////////////////////////////////////////////////////////
   unsigned AnimationTransitionPlanner::GetExecutedActionCount(BasicStanceEnum& stance) const
   {
      return mExecutedActionCounts.find(&stance)->second;
   }

   ////////////////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::ExecuteAction(const dtUtil::RefString& animatableName, BasicStanceEnum& basicStance)
   {
      mExecutedActionCounts[&basicStance]++;

      dtUtil::RefString actionOpName;

      if (basicStance == BasicStanceEnum::STANDING)
      {
         actionOpName = AnimationOperators::ANIM_STANDING_ACTION;
      }
      else if (basicStance == BasicStanceEnum::KNEELING)
      {
         actionOpName = AnimationOperators::ANIM_KNEELING_ACTION;
      }
      else if (basicStance == BasicStanceEnum::SITTING)
      {
         actionOpName = AnimationOperators::ANIM_SITTING_ACTION;
      }
      else if (basicStance == BasicStanceEnum::LYING)
      {
         actionOpName = AnimationOperators::ANIM_LYING_ACTION;
      }
      else if (basicStance == BasicStanceEnum::PRONE)
      {
         actionOpName = AnimationOperators::ANIM_PRONE_ACTION;
      }

      if (animatableName != actionOpName)
      {
         const HumanOperator* hOp = NULL;
         hOp = dynamic_cast<const HumanOperator*>(mPlannerHelper.GetOperator(actionOpName));
         if (hOp != NULL)
         {
            hOp->EnqueueReplacementAnim(animatableName);
         }
      }

//      if (!IsRemote())
//      {
//         //TODO send action message.
//      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::CheckAndUpdateAnimationState()
   {
      // TODO Check the animation helper for this.
      if (!IsDesiredState(mPlannerHelper.GetCurrentState())) //&& mModelNode.valid())
      {
         LOGN_DEBUG("animationtransitionplanner.cpp", "The planner is not in the desired state on actor named \"" + GetName() + "\".  Generating animations.");

         UpdatePlanAndAnimations();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::UpdatePlanAndAnimations()
   {
      const float blendTime = 0.2f;

      const dtAI::StateVariable* deadState = NULL;
      mPlannerHelper.GetCurrentState()->GetState(STATE_DEAD, deadState);

      //if we WERE dead and now we are not, we have to reset our state.
      if (deadState->Get() && !GetIsDead())
         Setup();

      bool gottaSequence = GenerateNewAnimationSequence();
      if (!gottaSequence)
      {
         Setup();
         gottaSequence = GenerateNewAnimationSequence();
      }

      const Animatable* animatable = NULL;
      AnimationHelper* animAC = GetOwner()->GetComponent<AnimationHelper>();

      if (gottaSequence)
      {
         dtAI::Planner::OperatorList::iterator i, iend;
         SequenceMixer& seqMixer = animAC->GetSequenceMixer();
         dtCore::RefPtr<AnimationSequence> generatedSequence = new AnimationSequence();

         std::string oldSeqId = mSequenceId;
         if (mSequenceId.empty())
         {
            mSequenceId = "seq:0";
         }
         else
         {
            // rather than do an int to string, just change the last character so it does'0'-'9'.
            // this will require much less overhead, and won't ever require allocating
            // and deallocating string memory.
            mSequenceId[4] = (((mSequenceId[4] - '0') + 1) % 10) + '0';
         }

         generatedSequence->SetName(mSequenceId);

         LOGN_DEBUG("animationtransitionplanner.cpp", "Current animation plan has \"" + dtUtil::ToString(mCurrentPlan.size()) + "\" steps.");

         if (!mCurrentPlan.empty())
         {
            i = mCurrentPlan.begin();
            iend = mCurrentPlan.end();

            float accumulatedStartTime = 0.0f;

            dtCore::RefPtr<Animatable> newAnim;
            for (; i != iend; ++i)
            {
               //if the last anim was NOT the last one, it has to end and be an action
               if (newAnim)
               {
                  AnimationChannel* animChannel = dynamic_cast<AnimationChannel*>(newAnim.get());
                  if (animChannel != NULL)
                  {

                     float duration = animChannel->GetAnimation()->GetDuration();
                     accumulatedStartTime += (duration - blendTime);
                     animChannel->SetMaxDuration(duration);
                     animChannel->SetAction(true);
                  }
               }

               animatable = ApplyOperatorAndGetAnimatable(**i);

               if (animatable != NULL)
               {
                  LOGN_DEBUG("animationtransitionplanner.cpp", std::string("Adding animatable named \"") + animatable->GetName().c_str() + "\".");
                  newAnim = animatable->Clone(GetOwner()->GetComponent<AnimationHelper>()->GetModelWrapper());
                  newAnim->SetStartDelay(std::max(0.0f, accumulatedStartTime));
                  newAnim->SetFadeIn(blendTime);
                  newAnim->SetFadeOut(blendTime);

                  generatedSequence->AddAnimation(newAnim);
               }
               else
               {
                  LOGN_DEBUG("animationtransitionplanner.cpp", std::string("Unable to find animatable with name: \"") + (*i)->GetName() + "\".");
                  newAnim = NULL;
               }
            }

            bool posesWereEnabled = animAC->GetPosesEnabled();
            seqMixer.ClearAnimation(oldSeqId, blendTime);
            seqMixer.ClearAnimation(AnimationOperators::ANIM_WALK_DEPLOYED, blendTime);
            seqMixer.PlayAnimation(generatedSequence.get());
            animAC->SetPosesEnabled(posesWereEnabled);

            SignalAnimationsTransitioning.emit_signal(*this);
         }
      }
      else
      {
         //This is the error-out state.
         animAC->ClearAnimation(mSequenceId, blendTime);
         animAC->PlayAnimation(AnimationOperators::ANIM_WALK_DEPLOYED);
         SignalAnimationsTransitioning.emit_signal(*this);
      }
      
      // Ensure that the mixer has at least one update in STAGE
      // so that the initial state can take effect.
      // TODO: Remove this once STAGE has its own way to update actors
      // through normal program flow.
      if (IsInSTAGE() && animAC != NULL && animatable != NULL)
      {
         animAC->ClearAll(0.0f);
         animAC->PlayAnimation(animatable->GetName());
         animAC->Update(blendTime);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool AnimationTransitionPlanner::GenerateNewAnimationSequence()
   {
      mCurrentPlan.clear();
      mPlanner.Reset(&mPlannerHelper);

      mPlanner.GetConfig().mMaxTimePerIteration = mMaxTimePerIteration;

      dtAI::Planner::PlannerResult result = mPlanner.GeneratePlan();
      if (result == dtAI::Planner::PLAN_FOUND)
      {
         mCurrentPlan = mPlanner.GetConfig().mResult;
         //std::cout << " BOGUS TEST -- animationtransitionplanner.cpp - Plan took[" << mPlanner.GetConfig().mTotalElapsedTime << "]." << std::endl;
         return true;
      }
      else
      {
         std::ostringstream ss;
         ss << "Unable to generate a plan. Time[" << mPlanner.GetConfig().mTotalElapsedTime
            << "]\n\nGoing from:\n\n"
            << *mPlannerHelper.GetCurrentState()
            << "\n\n Going To:\n\n"
            << "Stance:  \"" << GetStance().GetName()
            << "\"\n Primary Weapon: \"" << GetWeaponState().GetName()
            << "\"\n IsDead: \"" << GetIsDead();
         ExecuteActionCountMap::const_iterator i, iend;
         i = mExecutedActionCounts.begin();
         iend = mExecutedActionCounts.end();
         for (; i != iend; ++i)
         {
            ss << i->first->GetName() << ": \"" << i->second << "\" \n";
         }
         LOGN_ERROR("animationtransitionplanner.cpp", ss.str());
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtAI::Planner::OperatorList& AnimationTransitionPlanner::GetCurrentPlan()
   {
      return mCurrentPlan;
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::SetupAnimationHelper()
   {
      if (mIsAnimHelperSetup)
      {
         LOG_WARNING("Called SetupAnimationHelper more than once.");
         return;
      }

      AnimationHelper* animAC = GetOwner()->GetComponent<AnimationHelper>();

      if (animAC == NULL)
      {
         animAC = new dtAnim::AnimationHelper;
         GetOwner()->AddComponent(*animAC);
      }

      if (animAC != NULL)
      {
         if (animAC->GetModelWrapper() != NULL)
         {
            OnModelLoaded(animAC);
         }
         else
         {
            // Reset the state just so it's not empty.
            Setup();
         }

         animAC->ModelLoadedSignal.connect_slot(this, &AnimationTransitionPlanner::OnModelLoaded);
         animAC->ModelUnloadedSignal.connect_slot(this, &AnimationTransitionPlanner::OnModelUnloaded);

         mIsAnimHelperSetup = true;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      SetupAnimationHelper();

      RegisterForTick();
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::OnTickLocal(const dtGame::TickMessage& /*tickMessage*/)
   {
      Update();
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::OnTickRemote(const dtGame::TickMessage& /*tickMessage*/)
   {
      Update();
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::Update()
   {
      if (mResetNextTick)
      {
         Setup();
         UpdatePlanAndAnimations();
         mResetNextTick = false;
      }
      else
      {
         CheckAndUpdateAnimationState();
      }
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::OnAddedToActor(dtCore::BaseActorObject& actor)
   {
      BaseClass::OnAddedToActor(actor);

      // TEMP: This is a temporary workaround to initialize the AnimationHelper
      // for the sake of showing stances in STAGE.
      if (IsInSTAGE())
      {
         SetupAnimationHelper();

         if ( ! mIsAnimHelperSetup)
         {
            LOG_ERROR("Could not setup the AnimationHelper at this point.");
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      BaseClass::OnRemovedFromActor(actor);

      AnimationHelper* animAC = GetOwner()->GetComponent<AnimationHelper>();

      if (animAC != NULL)
      {
         animAC->ModelLoadedSignal.disconnect(this);
         animAC->ModelUnloadedSignal.disconnect(this);
      }
      else
      {
         LOG_ERROR("Could not disconnect from original AnimationHelper since it can no longer be accessed from the owner.");
      }

      mIsAnimHelperSetup = false;
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::OnModelLoaded(AnimationHelper*)
   {
      // Have to wait until the next tick because other listeners may get this message and do things that
      // will change the results of this.
      mResetNextTick = true;

      // This ensures that the initial stance is set on the model when it
      // is loaded in STAGE.
      if (IsInSTAGE())
      {
         Update();
      }
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::OnModelUnloaded(AnimationHelper*)
   {

   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::SetStance(dtAnim::BasicStanceEnum& stance)
   {
      if (mStance.get() != &stance)
      {
         mStance = &stance;

         // This was recommended temporarily for getting animations to
         // work in STAGE. This allows the newly set stance to be set
         // in the planner and update called at least once so that the
         // character assumes the initial animation pose.
         // TODO: Remove when STAGE has been fixed to generically work with animations.
         if (IsInSTAGE())
         {
            OnModelLoaded(NULL);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////
   dtAnim::BasicStanceEnum& AnimationTransitionPlanner::GetStance() const
   {
      return *mStance;
   }

   ////////////////////////////////////////////////////////////////////////
   void AnimationTransitionPlanner::SetWeaponState(dtAnim::WeaponStateEnum& weaponState)
   {
      if (mWeaponState.get() != &weaponState)
      {
         mWeaponState = &weaponState;

         // This was recommended temporarily for getting animations to
         // work in STAGE. This allows the newly set stance to be set
         // in the planner and update called at least once so that the
         // character assumes the initial animation pose.
         // TODO: Remove when STAGE has been fixed to generically work with animations.
         if (IsInSTAGE())
         {
            OnModelLoaded(NULL);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////
   dtAnim::WeaponStateEnum& AnimationTransitionPlanner::GetWeaponState() const
   {
      return *mWeaponState;
   }

   ////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(AnimationTransitionPlanner, bool, IsDead);
   //DT_IMPLEMENT_ACCESSOR(AnimationTransitionPlanner, dtUtil::EnumerationPointer<BasicStanceEnum>, Stance);
   //DT_IMPLEMENT_ACCESSOR(AnimationTransitionPlanner, dtUtil::EnumerationPointer<WeaponStateEnum>, WeaponState);
   DT_IMPLEMENT_ACCESSOR(AnimationTransitionPlanner, double, MaxTimePerIteration);


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString AnimationOperators::ANIM_WALK_READY("Walk Run Ready");
   const dtUtil::RefString AnimationOperators::ANIM_WALK_DEPLOYED("Walk Run Deployed");

   const dtUtil::RefString AnimationOperators::ANIM_SITTING_READY("Sitting Ready");
   const dtUtil::RefString AnimationOperators::ANIM_SITTING_DEPLOYED("Sitting");

   const dtUtil::RefString AnimationOperators::ANIM_LYING_READY("Lying Ready");
   const dtUtil::RefString AnimationOperators::ANIM_LYING_DEPLOYED("Lying");

   const dtUtil::RefString AnimationOperators::ANIM_LOW_WALK_READY("Kneel-Low Walk Ready");
   const dtUtil::RefString AnimationOperators::ANIM_LOW_WALK_DEPLOYED("Kneel-Low Walk Deployed");

   const dtUtil::RefString AnimationOperators::ANIM_STAND_TO_KNEEL("Stand To Kneel");
   const dtUtil::RefString AnimationOperators::ANIM_KNEEL_TO_STAND("Kneel To Stand");

   const dtUtil::RefString AnimationOperators::ANIM_STAND_TO_SIT("Stand To Sit");
   const dtUtil::RefString AnimationOperators::ANIM_SIT_TO_STAND("Sit To Stand");

   const dtUtil::RefString AnimationOperators::ANIM_SIT_TO_LIE("Sit To Lie");
   const dtUtil::RefString AnimationOperators::ANIM_LIE_TO_SIT("Lie To Sit");

   const dtUtil::RefString AnimationOperators::ANIM_CRAWL_READY("Prone-Crawl Ready");
   const dtUtil::RefString AnimationOperators::ANIM_CRAWL_DEPLOYED("Prone-Crawl Deployed");

   const dtUtil::RefString AnimationOperators::ANIM_PRONE_TO_KNEEL("Prone To Kneel");
   const dtUtil::RefString AnimationOperators::ANIM_KNEEL_TO_PRONE("Kneel To Prone");

   const dtUtil::RefString AnimationOperators::ANIM_DYING_STANDING("Shot Standing");
   const dtUtil::RefString AnimationOperators::ANIM_DYING_KNEELING("Shot Kneeling");
   const dtUtil::RefString AnimationOperators::ANIM_DYING_PRONE("Shot Prone");

   const dtUtil::RefString AnimationOperators::ANIM_DEAD_STANDING("Dead Standing");
   const dtUtil::RefString AnimationOperators::ANIM_DEAD_KNEELING("Dead Kneeling");
   const dtUtil::RefString AnimationOperators::ANIM_DEAD_PRONE("Dead Prone");

   const dtUtil::RefString AnimationOperators::ANIM_STANDING_ACTION("Standing Action");
   const dtUtil::RefString AnimationOperators::ANIM_KNEELING_ACTION("Kneeling Action");
   const dtUtil::RefString AnimationOperators::ANIM_PRONE_ACTION("Prone Action");
   const dtUtil::RefString AnimationOperators::ANIM_LYING_ACTION("Lying Action");
   const dtUtil::RefString AnimationOperators::ANIM_SITTING_ACTION("Sitting Action");

   const dtUtil::RefString AnimationOperators::OPER_DEPLOYED_TO_READY("Deployed To Ready");
   const dtUtil::RefString AnimationOperators::OPER_READY_TO_DEPLOYED("Ready To Deployed");

   ////////////////////////////////////////////////////////////////////////////
   AnimationOperators::AnimationOperators(dtAI::PlannerHelper& plannerHelper):
      mPlannerHelper(plannerHelper)
   {
      CreateOperators();
   }

   ////////////////////////////////////////////////////////////////////////////
   AnimationOperators::~AnimationOperators()
   {
   }


   ////////////////////////////////////////////////////////////////////////////
   HumanOperator* AnimationOperators::AddOperator(const std::string& name)
   {
      HumanOperator* op = new HumanOperator(name);
      mOperators.insert(std::make_pair(op->GetName(), op));
      mPlannerHelper.AddOperator(op);
      return op;
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationOperators::CreateOperators()
   {

      dtCore::RefPtr<HumanOperator::WeaponStateEnumConditional> deployed
         = new HumanOperator::WeaponStateEnumConditional(AnimationTransitionPlanner::STATE_WEAPON, WeaponStateEnum::DEPLOYED);

      dtCore::RefPtr<HumanOperator::WeaponStateEnumConditional> ready
         = new HumanOperator::WeaponStateEnumConditional(AnimationTransitionPlanner::STATE_WEAPON, WeaponStateEnum::FIRING_POSITION);


      dtCore::RefPtr<dtAI::Precondition> isDying
         = new dtAI::Precondition(AnimationTransitionPlanner::STATE_DYING, true);


      dtCore::RefPtr<dtAI::Precondition> isTransition
         = new dtAI::Precondition(AnimationTransitionPlanner::STATE_TRANSITION, true);

      dtCore::RefPtr<dtAI::Precondition> notTransition
         = new dtAI::Precondition(AnimationTransitionPlanner::STATE_TRANSITION, false);


      std::map<BasicStanceEnum*, dtCore::RefPtr<HumanOperator::BasicStanceEnumEffect> > stanceEffects;
      std::map<BasicStanceEnum*, dtCore::RefPtr<HumanOperator::BasicStanceEnumConditional> > stanceConditionals;
      for (unsigned i = 0; i < BasicStanceEnum::EnumerateType().size(); ++i)
      {
         BasicStanceEnum* curEnum = BasicStanceEnum::EnumerateType()[i];
         stanceEffects[curEnum] = new HumanOperator::BasicStanceEnumEffect(AnimationTransitionPlanner::STATE_BASIC_STANCE, *curEnum);
         stanceConditionals[curEnum] = new HumanOperator::BasicStanceEnumConditional(AnimationTransitionPlanner::STATE_BASIC_STANCE, *curEnum);
      }

      dtCore::RefPtr<HumanOperator::WeaponStateEnumEffect> readyEff
         = new HumanOperator::WeaponStateEnumEffect(AnimationTransitionPlanner::STATE_WEAPON, WeaponStateEnum::FIRING_POSITION);

      dtCore::RefPtr<HumanOperator::WeaponStateEnumEffect> deployedEff
         = new HumanOperator::WeaponStateEnumEffect(AnimationTransitionPlanner::STATE_WEAPON, WeaponStateEnum::DEPLOYED);

      dtCore::RefPtr<HumanOperator::UnsignedIntIncrementEffect >
         incrementStandingActionCount = new HumanOperator::UnsignedIntIncrementEffect(AnimationTransitionPlanner::STATE_STANDING_ACTION_COUNT);
      dtCore::RefPtr<HumanOperator::UnsignedIntIncrementEffect >
         incrementKneelingActionCount = new HumanOperator::UnsignedIntIncrementEffect(AnimationTransitionPlanner::STATE_KNEELING_ACTION_COUNT);
      dtCore::RefPtr<HumanOperator::UnsignedIntIncrementEffect >
         incrementSittingActionCount = new HumanOperator::UnsignedIntIncrementEffect(AnimationTransitionPlanner::STATE_SITTING_ACTION_COUNT);
      dtCore::RefPtr<HumanOperator::UnsignedIntIncrementEffect >
         incrementProneActionCount = new HumanOperator::UnsignedIntIncrementEffect(AnimationTransitionPlanner::STATE_PRONE_ACTION_COUNT);

      dtCore::RefPtr<dtAI::Effect>
         deadEff = new dtAI::Effect(AnimationTransitionPlanner::STATE_DEAD, true);

      dtCore::RefPtr<dtAI::Effect>
         dyingEff = new dtAI::Effect(AnimationTransitionPlanner::STATE_DYING, true);

      dtCore::RefPtr<dtAI::Effect>
         transitionEff = new dtAI::Effect(AnimationTransitionPlanner::STATE_TRANSITION, true);

      dtCore::RefPtr<dtAI::Effect>
         notTransitionEff = new dtAI::Effect(AnimationTransitionPlanner::STATE_TRANSITION, false);

      HumanOperator* newOp;

      newOp = AddOperator(ANIM_WALK_READY);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddPreCondition(ready.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::STANDING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_WALK_DEPLOYED);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::STANDING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_LOW_WALK_READY);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddPreCondition(ready.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::KNEELING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_LOW_WALK_DEPLOYED);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::KNEELING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_SITTING_READY);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::SITTING]);
      newOp->AddPreCondition(ready.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::SITTING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_SITTING_DEPLOYED);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::SITTING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::SITTING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_LYING_READY);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::LYING]);
      newOp->AddPreCondition(ready.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::LYING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_LYING_DEPLOYED);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::LYING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::LYING]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_STAND_TO_SIT);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::SITTING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_SIT_TO_STAND);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::SITTING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::STANDING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_SIT_TO_LIE);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::SITTING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::LYING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_LIE_TO_SIT);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::LYING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::SITTING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_STAND_TO_KNEEL);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::KNEELING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_KNEEL_TO_STAND);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::STANDING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_STAND_TO_KNEEL);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::KNEELING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_KNEEL_TO_STAND);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::STANDING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_PRONE_TO_KNEEL);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::PRONE]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::KNEELING]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_CRAWL_READY);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::PRONE]);
      newOp->AddPreCondition(ready.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::PRONE]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_CRAWL_DEPLOYED);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::PRONE]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::PRONE]);
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_KNEEL_TO_PRONE);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddPreCondition(deployed.get());

      newOp->AddEffect(stanceEffects[&BasicStanceEnum::PRONE]);
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(OPER_READY_TO_DEPLOYED);
      newOp->AddPreCondition(ready.get());
      newOp->AddEffect(deployedEff.get());
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(OPER_DEPLOYED_TO_READY);
      newOp->AddPreCondition(deployed.get());
      newOp->AddEffect(readyEff.get());
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_DYING_STANDING);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddEffect(dyingEff.get());
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_DYING_KNEELING);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddEffect(dyingEff.get());
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_DYING_PRONE);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::PRONE]);
      newOp->AddEffect(dyingEff.get());
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_DEAD_STANDING);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddPreCondition(isDying.get());
      newOp->AddEffect(deadEff.get());
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_DEAD_KNEELING);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddPreCondition(isDying.get());
      newOp->AddEffect(deadEff.get());
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_DEAD_PRONE);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::PRONE]);
      newOp->AddPreCondition(isDying.get());
      newOp->AddEffect(deadEff.get());
      newOp->AddEffect(notTransitionEff.get());

      newOp = AddOperator(ANIM_STANDING_ACTION);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::STANDING]);
      newOp->AddPreCondition(deployed.get());
      newOp->AddEffect(incrementStandingActionCount.get());
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_KNEELING_ACTION);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::KNEELING]);
      newOp->AddPreCondition(deployed.get());
      newOp->AddEffect(incrementKneelingActionCount.get());
      newOp->AddEffect(transitionEff.get());

      newOp = AddOperator(ANIM_PRONE_ACTION);
      newOp->AddPreCondition(stanceConditionals[&BasicStanceEnum::PRONE]);
      newOp->AddPreCondition(deployed.get());
      newOp->AddEffect(incrementProneActionCount.get());
      newOp->AddEffect(transitionEff.get());
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(BasicStanceEnum);
   BasicStanceEnum BasicStanceEnum::IDLE("IDLE", 1.75f);
   BasicStanceEnum BasicStanceEnum::STANDING("STANDING", 1.75f);
   BasicStanceEnum BasicStanceEnum::SITTING("SITTING", 1.10f);
   BasicStanceEnum BasicStanceEnum::LYING("LYING", 1.05f);
   BasicStanceEnum BasicStanceEnum::KNEELING("KNEELING", 1.0f);
   BasicStanceEnum BasicStanceEnum::PRONE("PRONE", 0.0f);

   ////////////////////////////////////////////////////////////////////////////
   BasicStanceEnum::BasicStanceEnum(const std::string& name, float costValue)
   : dtUtil::Enumeration(name)
   , mCostValue(costValue)
   {
      AddInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////
   float BasicStanceEnum::GetCostValue() const
   {
      return mCostValue;
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   BasicStanceState::BasicStanceState()
   : mStance(&BasicStanceEnum::IDLE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   BasicStanceState::~BasicStanceState()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   dtAI::IStateVariable* BasicStanceState::Copy() const
   {
      BasicStanceState* stanceState = new BasicStanceState;
      stanceState->mStance = mStance;
      return stanceState;
   }

   ////////////////////////////////////////////////////////////////////////////
   BasicStanceEnum& BasicStanceState::GetStance() const
   {
      return *mStance;
   }

   ////////////////////////////////////////////////////////////////////////////
   void BasicStanceState::SetStance(BasicStanceEnum& newStance)
   {
      mStance = &newStance;
   }
   ////////////////////////////////////////////////////////////////////////////
   BasicStanceEnum& BasicStanceState::GetValue() const
   {
      return GetStance();
   }

   ////////////////////////////////////////////////////////////////////////////
   void BasicStanceState::SetValue(BasicStanceEnum& pStance)
   {
      SetStance(pStance);
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string BasicStanceState::ToString() const
   {
      return GetStance().GetName();
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   WeaponState::WeaponState():
      mWeaponStateEnum(&WeaponStateEnum::STOWED)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   WeaponState::~WeaponState()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   dtAI::IStateVariable* WeaponState::Copy() const
   {
      WeaponState* weaponState = new WeaponState;
      weaponState->mWeaponStateEnum = mWeaponStateEnum;
      return weaponState;
   }

   ////////////////////////////////////////////////////////////////////////////
   WeaponStateEnum& WeaponState::GetWeaponStateEnum() const
   {
      return *mWeaponStateEnum;
   }

   ////////////////////////////////////////////////////////////////////////////
   void WeaponState::SetWeaponStateEnum(WeaponStateEnum& newWeaponStateEnum)
   {
      mWeaponStateEnum = &newWeaponStateEnum;
   }

   ////////////////////////////////////////////////////////////////////////////
   WeaponStateEnum& WeaponState::GetValue() const
   {
      return GetWeaponStateEnum();
   }

   ////////////////////////////////////////////////////////////////////////////
   void WeaponState::SetValue(WeaponStateEnum& pWeaponState)
   {
      SetWeaponStateEnum(pWeaponState);
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string WeaponState::ToString() const
   {
      return GetWeaponStateEnum().GetName();
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   HumanOperator::HumanOperator(const dtUtil::RefString& pName)
   : Operator(pName, Operator::ApplyOperatorFunctor(this, &HumanOperator::Apply))
   , mCost(1.0f)
   {}

   ////////////////////////////////////////////////////////////////////////////
   void HumanOperator::SetCost(float pcost) { mCost = pcost; }

   ////////////////////////////////////////////////////////////////////////////
   void HumanOperator::AddEffect(EffectType* pEffect) { mEffects.push_back(pEffect); }

   ////////////////////////////////////////////////////////////////////////////
   void HumanOperator::EnqueueReplacementAnim(const dtUtil::RefString& animName) const
   {
      mReplacementQueue.push_back(animName);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool HumanOperator::GetNextReplacementAnim(dtUtil::RefString& animName, bool dequeue) const
   {
      if (mReplacementQueue.empty())
      {
         return false;
      }

      animName = mReplacementQueue.front();
      if (dequeue)
      {
         mReplacementQueue.pop_front();
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool HumanOperator::Apply(const dtAI::Operator* oper, dtAI::WorldState* pWSIn) const
   {
      EffectList::const_iterator iter = mEffects.begin();
      EffectList::const_iterator endOfList = mEffects.end();
      for (;iter != endOfList; ++iter)
      {
         (*iter)->Apply(oper, pWSIn);
      }

      pWSIn->AddCost(mCost);
      return true;
   }

}

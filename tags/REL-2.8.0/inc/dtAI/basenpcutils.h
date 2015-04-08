/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 Bradley Anderegg
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
 * Bradley Anderegg 07/24/2006
 */

#ifndef __DELTA_PLANNERASTARUTILS_H__
#define __DELTA_PLANNERASTARUTILS_H__

#include <dtAI/operator.h>
#include <dtAI/worldstate.h>
#include <dtAI/conditional.h>
#include <dtAI/statevariable.h>

#include <dtUtil/functor.h>

#include <dtCore/refptr.h>

#include <list>
#include <vector>
#include <string>
#include <sstream>

/** \file
 * Utilities for making the use of the dtAI Planner even easier.
 */

namespace dtAI
{

   /**
    * Templated IStateVariable to make implementing a basic state easier.  Call
    * Set() and Get() to access the value contained within.
    * For example, to add a new boolean state named "StateHunger" to the WorldState:
    * @code 
    *   worldState->AddState("StateHunger", new dtAI::StateVar<bool>(true));
    * @endcode
    */
   template<typename _Type>
   class StateVar: public IStateVariable
   {
   public:

      /** 
       * @param pData : The data value this IStateVariable is managing.  Must be
       *                of _Type type.
       */
      StateVar(const _Type& pData) : mData(pData) {}

      IStateVariable* Copy() const
      {
         return new StateVar<_Type>(mData);
      }

      /** 
       * Get the value of this state.
       * @return The current value of type _Type
       */
      const _Type& Get() const
      {
         return mData;
      }

      /** 
       * Set the value of this state.
       * @param pData : The new value of type _Type
       */
      void Set(const _Type& pData)
      {
         mData = pData;
      }

      virtual const std::string ToString() const
      {
         std::ostringstream ss;
         ss << mData;
         return ss.str();
      }

   private:
      _Type mData;
   };


   //template <typename _Type>
   //   void Associate(WorldState* pWS, Operator* pOperator, const std::string& pPropertyName, const _Type& pStateVar, const _Type& pConditional)
   //{
   //   pWS->AddState(pPropertyName, new StateVar<_Type>(pStateVar));
   //   pOperator->AddPreCondition(new Conditional<_Type>(pPropertyName, pConditional));
   //}

   template <typename _Type>
   const StateVar<_Type>* GetWorldStateVariable(const WorldState* pWS, const std::string& pName)
   {
      return dynamic_cast<const StateVar<_Type>* >(pWS->GetState(pName));
   }

   template <typename _Type>
   StateVar<_Type>* GetWorldStateVariable(WorldState* pWS, const std::string& pName)
   {
      return dynamic_cast<StateVar<_Type>* >(pWS->GetState(pName));
   }



   /** Templated IConditional to make it easier implementing basic precondition
     * checks.  Will automatically verify if an existing dtAI::StateVar in the
     * dtAI::WorldState equates (==) to the supplied value.
     * For example, to add a precondition to an Operation that verifies the 
     * boolean "HasFood" StateVar is true:
     * @code 
     *   myOperation->AddPreCondition(new dtAI::Conditional<bool>("HasFood", true);
     * @endcode
     */
   template <typename _Type>
   class Conditional: public IConditional
   {
   public:
      /** 
        * @param pName : the name of this IConditional and the name of the 
        *                StateVar to check.
        * @param pData : the value of the StateVar to equate to.  Must be of _Type type.
        */
      Conditional(const std::string& pName, const _Type& pData): mName(pName), mData(pData){}

      virtual const std::string& GetName() const
      {
         return mName;
      }

      virtual bool Evaluate(const WorldState* pWS)
      {
         const StateVar<_Type>* pStateVar = GetWorldStateVariable<_Type>(pWS, mName);
         if (pStateVar)
         {
            return pStateVar->Get() == mData;
         }
         return false;
      }

   protected:
      ~Conditional(){}

   private:
      std::string mName;
      _Type mData;
   };

   class IEffect: public osg::Referenced
   {
   public:
      virtual bool Apply(const Operator*, WorldState* pWSIn) const = 0;
   };


   /** 
    * Templated IEffect that makes it easier to implement simple IEffects.
    *
    */
   template <typename _Type>
   class Eff: public IEffect
   {
   public:
      /** 
       * @param pName : the name of the existing StateVar to affect
       * @param pData : The value to apply to the StateVar.  Must be of type _Type
       */
      Eff(const std::string& pName, const _Type& pData) : mName(pName), mData(pData) {}
      ~Eff() {}

      virtual bool Apply(const Operator*, WorldState* pWSIn) const
      {
         StateVar<_Type>* pState = GetWorldStateVariable<_Type>(pWSIn, mName);
         pState->Set(mData);
         return true;
      }

   private:
      std::string mName;
      _Type mData;
   };

   /** 
    * A templated Conditional, used to represent a Goal state to achieve.
    */
   template<typename _CostType, typename _Type>
   class TGoal: public Conditional<_Type>
   {
   public:
      /** 
       * @param pName the name of this Conditional and the name of the StateVar this
       *        goal is referring to
       * @param pData The data value the StateVar should be in order for this
       *              goal to be achieved.
       */
      TGoal(const std::string& pName, const _Type& pData, _CostType pCost)
      : Conditional<_Type>(pName, pData),
        mCost(pCost){}

      _CostType GetCost(){return mCost;}

   private:
      _CostType mCost;
   };


   typedef StateVar<bool>        StateVariable;
   typedef Conditional<bool>     Precondition;
   typedef Conditional<bool>     Interrupt;
   typedef Eff<bool>             Effect;

   /** 
    * A specialized TGoal used with dtAI::BaseNPC.  The cost type is a float
    * and the StateVar type is a bool.
    * @see dtAI::BaseNPC::AddGoal()
    */
   typedef TGoal<float, bool> Goal;


   /** 
    * A templatized Operator that has built in ability to alter StateVar states (AddEffect()).
    */
   template <typename _Type>
   class TOperator: public Operator
   {
   public:
      typedef Eff<_Type> EffectType;
      typedef Conditional<_Type> InterruptType;
      typedef std::vector< dtCore::RefPtr<EffectType> > EffectList;
      typedef std::vector< dtCore::RefPtr<InterruptType> > InterruptList;

   public:
      /** 
       * @param pName The name of this Operator.
       */
      TOperator(const std::string& pName):
         Operator(pName, Operator::ApplyOperatorFunctor(this, &TOperator<_Type>::Apply))
         ,mCost(0.0f)
         {}

      void SetCost(float pcost){mCost = pcost;}

      /** 
       * Add an "effect" to this Operator.  The list of EffectType supplied will
       * take affect when this Operator is triggered.
       * Example, to automatically set the boolean StateVar "StateAmountFood" to true
       * when this operates:
       * @code
       *    myOp->AddEffect(new dtAI::TOperator<bool>::EffectType("StateAmountFood", true));
       * @endcode
       * @param pEffect : The EffectType to operate on.     
       */
      void AddEffect(EffectType* pEffect) { mEffects.push_back(pEffect); }
      void AddInterrupt(InterruptType* pInterrupt) { mInterrupts.push_back(pInterrupt); }

      bool CheckInterrupts(const WorldState* pCurrent) const
      {
         typename InterruptList::const_iterator iter = mInterrupts.begin();
         typename InterruptList::const_iterator endOfList = mInterrupts.end();
         while (iter != endOfList)
         {
            if ((*iter)->Evaluate(pCurrent))
            {
               return false;
            }
            ++iter;
         }
         return true;
      }

      bool Apply(const Operator* oper, WorldState* pWSIn) const
      {
         typename EffectList::const_iterator iter = mEffects.begin();
         typename EffectList::const_iterator endOfList = mEffects.end();
         while (iter != endOfList)
         {
            (*iter)->Apply(oper, pWSIn);
            ++iter;
         }

         pWSIn->AddCost(mCost);
         return true;
      }

   private:
      float mCost;
      EffectList mEffects;
      InterruptList mInterrupts;
   };

   /**
    *  A specialized TOperator that operates on a boolean StateVar
    */
   typedef TOperator<bool> NPCOperator;

} // namespace dtAI

#endif // __DELTA_PLANNERASTARUTILS_H__

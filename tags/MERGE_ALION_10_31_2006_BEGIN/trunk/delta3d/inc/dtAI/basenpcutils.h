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

#include <list>
#include <vector>
#include <string>

namespace dtAI
{
   /**
    * Utilities for making the use of the Planner even easier
    */

   template<typename _Type>
   class StateVar: public IStateVariable
   {
   public:
      StateVar(const _Type& pData): mData(pData){}

      IStateVariable* Copy() const
      {
         return new StateVar<_Type>(mData);
      }

      const _Type& Get() const
      {
         return mData;
      }

      void Set(const _Type& pData)
      {
         mData = pData;
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



   template <typename _Type>
   class Conditional: public IConditional
   {
   public:
      Conditional(const std::string& pName, const _Type& pData): mName(pName), mData(pData){}
      ~Conditional(){}

      /*virtual*/ const std::string& GetName() const
      {
         return mName;
      }

      /*virtual*/ bool Evaluate(const WorldState* pWS)
      {         
         const StateVar<_Type>* pStateVar = GetWorldStateVariable<_Type>(pWS, mName);
         if(pStateVar)
         {
            return pStateVar->Get() == mData;
         }
         return false;
      }


   private:
      std::string mName;
      _Type mData;
   };

   class IEffect
   {
   public:
      virtual bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const = 0;

   };


   template <typename _Type>
   class Eff: public IEffect
   {
   public:
      Eff(const std::string& pName, const _Type& pData): mName(pName), mData(pData){}
      ~Eff(){}

      bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const
      {
         StateVar<_Type>* pState = GetWorldStateVariable<_Type>(pWSIn, mName);
         pState->Set(mData);
         return true;
      }

   private:
      std::string mName;
      _Type mData;
   };

   template<typename _CostType, typename _Type>
   class TGoal: public Conditional<_Type>
   {
   public:
      TGoal(const std::string& pName, const _Type& pData, _CostType pCost): Conditional<_Type>(pName, pData), mCost(pCost){}

      _CostType GetCost(){return mCost;}

   private:
      _CostType mCost;
   };


   typedef StateVar<bool>        StateVariable;
   typedef Conditional<bool>     Precondition;
   typedef Conditional<bool>     Interrupt;
   typedef Eff<bool>             Effect;

   typedef TGoal<float, bool> Goal;


   template <typename _Type>
   class TOperator: public Operator
   {
      public:
         typedef std::list<IEffect*> EffectList;
         typedef std::list<Interrupt*> InterruptList;

      public:
         TOperator(const std::string& pName): Operator(pName, Operator::ApplyOperatorFunctor(this, &TOperator<_Type>::Apply)){}

         void SetCost(float pcost){mCost = pcost;}

         void AddEffect(Effect* pEffect){mEffects.push_back(pEffect);}
         void AddInterrupt(Interrupt* pInterrupt){mInterrupts.push_back(pInterrupt);}

         bool CheckInterrupts(const WorldState* pCurrent) const
         {
            InterruptList::const_iterator iter = mInterrupts.begin();
            InterruptList::const_iterator endOfList = mInterrupts.end();
            while(iter != endOfList)
            {
               if((*iter)->Evaluate(pCurrent))
               {
                  return false;
               }
               ++iter;
            }
            return true;
         }

         bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const
         {
            EffectList::const_iterator iter = mEffects.begin();
            EffectList::const_iterator endOfList = mEffects.end();
            while(iter != endOfList)
            {
               (*iter)->Apply(pCurrent, pWSIn);
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

 
   typedef TOperator<bool> NPCOperator;


}//namespace dtAI

#endif // __DELTA_PLANNERASTARUTILS_H__

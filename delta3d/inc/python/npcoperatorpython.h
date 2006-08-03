#ifndef __NPC_OPERATOR_PYTHON_H__
#define __NPC_OPERATOR_PYTHON_H__

#include <dtAI/conditional.h>
#include <dtAI/worldstate.h>
#include <dtAI/npcoperator.h>

#include <dtUtil/functor.h>

#include <vector>
#include <string>

namespace dtAI
{


struct NPCOperatorApplyFunctor_python
{
   virtual bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const = 0;
};


struct NPCOperatorPython
{
   typedef std::vector<IConditional*> ConditionalList;

   NPCOperatorPython(const std::string& pName, NPCOperatorApplyFunctor_python* pFunc)
      : mApplyFunctor(pFunc)
      , mOperator(new NPCOperator(pName, NPCOperator::ApplyOperatorFunctor(mApplyFunctor, &NPCOperatorApplyFunctor_python::Apply)))
   {

   }

   bool operator==(const NPCOperator& pRHS) const
   {
      return *mOperator == pRHS;
   }

   const std::string& GetName() const
   {
      return mOperator->GetName();
   }

   bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const
   {
      return mOperator->Apply(pCurrent, pWSIn);
   }

   void AddPreCondition(IConditional* pCondIn)
   {
      return mOperator->AddPreCondition(pCondIn);
   }

   void AddInterrupt(IConditional* pCondIn)
   {
      return mOperator->AddInterrupt(pCondIn);
   }

   void RemovePreCondition(IConditional* pConditional)
   {
      mOperator->RemovePreCondition(pConditional);
   }

   void RemoveInterrupt(IConditional* pConditional)
   {
      mOperator->RemoveInterrupt(pConditional);
   }

   ConditionalList GetPreConditions() const
   {
      return ConditionalList(mOperator->GetPreConditions().begin(), mOperator->GetPreConditions().end());
   }

   ConditionalList GetInterrupts() const
   {
      return ConditionalList(mOperator->GetInterrupts().begin(), mOperator->GetInterrupts().end());
   }

   NPCOperator* GetOperator()
   {
      return mOperator;
   }

private:
   NPCOperatorApplyFunctor_python* mApplyFunctor;
   NPCOperator* mOperator;
};


}//namespace dtAI

#endif//__NPC_OPERATOR_PYTHON_H__

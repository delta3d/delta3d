#ifndef __PLANNER_HELPER_PYTHON_H__
#define __PLANNER_HELPER_PYTHON_H__

#include <dtAI/plannerhelper.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>
#include <dtUtil/functor.h>

#include <python/npcoperatorpython.h>

#include <vector>
#include <iostream>

namespace dtAI
{


struct DesiredStateFunctor_python
{
   virtual bool IsDesiredState(const WorldState*) const = 0;
};

struct RemainingCostFunctor_python
{
   virtual float RemainingCost(const WorldState*) const = 0;
};



struct PlannerHelperPython: PlannerHelper
{

   typedef std::vector<NPCOperator*> HelperOperators;

   PlannerHelperPython(RemainingCostFunctor_python* pRemFunc, DesiredStateFunctor_python* pDesFunc )
      : mRemainingCostFunctor(pRemFunc)
      , mDesiredStateFunctor(pDesFunc)
   {
      pDesFunc->IsDesiredState(0);
   }

   PlannerHelperPython::~PlannerHelperPython()
   {
   }

   void AddOperator(NPCOperatorPython* pOperator){PlannerHelper::AddOperator(pOperator->GetOperator());}
   void RemoveOperator(NPCOperatorPython* pOperator){PlannerHelper::RemoveOperator(pOperator->GetOperator());}

   HelperOperators GetOperators() const{return HelperOperators(PlannerHelper::GetOperators().begin(), PlannerHelper::GetOperators().end());}

   void SetCurrentState(const WorldState& pNewState){PlannerHelper::SetCurrentState(pNewState);}

   WorldState* GetCurrentState(){return PlannerHelper::GetCurrentState();}

   void SetRemainingCostFunc(RemainingCostFunctor_python* pFunc)
   {
      mRemainingCostFunctor = pFunc;
   }
   void SetDesiredStateFunc(DesiredStateFunctor_python* pFunc)
   {
      mDesiredStateFunctor = pFunc;
   }

   float RemainingCost(const WorldState* pWS) const
   {
      std::cout << "RemainingCost PLannerHelperPython" << std::endl;
      return mRemainingCostFunctor->RemainingCost(pWS);
   }

   bool IsDesiredState(const WorldState* pWS) const 
   {
      std::cout << "IsDesiredState PLannerHelperPython" << std::endl;
      return mDesiredStateFunctor->IsDesiredState(pWS);
   }


private:
   RemainingCostFunctor_python* mRemainingCostFunctor;
   DesiredStateFunctor_python* mDesiredStateFunctor;
};


}//namespace dtAI

#endif

#ifndef __PLANNER_HELPER_PYTHON_H__
#define __PLANNER_HELPER_PYTHON_H__

#include <dtAI/plannerhelper.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>
#include <dtUtil/functor.h>

#include <python/npcoperatorpython.h>

#include <vector>

namespace dtAI
{


struct DesiredStateFunctor_python
{
   virtual bool IsDesiredState(const WorldState*) = 0;
};

struct RemainingCostFunctor_python
{
   virtual float RemainingCost(const WorldState*) = 0;
};



struct PlannerHelperPython
{

   typedef std::vector<NPCOperator*> HelperOperators;

   PlannerHelperPython(RemainingCostFunctor_python* pRemFunc, DesiredStateFunctor_python* pDesFunc )
      : mRemainingCostFunctor(pRemFunc)
      , mDesiredStateFunctor(pDesFunc)
      , mHelper(new PlannerHelper(PlannerHelper::RemainingCostFunctor(mRemainingCostFunctor, &RemainingCostFunctor_python::RemainingCost), PlannerHelper::DesiredStateFunctor(mDesiredStateFunctor, &DesiredStateFunctor_python::IsDesiredState)) )
   {

   }


   void AddOperator(NPCOperatorPython* pOperator){mHelper->AddOperator(pOperator->GetOperator());}
   void RemoveOperator(NPCOperatorPython* pOperator){mHelper->RemoveOperator(pOperator->GetOperator());}

   HelperOperators GetOperators() const{return HelperOperators(mHelper->GetOperators().begin(), mHelper->GetOperators().end());}

   void SetCurrentState(const WorldState& pNewState){mHelper->SetCurrentState(pNewState);}

   WorldState* GetCurrentState(){return mHelper->GetCurrentState();}

   void SetRemainingCostFunc(RemainingCostFunctor_python* pFunc)
   {
      mRemainingCostFunctor = pFunc;
      mHelper->SetRemainingCostFunc(PlannerHelper::RemainingCostFunctor(mRemainingCostFunctor, &RemainingCostFunctor_python::RemainingCost));
   }
   void SetDesiredStateFunc(DesiredStateFunctor_python* pFunc)
   {
      mDesiredStateFunctor = pFunc;
      mHelper->SetDesiredStateFunc(PlannerHelper::DesiredStateFunctor(mDesiredStateFunctor, DesiredStateFunctor_python::IsDesiredState));
   }

   float RemainingCost(const WorldState* pWS) const
   {
      return mHelper->RemainingCost(pWS);
   }

   bool IsDesiredState(const WorldState* pWS) const
   {
      return mHelper->IsDesiredState(pWS);
   }

   PlannerHelper* GetHelper()
   {
      return mHelper;
   }


private:
   RemainingCostFunctor_python* mRemainingCostFunctor;
   DesiredStateFunctor_python* mDesiredStateFunctor;
   PlannerHelper* mHelper;
};


}//namespace dtAI

#endif

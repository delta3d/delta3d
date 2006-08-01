#include <dtAI/plannerhelper.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>
#include <dtUtil/functor.h>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace dtAI;

struct DesiredStateFunctor_python
{
   virtual bool IsDesiredState(const WorldState*) = 0;
};

struct RemainingCostFunctor_python
{
   virtual float RemainingCost(const WorldState*) = 0;
};

struct DesiredStateWrap: DesiredStateFunctor_python, wrapper<DesiredStateFunctor_python>
{
   bool IsDesiredState(const WorldState* pWS)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<bool>( this->get_override("IsDesiredState").ptr(), pWS);
      #else
         return this->get_override("IsDesiredState")(pWS);
      #endif
   }
};

struct RemainingCostWrap: RemainingCostFunctor_python, wrapper<RemainingCostFunctor_python>
{
   float RemainingCost(const WorldState* pWS)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<float>( this->get_override("RemainingCost").ptr(), pWS);
      #else
         return this->get_override("RemainingCost")(pWS);
      #endif
   }
};

struct PlannerHelperPython
{

   typedef std::vector<NPCOperator*> HelperOperators;
   
   PlannerHelperPython(RemainingCostFunctor_python* pRemFunc, DesiredStateFunctor_python* pDesFunc )
      : mRemainingCostFunctor(pRemFunc)
      , mDesiredStateFunctor(pDesFunc)
      , mHelper(PlannerHelper::RemainingCostFunctor(mRemainingCostFunctor, &RemainingCostFunctor_python::RemainingCost), PlannerHelper::DesiredStateFunctor(mDesiredStateFunctor, &DesiredStateFunctor_python::IsDesiredState) )
   {

   }


   void AddOperator(NPCOperator* pOperator){mHelper.AddOperator(pOperator);}
   void RemoveOperator(NPCOperator* pOperator){mHelper.RemoveOperator(pOperator);}
   
   HelperOperators GetOperators() const{return HelperOperators(mHelper.GetOperators().begin(), mHelper.GetOperators().end());}

   void SetCurrentState(const WorldState& pNewState){mHelper.SetCurrentState(pNewState);}

   WorldState* GetCurrentState(){return mHelper.GetCurrentState();}

   void SetRemainingCostFunc(RemainingCostFunctor_python* pFunc)
   {
      mRemainingCostFunctor = pFunc;
      mHelper.SetRemainingCostFunc(PlannerHelper::RemainingCostFunctor(mRemainingCostFunctor, &RemainingCostFunctor_python::RemainingCost));
   }
   void SetDesiredStateFunc(DesiredStateFunctor_python* pFunc)
   {
      mDesiredStateFunctor = pFunc;
      mHelper.SetDesiredStateFunc(PlannerHelper::DesiredStateFunctor(mDesiredStateFunctor, DesiredStateFunctor_python::IsDesiredState));
   }

   float RemainingCost(const WorldState* pWS) const
   {
      return mHelper.RemainingCost(pWS);
   }

   bool IsDesiredState(const WorldState* pWS) const
   {
      return mHelper.IsDesiredState(pWS);
   }


private:
   RemainingCostFunctor_python* mRemainingCostFunctor;
   DesiredStateFunctor_python* mDesiredStateFunctor;
   PlannerHelper mHelper;
};


void init_PlannerHelperBindings()
{   

   class_<DesiredStateWrap, boost::noncopyable>("DesiredStateFunctor")
      .def("IsDesiredState", pure_virtual(&DesiredStateFunctor_python::IsDesiredState));


   class_<RemainingCostWrap, boost::noncopyable>("RemainingCostFunctor")
      .def("RemainingCost", pure_virtual(&RemainingCostFunctor_python::RemainingCost));


   scope helper = class_<PlannerHelperPython, PlannerHelperPython*>("PlannerHelper", init<RemainingCostFunctor_python*, DesiredStateFunctor_python*>() )
      .def("AddOperator", &PlannerHelperPython::AddOperator)
      .def("RemoveOperator", &PlannerHelperPython::RemoveOperator)
      .def("GetOperators", &PlannerHelperPython::GetOperators)      
      .def("SetCurrentState", &PlannerHelperPython::SetCurrentState)
      .def("GetCurrentState", &PlannerHelperPython::GetCurrentState, return_value_policy<reference_existing_object>())
      .def("SetRemainingCostFunc", &PlannerHelperPython::SetRemainingCostFunc)
      .def("SetDesiredStateFunc", &PlannerHelperPython::SetDesiredStateFunc)
      .def("RemainingCost", &PlannerHelperPython::RemainingCost)
      .def("IsDesiredState", &PlannerHelperPython::IsDesiredState)
      ;

   class_<PlannerHelperPython::HelperOperators>("OperatorList")
      .def(vector_indexing_suite<PlannerHelperPython::HelperOperators>());

}

#include <dtAI/plannerhelper.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>
#include <dtUtil/functor.h>

#include <python/npcoperatorpython.h>
#include <python/plannerhelperpython.h>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace dtAI;


struct DesiredStateWrap: DesiredStateFunctor_python, wrapper<DesiredStateFunctor_python>
{
   bool IsDesiredState(const WorldState* pWS) const
   {
      std::cout << "DesiredStateWrap::IsDesiredState" << std::endl;
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<bool>( this->get_override("IsDesiredState").ptr(), pWS);
      #else
         return this->get_override("IsDesiredState")(pWS);
      #endif
   }
};

struct RemainingCostWrap: RemainingCostFunctor_python, wrapper<RemainingCostFunctor_python>
{
   float RemainingCost(const WorldState* pWS) const   
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<float>( this->get_override("RemainingCost").ptr(), pWS);
      #else
         return this->get_override("RemainingCost")(pWS);
      #endif
   }
};



void init_PlannerHelperBindings()
{   

   class_<DesiredStateWrap, boost::noncopyable>("DesiredStateFunctor")
      .def("IsDesiredState", pure_virtual(&DesiredStateFunctor_python::IsDesiredState));


   class_<RemainingCostWrap, boost::noncopyable>("RemainingCostFunctor")
      .def("RemainingCost", pure_virtual(&RemainingCostFunctor_python::RemainingCost));

   class_<PlannerHelper, boost::noncopyable>("PlannerHelperBase");

   scope helper = class_<PlannerHelperPython, bases<PlannerHelper>, boost::noncopyable>("PlannerHelper", init<RemainingCostFunctor_python*, DesiredStateFunctor_python*>()[with_custodian_and_ward<1,2, with_custodian_and_ward<1,3> >()])
      .def("AddOperator", &PlannerHelperPython::AddOperator, with_custodian_and_ward<1,2>())
      .def("RemoveOperator", &PlannerHelperPython::RemoveOperator)
      .def("GetOperators", &PlannerHelperPython::GetOperators)      
      .def("SetCurrentState", &PlannerHelperPython::SetCurrentState)
      .def("GetCurrentState", &PlannerHelperPython::GetCurrentState, return_internal_reference<>())
      .def("SetRemainingCostFunc", &PlannerHelperPython::SetRemainingCostFunc)
      .def("SetDesiredStateFunc", &PlannerHelperPython::SetDesiredStateFunc)
      ;

   class_<PlannerHelperPython::HelperOperators>("OperatorList")
      .def(vector_indexing_suite<PlannerHelperPython::HelperOperators>());

}

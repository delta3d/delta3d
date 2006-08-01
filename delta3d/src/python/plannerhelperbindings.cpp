#include <dtAI/plannerhelper.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace dtAI;

WorldState* (PlannerHelper::*MyGetState)() = &PlannerHelper::GetCurrentState;

void init_PlannerHelperBindings()
{   

   class_<PlannerHelper>("PlannerHelper")
      .def("AddOperator", &PlannerHelper::AddOperator)
      .def("RemoveOperator", &PlannerHelper::RemoveOperator)
      .def("SetCurrentState", &PlannerHelper::SetCurrentState)
      .def("GetCurrentState", MyGetState, return_internal_reference<>())
      ;

}

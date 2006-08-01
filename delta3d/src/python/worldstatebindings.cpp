#include <dtAI/statevariable.h>
#include <dtAI/worldstate.h>

#include <boost/python.hpp>
#include <string>

using namespace boost::python;
using namespace dtAI;

IStateVariable* (WorldState::*GetMyState)(const std::string&) = &WorldState::GetState;

void init_WorldStateBindings()
{   

   class_<WorldState>("WorldState")
      .def("GetCost", &WorldState::GetCost)
      .def("AddCost", &WorldState::AddCost)
      .def("AddState", &WorldState::AddState)
      .def("GetState", GetMyState, return_value_policy<reference_existing_object>())
      ;

}

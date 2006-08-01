#include <dtAI/conditional.h>
#include <dtAI/worldstate.h>
#include <dtAI/npcoperator.h>
#include <string>

#include <boost/python.hpp>

using namespace boost::python;
using namespace dtAI;


void init_NPCOperatorBindings()
{   

   class_<NPCOperator, boost::noncopyable>("NPCOperator")
      .def("GetName", &NPCOperator::GetName, return_value_policy<reference_existing_object>())
      .def("AddPreCondition", &NPCOperator::AddPreCondition)
      .def("AddInterrupt", &NPCOperator::AddInterrupt)
      .def("RemovePreCondition", &NPCOperator::RemovePreCondition)
      .def("RemoveInterrupt", &NPCOperator::RemoveInterrupt)
      ;

}

#include <dtAI/conditional.h>
#include <dtAI/worldstate.h>
#include <dtAI/npcoperator.h>

#include <python/npcoperatorpython.h>

#include <string>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace dtAI;


struct NPCOperatorApplyWrap: NPCOperatorApplyFunctor_python, wrapper<NPCOperatorApplyFunctor_python>
{
   bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const 
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<bool>( this->get_override("Apply").ptr(), pCurrent, pWSIn);
      #else
         return this->get_override("Apply")(pCurrent, pWSIn);
      #endif
   }
};


void init_NPCOperatorBindings()
{   
   class_<NPCOperatorApplyWrap, boost::noncopyable>("ApplyOperatorFunctor")
      .def("Apply", pure_virtual(&NPCOperatorApplyFunctor_python::Apply));

   scope npcOperator = class_<NPCOperatorPython, boost::noncopyable>("NPCOperator", init<const std::string&, NPCOperatorApplyFunctor_python*>())
      .def("GetName", &NPCOperatorPython::GetName, return_value_policy<reference_existing_object>())
      .def("Apply", &NPCOperatorPython::Apply)
      .def("AddPreCondition", &NPCOperatorPython::AddPreCondition)
      .def("AddInterrupt", &NPCOperatorPython::AddInterrupt)
      .def("RemovePreCondition", &NPCOperatorPython::RemovePreCondition)
      .def("RemoveInterrupt", &NPCOperatorPython::RemoveInterrupt)
      ;

   class_<NPCOperatorPython::ConditionalList>("ConditionalList")
      .def(vector_indexing_suite<NPCOperatorPython::ConditionalList>());
}

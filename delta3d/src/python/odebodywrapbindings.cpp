// odebodywrapbindings.cpp: Base binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/ODEBodyWrap.h>

using namespace boost::python;
using namespace dtCore;

void initODEBodyWrapBindings()
{
	scope odeBodyWrapScope = class_<ODEBodyWrap, boost::noncopyable>("ODEBodyWrap", no_init)
      .def("SetBodyID", &ODEBodyWrap::SetBodyID)
	  //.def("GetBodyID", &ODEBodyWrap::GetBodyID, return_value_policy<return_internal_reference>())
	  //.def("EnableDynamics", &ODEBodyWrap::EnableDynamics)
	  //.def("DynamicsEnabled", &ODEBodyWrap::DynamicsEnabled)
      ;
}

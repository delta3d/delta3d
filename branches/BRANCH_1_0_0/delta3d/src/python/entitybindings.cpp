// entitybindings.cpp: Entity binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "entity.h"

using namespace boost::python;
using namespace dtCore;
using namespace dtHLA;

void initEntityBindings()
{
   Entity* (*EntityGI1)(int) = &Entity::GetInstance;
   Entity* (*EntityGI2)(std::string) = &Entity::GetInstance;

   class_<Entity, bases<Object>, osg::ref_ptr<Entity> >("Entity", init<optional<std::string> >())
      .def("GetInstanceCount", &Entity::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", EntityGI1, return_internal_reference<>())
      .def("GetInstance", EntityGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetEntityIdentifier", &Entity::SetEntityIdentifier)
      .def("GetEntityIdentifier", &Entity::GetEntityIdentifier, return_internal_reference<>())
      .def("SetEntityType", &Entity::SetEntityType)
      .def("GetEntityType", &Entity::GetEntityType, return_internal_reference<>())
      .def("SetWorldLocation", &Entity::SetWorldLocation)
      .def("GetWorldLocation", &Entity::GetWorldLocation, return_internal_reference<>())
      .def("SetOrientation", &Entity::SetOrientation)
      .def("GetOrientation", &Entity::GetOrientation, return_internal_reference<>())
      .def("SetVelocityVector", &Entity::SetVelocityVector)
      .def("GetVelocityVector", &Entity::GetVelocityVector, return_internal_reference<>())
      .def("SetAccelerationVector", &Entity::SetAccelerationVector)
      .def("GetAccelerationVector", &Entity::GetAccelerationVector, return_internal_reference<>())
      .def("SetAngularVelocityVector", &Entity::SetAngularVelocityVector)
      .def("GetAngularVelocityVector", &Entity::GetAngularVelocityVector, return_internal_reference<>())
      .def("SetArticulatedParametersArray", &Entity::SetArticulatedParametersArray)
      .def("GetArticulatedParametersArray", &Entity::GetArticulatedParametersArray, return_internal_reference<>())
      .def("SetDamageState", &Entity::SetDamageState)
      .def("GetDamageState", &Entity::GetDamageState);
}

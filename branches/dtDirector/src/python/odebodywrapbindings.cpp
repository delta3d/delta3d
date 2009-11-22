#include <python/dtpython.h>
#include <dtCore/odebodywrap.h>
#include <dtCore/transform.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(dxBody); // for GetBodyID()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ED_overloads, EnableDynamics, 0, 1)

void initODEBodyWrapBindings()
{
   void (ODEBodyWrap::*SetMass1)(const dMass*) = &ODEBodyWrap::SetMass;
   void (ODEBodyWrap::*SetMass2)(float) = &ODEBodyWrap::SetMass;

   void (ODEBodyWrap::*GetMass1)(dMass*) const = &ODEBodyWrap::GetMass;
   float (ODEBodyWrap::*GetMass2)() const = &ODEBodyWrap::GetMass;

   class_<ODEBodyWrap, bases<>, dtCore::RefPtr<ODEBodyWrap>, boost::noncopyable>("ODEBodyWrap", no_init)
      .def("SetBodyID", &ODEBodyWrap::SetBodyID)
      .def("GetBodyID", &ODEBodyWrap::GetBodyID, return_value_policy<return_opaque_pointer>())
      .def("EnableDynamics", &ODEBodyWrap::EnableDynamics, ED_overloads())
      .def("DynamicsEnabled", &ODEBodyWrap::DynamicsEnabled)
      .def("SetMass", SetMass1)
      .def("GetMass", GetMass1)
      .def("SetMass", SetMass2)
      .def("GetMass", GetMass2)
      .def("SetPosition", &ODEBodyWrap::SetPosition)
      .def("SetRotation", &ODEBodyWrap::SetRotation)
      .def("SetCenterOfGravity", &ODEBodyWrap::SetCenterOfGravity)
      .def("GetCenterOfGravity", &ODEBodyWrap::GetCenterOfGravity)
      .def("SetInertiaTensor", &ODEBodyWrap::SetInertiaTensor)
      .def("GetInertiaTensor", &ODEBodyWrap::GetInertiaTensor)
      .def("GetLinearVelocity", &ODEBodyWrap::GetLinearVelocity)
      .def("GetAngularVelocity", &ODEBodyWrap::GetAngularVelocity)
      .def("ApplyForce", &ODEBodyWrap::ApplyForce)
      .def("ApplyRelForce", &ODEBodyWrap::ApplyRelForce)
      .def("ApplyRelForceAtRelPos", &ODEBodyWrap::ApplyRelForceAtRelPos)
      .def("ApplyTorque", &ODEBodyWrap::ApplyTorque)
      .def("ApplyRelTorque", &ODEBodyWrap::ApplyRelTorque)
      .def("GetBodyTransform", &ODEBodyWrap::GetBodyTransform)
      .def("UpdateBodyTransform", &ODEBodyWrap::UpdateBodyTransform)
      ;
}


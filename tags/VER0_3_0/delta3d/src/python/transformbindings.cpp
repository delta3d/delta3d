// transformbindings.cpp: Transform binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "transform.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EE_overloads, EpsilonEquals, 1, 2)

void initTransformBindings()
{
   void (Transform::*Set1)(sgMat4) = &Transform::Set;
   void (Transform::*Set2)(float, float, float, float, float, float) = &Transform::Set;
   void (Transform::*Set3)(sgVec3, sgVec3) = &Transform::Set;

   void (Transform::*SetLookAt1)(sgVec3, sgVec3, sgVec3) = &Transform::SetLookAt;
   void (Transform::*SetLookAt2)(float, float, float, float, float, float, float, float, float) = &Transform::SetLookAt;
   
   void (Transform::*SetTranslation1)(float, float, float) = &Transform::SetTranslation;
   void (Transform::*SetTranslation2)(sgVec3) = &Transform::SetTranslation;

   void (Transform::*SetRotation1)(sgVec3) = &Transform::SetRotation;
   void (Transform::*SetRotation2)(float, float, float) = &Transform::SetRotation;
   void (Transform::*SetRotation3)(sgMat4) = &Transform::SetRotation;

   void (Transform::*Get1)(sgVec3, sgVec3) = &Transform::Get;
   void (Transform::*Get2)(sgMat4) = &Transform::Get;
   void (Transform::*Get3)(float*, float*, float*, float*, float*, float*) = &Transform::Get;

   void (Transform::*GetTranslation1)(sgVec3) = &Transform::GetTranslation;
   void (Transform::*GetTranslation2)(float*, float*, float*) = &Transform::GetTranslation;

   void (Transform::*GetRotation1)(float*, float*, float*) = &Transform::GetRotation;
   void (Transform::*GetRotation2)(sgVec3) = &Transform::GetRotation;
   void (Transform::*GetRotation3)(sgMat4) = &Transform::GetRotation;
      
   class_<Transform>("Transform", init<optional<float, float, float, float, float, float> >())
      .def("Set", Set1)
      .def("Set", Set2)
      .def("Set", Set3)
      .def("SetLookAt", SetLookAt1)
      .def("SetLookAt", SetLookAt2)
      .def("SetTranslation", SetTranslation1)
      .def("SetTranslation", SetTranslation2)
      .def("SetRotation", SetRotation1)
      .def("SetRotation", SetRotation2)
      .def("SetRotation", SetRotation3)
      .def("Get", Get1)
      .def("Get", Get2)
      .def("Get", Get3)
      .def("GetTranslation", GetTranslation1)
      .def("GetTranslation", GetTranslation2)
      .def("GetRotation", GetRotation1)
      .def("GetRotation", GetRotation2)
      .def("GetRotation", GetRotation3)
      .def("EpsilonEquals", &Transform::EpsilonEquals, EE_overloads());
}

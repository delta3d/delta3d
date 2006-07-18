// transformbindings.cpp: Transform binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>
#include <dtCore/transform.h>

#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EE_overloads, EpsilonEquals, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(S_overloads, Set, 6, 9)

void initTransformBindings()
{
   void (Transform::*Set1)(float, float, float, float, float, float, float, float, float) = &Transform::Set;
   void (Transform::*Set2)(const osg::Vec3&, const osg::Vec3&, const osg::Vec3&) = &Transform::Set;
   void (Transform::*Set3)(const osg::Vec3&, const osg::Matrix&, const osg::Vec3&) = &Transform::Set;
   void (Transform::*Set4)(const osg::Matrix&) = &Transform::Set;
   void (Transform::*SetTranslation1)(float, float, float) = &Transform::SetTranslation;
   void (Transform::*SetTranslation2)(const osg::Vec3&) = &Transform::SetTranslation;
   void (Transform::*SetRotation1)(float, float, float) = &Transform::SetRotation;
   void (Transform::*SetRotation2)(const osg::Vec3&) = &Transform::SetRotation;
   void (Transform::*SetRotation3)(const osg::Matrix&) = &Transform::SetRotation;
   void (Transform::*SetScale1)(float, float, float) = &Transform::SetScale;
   void (Transform::*SetScale2)(const osg::Vec3&) = &Transform::SetScale;

   void (Transform::*Get1)(osg::Vec3&, osg::Vec3&, osg::Vec3&) const = &Transform::Get;
   void (Transform::*Get2)(osg::Vec3&, osg::Matrix&, osg::Vec3&) const = &Transform::Get;
   void (Transform::*Get3)(osg::Matrix&) const = &Transform::Get;
   void (Transform::*GetTranslation)(osg::Vec3&) const = &Transform::GetTranslation;
   void (Transform::*GetRotation1)(osg::Vec3&) const = &Transform::GetRotation;
   void (Transform::*GetRotation2)(osg::Matrix&) const = &Transform::GetRotation;
   void (Transform::*GetScale)(osg::Vec3&) const = &Transform::GetScale;

   void (Transform::*SetLookAt1)(const osg::Vec3&, const osg::Vec3&, const osg::Vec3&) = &Transform::SetLookAt;
   void (Transform::*SetLookAt2)(float, float, float, float, float, float, float, float, float) = &Transform::SetLookAt;

   class_<Transform>("Transform", init<optional<float, float, float, float, float, float, float, float, float> >())
      .def("Set", Set1, S_overloads())
      .def("Set", Set2)
      .def("Set", Set3)
      .def("Set", Set4)
      .def("SetTranslation", SetTranslation1)
      .def("SetTranslation", SetTranslation2)
      .def("SetRotation", SetRotation1)
      .def("SetRotation", SetRotation2)
      .def("SetRotation", SetRotation3)
      .def("SetScale", SetScale1)
      .def("SetScale", SetScale2)
      .def("Get", Get1)
      .def("Get", Get2)
      .def("Get", Get3)
      .def("GetTranslation", GetTranslation)
      .def("GetRotation", GetRotation1)
      .def("GetRotation", GetRotation2)
      .def("GetScale", GetScale)
      .def("SetLookAt", SetLookAt1)
      .def("SetLookAt", SetLookAt2)
      .def("EpsilonEquals", &Transform::EpsilonEquals, EE_overloads());
}

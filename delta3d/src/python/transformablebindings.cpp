// transformablebindings.cpp: Transformable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/transformable.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ST_overloads, SetTransform, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GT_overloads, GetTransform, 1, 2)

void initTransformableBindings()
{
   Transformable* (*TransformableGI1)(int) = &Transformable::GetInstance;
   Transformable* (*TransformableGI2)(std::string) = &Transformable::GetInstance;

   scope Transformable_scope = class_<Transformable, bases<Base>, osg::ref_ptr<Transformable> >("Transformable", no_init)
      .def("GetInstanceCount", &Transformable::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TransformableGI1, return_internal_reference<>())
      .def("GetInstance", TransformableGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("CanBeChild", &Transformable::CanBeChild)
      .def("AddChild", &Transformable::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &Transformable::RemoveChild)
      .def("GetNumChildren", &Transformable::GetNumChildren)
      .def("GetChild", &Transformable::GetChild, return_internal_reference<>())
      .def("GetParent", &Transformable::GetParent, return_internal_reference<>())
      .def("SetTransform", &Transformable::SetTransform, ST_overloads())
      .def("GetTransform", &Transformable::GetTransform, GT_overloads());

   enum_<Transformable::CoordSysEnum>("CoordSysEnum")
      .value("REL_CS", Transformable::REL_CS)
      .value("ABS_CS", Transformable::ABS_CS)
      .export_values();
}

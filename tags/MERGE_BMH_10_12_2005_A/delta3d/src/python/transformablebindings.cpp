// transformablebindings.cpp: Transformable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ST_overloads, SetTransform, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GT_overloads, GetTransform, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RPN_overloads, RenderProxyNode, 0, 1)

void initTransformableBindings()
{
   Transformable* (*TransformableGI1)(int) = &Transformable::GetInstance;
   Transformable* (*TransformableGI2)(std::string) = &Transformable::GetInstance;

   scope Transformable_scope = class_<Transformable, bases<DeltaDrawable>, dtCore::RefPtr<Transformable> >("Transformable", no_init)
      .def("GetInstanceCount", &Transformable::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TransformableGI1, return_internal_reference<>())
      .def("GetInstance", TransformableGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddChild", &Transformable::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &Transformable::RemoveChild)
      .def("SetTransform", &Transformable::SetTransform, ST_overloads())
      .def("GetTransform", &Transformable::GetTransform, GT_overloads())
      .def("RenderProxyNode", &Transformable::RenderProxyNode, RPN_overloads())
      .def("SetNormalRescaling", &Transformable::SetNormalRescaling)
      .def("GetNormalRescaling", &Transformable::GetNormalRescaling);

   enum_<Transformable::CoordSysEnum>("CoordSysEnum")
      .value("REL_CS", Transformable::REL_CS)
      .value("ABS_CS", Transformable::ABS_CS)
      .export_values();
}

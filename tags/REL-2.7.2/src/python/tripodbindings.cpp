// tripodbindings.cpp: Tripod binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/tripod.h>
#include <dtCore/camera.h>

using namespace boost::python;
using namespace dtCore;

void initTripodBindings()
{
   Tripod* (*TripodGI1)(int) = &Tripod::GetInstance;
   Tripod* (*TripodGI2)(std::string) = &Tripod::GetInstance;

   void (Tripod::*SetChild1)(Transformable*) = &Tripod::SetChild;
   void (Tripod::*SetChild2)(const std::string&) = &Tripod::SetChild;

   Transformable* (Tripod::*GetChild1)() = &Tripod::GetChild;

   void (Tripod::*SetAttachToTransformable1)(Transformable*) = &Tripod::SetAttachToTransformable;
   void (Tripod::*SetAttachToTransformable2)(const std::string&) = &Tripod::SetAttachToTransformable;

   Transformable* (Tripod::*GetAttachedTransformable1)() = &Tripod::GetAttachedTransformable;

   void (Tripod::*SetOffset1)(float, float, float, float, float, float) = &Tripod::SetOffset;
   void (Tripod::*SetOffset2)(const osg::Vec3&, const osg::Vec3&) = &Tripod::SetOffset;

   void (Tripod::*GetOffset1)(osg::Vec3&, osg::Vec3&) const = &Tripod::GetOffset;

   void (Tripod::*SetScale1)(float, float, float, float, float, float) = &Tripod::SetScale;
   void (Tripod::*SetScale2)(const osg::Vec3&, const osg::Vec3&) = &Tripod::SetScale;

   void (Tripod::*GetScale1)(osg::Vec3&, osg::Vec3&) = &Tripod::GetScale;

   Transformable* (Tripod::*GetLookAtTarget1)() = &Tripod::GetLookAtTarget;

   scope Tripod_scope = class_<Tripod, bases<Transformable>, dtCore::RefPtr<Tripod>, boost::noncopyable >("Tripod", init<optional<Camera*, Transformable*> >())
      .def("GetInstanceCount", &Tripod::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TripodGI1, return_internal_reference<>())
      .def("GetInstance", TripodGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetChild", SetChild1)
      .def("SetChild", SetChild2)
      .def("GetChild", GetChild1, return_internal_reference<>())
      .def("SetAttachToTransformable", SetAttachToTransformable1)
      .def("SetAttachToTransformable", SetAttachToTransformable2)
      .def("GetAttachedTransformable", GetAttachedTransformable1, return_internal_reference<>())
      .def("SetOffset", SetOffset1)
      .def("SetOffset", SetOffset2)
      .def("GetOffset", GetOffset1)
      .def("SetScale", SetScale1)
      .def("SetScale", SetScale2)
      .def("GetScale", GetScale1)
      .def("SetTetherMode", &Tripod::SetTetherMode)
      .def("GetTetherMode", &Tripod::GetTetherMode)
      .def("SetLookAtTarget", &Tripod::SetLookAtTarget)
      .def("GetLookAtTarget", GetLookAtTarget1, return_internal_reference<>())
      ;

   enum_<Tripod::TetherMode>("TetherMode")
      .value("TETHER_PARENT_REL", Tripod::TETHER_PARENT_REL)
      .value("TETHER_WORLD_REL", Tripod::TETHER_WORLD_REL)
      .export_values();
}

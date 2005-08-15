// tripodbindings.cpp: Tripod binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/tripod.h"

using namespace boost::python;
using namespace dtCore;

void initTripodBindings()
{
   Tripod* (*TripodGI1)(int) = &Tripod::GetInstance;
   Tripod* (*TripodGI2)(std::string) = &Tripod::GetInstance;

   void (Tripod::*SetCamera1)(Camera*) = &Tripod::SetCamera;
   void (Tripod::*SetCamera2)(const std::string&) = &Tripod::SetCamera;
   
   void (Tripod::*SetAttachToTransformable1)(Transformable*) = &Tripod::SetAttachToTransformable;
   void (Tripod::*SetAttachToTransformable2)(const std::string&) = &Tripod::SetAttachToTransformable;

   void (Tripod::*SetOffset1)(float, float, float, float, float, float) = &Tripod::SetOffset;
   void (Tripod::*SetOffset2)(const osg::Vec3&, const osg::Vec3&) = &Tripod::SetOffset;
   void (Tripod::*SetOffset3)(sgCoord*) = &Tripod::SetOffset;

   void (Tripod::*GetOffset1)(osg::Vec3&, osg::Vec3&) const = &Tripod::GetOffset;
   void (Tripod::*GetOffset2)(sgCoord*) = &Tripod::GetOffset;
   
   void (Tripod::*SetScale1)(float, float, float, float, float, float) = &Tripod::SetScale;
   void (Tripod::*SetScale2)(const osg::Vec3&, const osg::Vec3&) = &Tripod::SetScale;
   void (Tripod::*SetScale3)(sgVec3, sgVec3) = &Tripod::SetScale;

   void (Tripod::*GetScale1)(osg::Vec3&, osg::Vec3&) = &Tripod::GetScale;
   void (Tripod::*GetScale2)(sgVec3, sgVec3) = &Tripod::GetScale;
   
   scope Tripod_scope = class_<Tripod, bases<Transformable>, dtCore::RefPtr<Tripod> >("Tripod", init<optional<Camera*, Transformable*> >())
      .def("GetInstanceCount", &Tripod::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TripodGI1, return_internal_reference<>())
      .def("GetInstance", TripodGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetCamera", SetCamera1)
      .def("SetCamera", SetCamera2)
      .def("GetCamera", &Tripod::GetCamera)
      .def("SetAttachToTransformable", SetAttachToTransformable1)
      .def("SetAttachToTransformable", SetAttachToTransformable2)
      .def("GetAttachedTransformable", &Tripod::GetAttachedTransformable)
      .def("SetOffset", SetOffset1)
      .def("SetOffset", SetOffset2)
      .def("SetOffset", SetOffset3)
      .def("GetOffset", GetOffset1)
      .def("GetOffset", GetOffset2)
      .def("SetScale", SetScale1)
      .def("SetScale", SetScale2)
      .def("SetScale", SetScale3)
      .def("GetScale", GetScale1)
      .def("GetScale", GetScale2)
      .def("SetTetherMode", &Tripod::SetTetherMode)
      .def("GetTetherMode", &Tripod::GetTetherMode)
      .def("SetLookAtTarget", &Tripod::SetLookAtTarget);
      
   enum_<Tripod::TetherMode>("TetherMode")
      .value("TETHER_PARENT_REL", Tripod::TETHER_PARENT_REL)
      .value("TETHER_WORLD_REL", Tripod::TETHER_WORLD_REL)
      .export_values();
}

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
   void (Tripod::*SetCamera2)(const std::string) = &Tripod::SetCamera;
   
   void (Tripod::*SetAttachToTransformable1)(Transformable*) = &Tripod::SetAttachToTransformable;
   void (Tripod::*SetAttachToTransformable2)(const std::string) = &Tripod::SetAttachToTransformable;

   void (Tripod::*SetOffset1)(float, float, float, float, float, float) = &Tripod::SetOffset;
   void (Tripod::*SetOffset2)(sgCoord*) = &Tripod::SetOffset;
   
   void (Tripod::*SetScale1)(float, float, float, float, float, float) = &Tripod::SetScale;
   void (Tripod::*SetScale2)(sgVec3, sgVec3) = &Tripod::SetScale;
   
   scope Tripod_scope = class_<Tripod, bases<Base>, osg::ref_ptr<Tripod> >("Tripod", init<optional<Camera*, Transformable*> >())
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
      .def("GetOffset", &Tripod::GetOffset)
      .def("SetScale", SetScale1)
      .def("SetScale", SetScale2)
      .def("GetScale", &Tripod::GetScale)
      .def("SetTetherMode", &Tripod::SetTetherMode)
      .def("GetTetherMode", &Tripod::GetTetherMode)
      .def("SetLookAtTarget", &Tripod::SetLookAtTarget);
      
   enum_<Tripod::TetherMode>("TetherMode")
      .value("TETHER_PARENT_REL", Tripod::TETHER_PARENT_REL)
      .value("TETHER_WORLD_REL", Tripod::TETHER_WORLD_REL)
      .export_values();
}

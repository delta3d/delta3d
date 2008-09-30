// motionmodelbindings.cpp: MotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/motionmodel.h>
#include <dtCore/transformable.h>

using namespace boost::python;
using namespace dtCore;

void initMotionModelBindings()
{
   MotionModel* (*MotionModelGI1)(int) = &MotionModel::GetInstance;
   MotionModel* (*MotionModelGI2)(std::string) = &MotionModel::GetInstance;

   Transformable* (MotionModel::*GetTarget1)() = &MotionModel::GetTarget;
   const Transformable* (MotionModel::*GetTarget2)() const = &MotionModel::GetTarget;

   class_<MotionModel, bases<Base>, dtCore::RefPtr<MotionModel> >("MotionModel", init<optional<std::string> >())
      .def("GetInstanceCount", &MotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", MotionModelGI1, return_internal_reference<>())
      .def("GetInstance", MotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetTarget", &MotionModel::SetTarget)
      .def("GetTarget", GetTarget1, return_internal_reference<>())
      .def("GetTarget", GetTarget2, return_internal_reference<>())
      .def("SetEnabled", &MotionModel::SetEnabled)
      .def("IsEnabled", &MotionModel::IsEnabled)
      ;
}

// ufomotionmodelbindings.cpp: UFOMotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/transformable.h>

using namespace boost::python;
using namespace dtCore;

void initUFOMotionModelBindings()
{
   UFOMotionModel* (*UFOMotionModelGI1)(int) = &UFOMotionModel::GetInstance;
   UFOMotionModel* (*UFOMotionModelGI2)(std::string) = &UFOMotionModel::GetInstance;

   class_<UFOMotionModel, bases<MotionModel>, dtCore::RefPtr<UFOMotionModel> >("UFOMotionModel", init<optional<Keyboard*, Mouse*> >())
      .def("GetInstanceCount", &UFOMotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", UFOMotionModelGI1, return_internal_reference<>())
      .def("GetInstance", UFOMotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetDefaultMappings", &UFOMotionModel::SetDefaultMappings)
      .def("SetFlyForwardBackwardAxis", &UFOMotionModel::SetFlyForwardBackwardAxis)
      .def("GetFlyForwardBackwardAxis", &UFOMotionModel::GetFlyForwardBackwardAxis, return_internal_reference<>())
      .def("SetFlyLeftRightAxis", &UFOMotionModel::SetFlyLeftRightAxis)
      .def("GetFlyLeftRightAxis", &UFOMotionModel::GetFlyLeftRightAxis, return_internal_reference<>())
      .def("SetFlyUpDownAxis", &UFOMotionModel::SetFlyUpDownAxis)
      .def("GetFlyUpDownAxis", &UFOMotionModel::GetFlyUpDownAxis, return_internal_reference<>())
      .def("SetTurnLeftRightAxis", &UFOMotionModel::SetTurnLeftRightAxis)
      .def("GetTurnLeftRightAxis", &UFOMotionModel::GetTurnLeftRightAxis, return_internal_reference<>())
      .def("SetMaximumFlySpeed", &UFOMotionModel::SetMaximumFlySpeed)
      .def("GetMaximumFlySpeed", &UFOMotionModel::GetMaximumFlySpeed)
      .def("SetMaximumTurnSpeed", &UFOMotionModel::SetMaximumTurnSpeed)
      .def("GetMaximumTurnSpeed", &UFOMotionModel::GetMaximumTurnSpeed);
}

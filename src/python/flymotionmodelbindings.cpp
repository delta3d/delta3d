// flymotionmodelbindings.cpp: FlyMotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/inputdevice.h>
#include <dtCore/keyboard.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/transformable.h>

using namespace boost::python;
using namespace dtCore;

void initFlyMotionModelBindings()
{
   FlyMotionModel* (*FlyMotionModelGI1)(int) = &FlyMotionModel::GetInstance;
   FlyMotionModel* (*FlyMotionModelGI2)(std::string) = &FlyMotionModel::GetInstance;

   Axis* (FlyMotionModel::*GetTurnUpDownAxis1)() = &FlyMotionModel::GetTurnUpDownAxis;
   const Axis* (FlyMotionModel::*GetTurnUpDownAxis2)() const = &FlyMotionModel::GetTurnUpDownAxis;

   Axis* (FlyMotionModel::*GetForwardBackwardAxis1)() = &FlyMotionModel::GetFlyForwardBackwardAxis;
   const Axis* (FlyMotionModel::*GetForwardBackwardAxis2)() const = &FlyMotionModel::GetFlyForwardBackwardAxis;

   Axis* (FlyMotionModel::*GetTurnLeftRightAxis1)() = &FlyMotionModel::GetTurnLeftRightAxis;
   const Axis* (FlyMotionModel::*GetTurnLeftRightAxis2)() const = &FlyMotionModel::GetTurnLeftRightAxis;

   class_<FlyMotionModel, bases<MotionModel>, dtCore::RefPtr<FlyMotionModel> >("FlyMotionModel", init<optional<Keyboard*, Mouse*, FlyMotionModel::BehaviorOptions> >())
      .def("GetInstanceCount", &FlyMotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", FlyMotionModelGI1, return_internal_reference<>())
      .def("GetInstance", FlyMotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetDefaultMappings", &FlyMotionModel::SetDefaultMappings)
      .def("SetFlyForwardBackwardAxis", &FlyMotionModel::SetFlyForwardBackwardAxis)
      .def("GetFlyForwardBackwardAxis", GetForwardBackwardAxis1, return_internal_reference<>())
      .def("GetFlyForwardBackwardAxis", GetForwardBackwardAxis2, return_internal_reference<>())
      .def("SetTurnLeftRightAxis", &FlyMotionModel::SetTurnLeftRightAxis)
      .def("GetTurnLeftRightAxis", GetTurnLeftRightAxis1, return_internal_reference<>())
      .def("GetTurnLeftRightAxis", GetTurnLeftRightAxis2, return_internal_reference<>())
      .def("SetTurnUpDownAxis", &FlyMotionModel::SetTurnUpDownAxis)
      .def("GetTurnUpDownAxis", GetTurnUpDownAxis1, return_internal_reference<>())
      .def("GetTurnUpDownAxis", GetTurnUpDownAxis2, return_internal_reference<>())
      .def("SetMaximumFlySpeed", &FlyMotionModel::SetMaximumFlySpeed)
      .def("GetMaximumFlySpeed", &FlyMotionModel::GetMaximumFlySpeed)
      .def("SetMaximumTurnSpeed", &FlyMotionModel::SetMaximumTurnSpeed)
      .def("GetMaximumTurnSpeed", &FlyMotionModel::GetMaximumTurnSpeed);

   enum_<FlyMotionModel::BehaviorOptions>("BehaviorOptions")
      .value("OPTIONS_NONE", FlyMotionModel::OPTION_NONE)
      .value("OPTION_USE_SIMTIME_FOR_SPEED", FlyMotionModel::OPTION_USE_SIMTIME_FOR_SPEED)
      .value("OPTION_REQUIRE_MOUSE_DOWN", FlyMotionModel::OPTION_REQUIRE_MOUSE_DOWN)
      .value("OPTION_RESET_MOUSE_CURSOR", FlyMotionModel::OPTION_RESET_MOUSE_CURSOR)
      .value("OPTION_USE_CURSOR_KEYS", FlyMotionModel::OPTION_USE_CURSOR_KEYS)
      .value("OPTION_DEFAULT", FlyMotionModel::OPTION_DEFAULT)
      .export_values();
}

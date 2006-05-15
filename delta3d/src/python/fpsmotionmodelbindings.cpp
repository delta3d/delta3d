#include <python/dtpython.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <dtCore/isector.h>

using namespace boost::python;
using namespace dtCore;

void initFPSMotionModelBindings()
{
   FPSMotionModel* (*FPSMotionModelGI1)(int) = &FPSMotionModel::GetInstance;
   FPSMotionModel* (*FPSMotionModelGI2)(std::string) = &FPSMotionModel::GetInstance;

   class_<FPSMotionModel, bases<MotionModel>, RefPtr<FPSMotionModel> >("FPSMotionModel", init<optional<Keyboard*, Mouse*> > ())
      .def("GetInstanceCount", &FPSMotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", FPSMotionModelGI1, return_internal_reference<>())
      .def("GetInstance", FPSMotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetScene", &FPSMotionModel::SetScene)
      .def("GetScene", &FPSMotionModel::GetScene, return_internal_reference<>())
      .def("SetDefaultMappings", &FPSMotionModel::SetDefaultMappings)
      .def("SetWalkForwardBackwardAxis", &FPSMotionModel::SetWalkForwardBackwardAxis)
      .def("GetWalkForwardBackwardAxis", &FPSMotionModel::GetWalkForwardBackwardAxis, return_internal_reference<>())
      .def("SetTurnLeftRightAxis", &FPSMotionModel::SetTurnLeftRightAxis)
      .def("GetTurnLeftRightAxis", &FPSMotionModel::GetTurnLeftRightAxis, return_internal_reference<>())
      .def("SetLookUpDownAxis", &FPSMotionModel::SetLookUpDownAxis)
      .def("GetLookUpDownAxis", &FPSMotionModel::GetLookUpDownAxis, return_internal_reference<>())
      .def("SetSidestepLeftRightAxis", &FPSMotionModel::SetSidestepLeftRightAxis)
      .def("GetSidestepLeftRightAxis", &FPSMotionModel::GetSidestepLeftRightAxis, return_internal_reference<>())
      .def("SetMaximumWalkSpeed", &FPSMotionModel::SetMaximumWalkSpeed)
      .def("GetMaximumWalkSpeed", &FPSMotionModel::GetMaximumWalkSpeed)
      .def("SetMaximumTurnSpeed", &FPSMotionModel::SetMaximumTurnSpeed)
      .def("GetMaximumTurnSpeed", &FPSMotionModel::GetMaximumTurnSpeed)
      .def("SetMaximumSidestepSpeed", &FPSMotionModel::SetMaximumSidestepSpeed)
      .def("GetMaximumSidestepSpeed", &FPSMotionModel::GetMaximumSidestepSpeed)
      .def("SetHeightAboveTerrain", &FPSMotionModel::SetHeightAboveTerrain)
      .def("GetHeightAboveTerrain", &FPSMotionModel::GetHeightAboveTerrain)
      .def("SetMaximumStepUpDistance", &FPSMotionModel::SetMaximumStepUpDistance)
      .def("GetMaximumStepUpDistance", &FPSMotionModel::GetMaximumStepUpDistance);
}

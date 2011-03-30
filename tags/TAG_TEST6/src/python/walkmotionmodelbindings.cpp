// walkmotionmodelbindings.cpp: WalkMotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/walkmotionmodel.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>

using namespace boost::python;
using namespace dtCore;

void initWalkMotionModelBindings()
{
   WalkMotionModel* (*WalkMotionModelGI1)(int) = &WalkMotionModel::GetInstance;
   WalkMotionModel* (*WalkMotionModelGI2)(std::string) = &WalkMotionModel::GetInstance;

   class_<WalkMotionModel, bases<MotionModel>, dtCore::RefPtr<WalkMotionModel> >("WalkMotionModel", init<optional<Keyboard*, Mouse*> >())
      .def("GetInstanceCount", &WalkMotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", WalkMotionModelGI1, return_internal_reference<>())
      .def("GetInstance", WalkMotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetScene", &WalkMotionModel::SetScene)
      .def("GetScene", &WalkMotionModel::GetScene, return_internal_reference<>())
      .def("SetDefaultMappings", &WalkMotionModel::SetDefaultMappings)
      .def("SetWalkForwardBackwardAxis", &WalkMotionModel::SetWalkForwardBackwardAxis)
      .def("GetWalkForwardBackwardAxis", &WalkMotionModel::GetWalkForwardBackwardAxis, return_internal_reference<>())
      .def("SetTurnLeftRightAxis", &WalkMotionModel::SetTurnLeftRightAxis)
      .def("GetTurnLeftRightAxis", &WalkMotionModel::GetTurnLeftRightAxis, return_internal_reference<>())
      .def("SetSidestepLeftRightAxis", &WalkMotionModel::SetSidestepLeftRightAxis)
      .def("GetSidestepLeftRightAxis", &WalkMotionModel::GetSidestepLeftRightAxis, return_internal_reference<>())
      .def("SetMaximumWalkSpeed", &WalkMotionModel::SetMaximumWalkSpeed)
      .def("GetMaximumWalkSpeed", &WalkMotionModel::GetMaximumWalkSpeed)
      .def("SetMaximumTurnSpeed", &WalkMotionModel::SetMaximumTurnSpeed)
      .def("GetMaximumTurnSpeed", &WalkMotionModel::GetMaximumTurnSpeed)
      .def("SetMaximumSidestepSpeed", &WalkMotionModel::SetMaximumSidestepSpeed)
      .def("GetMaximumSidestepSpeed", &WalkMotionModel::GetMaximumSidestepSpeed)
      .def("SetHeightAboveTerrain", &WalkMotionModel::SetHeightAboveTerrain)
      .def("GetHeightAboveTerrain", &WalkMotionModel::GetHeightAboveTerrain)
      .def("SetMaximumStepUpDistance", &WalkMotionModel::SetMaximumStepUpDistance)
      .def("GetMaximumStepUpDistance", &WalkMotionModel::GetMaximumStepUpDistance);
}

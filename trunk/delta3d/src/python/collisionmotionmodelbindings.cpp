#include <python/dtpython.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <dtCore/isector.h>

using namespace boost::python;
using namespace dtCore;

void initCollisionMotionModelBindings()
{
   class_<CollisionMotionModel, bases<MotionModel>, RefPtr<CollisionMotionModel> >("CollisionMotionModel", init<float, float, float, Scene*, Keyboard*, Mouse*>())
      .def("SetDefaultMappings", &CollisionMotionModel::SetDefaultMappings)
      .def("SetWalkForwardBackwardAxis", &CollisionMotionModel::SetWalkForwardBackwardAxis)
      .def("GetWalkForwardBackwardAxis", &CollisionMotionModel::GetWalkForwardBackwardAxis, return_internal_reference<>())
      .def("SetTurnLeftRightAxis", &CollisionMotionModel::SetTurnLeftRightAxis)
      .def("GetTurnLeftRightAxis", &CollisionMotionModel::GetTurnLeftRightAxis, return_internal_reference<>())
      .def("SetLookUpDownAxis", &CollisionMotionModel::SetLookUpDownAxis)
      .def("GetLookUpDownAxis", &CollisionMotionModel::GetLookUpDownAxis, return_internal_reference<>())
      .def("SetSidestepLeftRightAxis", &CollisionMotionModel::SetSidestepLeftRightAxis)
      .def("GetSidestepLeftRightAxis", &CollisionMotionModel::GetSidestepLeftRightAxis, return_internal_reference<>())
      .def("SetMaximumWalkSpeed", &CollisionMotionModel::SetMaximumWalkSpeed)
      .def("GetMaximumWalkSpeed", &CollisionMotionModel::GetMaximumWalkSpeed)
      .def("SetMaximumTurnSpeed", &CollisionMotionModel::SetMaximumTurnSpeed)
      .def("GetMaximumTurnSpeed", &CollisionMotionModel::GetMaximumTurnSpeed)
      .def("GetFPSCollider", &CollisionMotionModel::GetFPSCollider, return_internal_reference<>())
      ;
}

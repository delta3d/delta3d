// flymotionmodelbindings.cpp: FlyMotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "flymotionmodel.h"

using namespace boost::python;
using namespace dtCore;

void initFlyMotionModelBindings()
{
   FlyMotionModel* (*FlyMotionModelGI1)(int) = &FlyMotionModel::GetInstance;
   FlyMotionModel* (*FlyMotionModelGI2)(std::string) = &FlyMotionModel::GetInstance;

   class_<FlyMotionModel, bases<MotionModel>, osg::ref_ptr<FlyMotionModel> >("FlyMotionModel", init<optional<Keyboard*, Mouse*> >())
      .def("GetInstanceCount", &FlyMotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", FlyMotionModelGI1, return_internal_reference<>())
      .def("GetInstance", FlyMotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetDefaultMappings", &FlyMotionModel::SetDefaultMappings)
      .def("SetFlyForwardBackwardAxis", &FlyMotionModel::SetFlyForwardBackwardAxis)
      .def("GetFlyForwardBackwardAxis", &FlyMotionModel::GetFlyForwardBackwardAxis, return_internal_reference<>())
      .def("SetTurnLeftRightAxis", &FlyMotionModel::SetTurnLeftRightAxis)
      .def("GetTurnLeftRightAxis", &FlyMotionModel::GetTurnLeftRightAxis, return_internal_reference<>())
      .def("SetTurnUpDownAxis", &FlyMotionModel::SetTurnUpDownAxis)
      .def("GetTurnUpDownAxis", &FlyMotionModel::GetTurnUpDownAxis, return_internal_reference<>())
      .def("SetMaximumFlySpeed", &FlyMotionModel::SetMaximumFlySpeed)
      .def("GetMaximumFlySpeed", &FlyMotionModel::GetMaximumFlySpeed)
      .def("SetMaximumTurnSpeed", &FlyMotionModel::SetMaximumTurnSpeed)
      .def("GetMaximumTurnSpeed", &FlyMotionModel::GetMaximumTurnSpeed);
}

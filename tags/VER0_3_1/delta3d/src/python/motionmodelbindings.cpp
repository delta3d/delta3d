// motionmodelbindings.cpp: MotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "motionmodel.h"

using namespace boost::python;
using namespace dtCore;

void initMotionModelBindings()
{
   MotionModel* (*MotionModelGI1)(int) = &MotionModel::GetInstance;
   MotionModel* (*MotionModelGI2)(std::string) = &MotionModel::GetInstance;

   class_<MotionModel, bases<Base>, osg::ref_ptr<MotionModel> >("MotionModel", init<optional<std::string> >())
      .def("GetInstanceCount", &MotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", MotionModelGI1, return_internal_reference<>())
      .def("GetInstance", MotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetTarget", &MotionModel::SetTarget)
      .def("GetTarget", &MotionModel::GetTarget, return_internal_reference<>())
      .def("SetEnabled", &MotionModel::SetEnabled)
      .def("IsEnabled", &MotionModel::IsEnabled);
}

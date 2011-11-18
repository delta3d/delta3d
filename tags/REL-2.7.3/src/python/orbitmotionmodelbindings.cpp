// orbitmotionmodelbindings.cpp: OrbitMotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/keyboard.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/transformable.h>

using namespace boost::python;
using namespace dtCore;

void initOrbitMotionModelBindings()
{
   OrbitMotionModel* (*OrbitMotionModelGI1)(int) = &OrbitMotionModel::GetInstance;
   OrbitMotionModel* (*OrbitMotionModelGI2)(std::string) = &OrbitMotionModel::GetInstance;

   class_<OrbitMotionModel, bases<MotionModel>, dtCore::RefPtr<OrbitMotionModel> >("OrbitMotionModel", init<optional<Keyboard*, Mouse*> >())
      .def("GetInstanceCount", &OrbitMotionModel::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", OrbitMotionModelGI1, return_internal_reference<>())
      .def("GetInstance", OrbitMotionModelGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetDefaultMappings", &OrbitMotionModel::SetDefaultMappings)
      .def("SetAzimuthAxis", &OrbitMotionModel::SetAzimuthAxis)
      .def("GetAzimuthAxis", &OrbitMotionModel::GetAzimuthAxis, return_internal_reference<>())
      .def("SetElevationAxis", &OrbitMotionModel::SetElevationAxis)
      .def("GetElevationAxis", &OrbitMotionModel::GetElevationAxis, return_internal_reference<>())
      .def("SetDistanceAxis", &OrbitMotionModel::SetDistanceAxis)
      .def("GetDistanceAxis", &OrbitMotionModel::GetDistanceAxis, return_internal_reference<>())
      .def("SetLeftRightTranslationAxis", &OrbitMotionModel::SetLeftRightTranslationAxis)
      .def("GetLeftRightTranslationAxis", &OrbitMotionModel::GetLeftRightTranslationAxis, return_internal_reference<>())
      .def("SetUpDownTranslationAxis", &OrbitMotionModel::SetUpDownTranslationAxis)
      .def("GetUpDownTranslationAxis", &OrbitMotionModel::GetUpDownTranslationAxis, return_internal_reference<>())
      .def("SetAngularRate", &OrbitMotionModel::SetAngularRate)
      .def("GetAngularRate", &OrbitMotionModel::GetAngularRate)
      .def("SetLinearRate", &OrbitMotionModel::SetLinearRate)
      .def("GetLinearRate", &OrbitMotionModel::GetLinearRate)
      .def("SetDistance", &OrbitMotionModel::SetDistance)
      .def("GetDistance", &OrbitMotionModel::GetDistance);
}

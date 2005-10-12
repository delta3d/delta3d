// camerabindings.cpp: Camera binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/camera.h>

using namespace boost::python;
using namespace dtCore;

void initCameraBindings()
{
   Camera* (*CameraGI1)(int) = &Camera::GetInstance;
   Camera* (*CameraGI2)(std::string) = &Camera::GetInstance;
   
   void (Camera::*SetClearColor1)(float, float, float, float) = &Camera::SetClearColor;
   void (Camera::*SetClearColor2)(const osg::Vec4&t) = &Camera::SetClearColor;
   
   void (Camera::*GetClearColor1)(float&, float&, float&, float&) = &Camera::GetClearColor;
   void (Camera::*GetClearColor2)(osg::Vec4&) = &Camera::GetClearColor;
   
   class_<Camera, bases<Transformable>, dtCore::RefPtr<Camera> >("Camera", init<optional<const std::string&> >())
      .def("GetInstanceCount", &Camera::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CameraGI1, return_internal_reference<>())
      .def("GetInstance", CameraGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetWindow", &Camera::SetWindow, with_custodian_and_ward<1, 2>())
      .def("GetWindow", &Camera::GetWindow, return_internal_reference<>())
      .def("Frame", &Camera::Frame)
      .def("SetScene", &Camera::SetScene, with_custodian_and_ward<1, 2>())
      .def("GetScene", &Camera::GetScene, return_internal_reference<>())
      .def("SetClearColor", SetClearColor1)
      .def("SetClearColor", SetClearColor2)
      .def("GetClearColor", GetClearColor1)
      .def("GetClearColor", GetClearColor2)
      .def("SetPerspective", &Camera::SetPerspective)
      .def("SetFrustum", &Camera::SetFrustum)
      .def("SetOrtho", &Camera::SetOrtho)
      .def("ConvertToOrtho", &Camera::ConvertToOrtho)
      .def("ConvertToPerspective", &Camera::ConvertToPerspective)
      .def("GetHorizontalFov", &Camera::GetHorizontalFov)
      .def("GetVerticalFov", &Camera::GetVerticalFov)
      .def("SetAutoAspect", &Camera::SetAutoAspect)
      .def("GetAutoAspect", &Camera::GetAutoAspect)
      .def("SetAspectRatio", &Camera::SetAspectRatio)
      .def("GetAspectRatio", &Camera::GetAspectRatio)
      .def("SetNextStatisticsType", &Camera::SetNextStatisticsType);
}

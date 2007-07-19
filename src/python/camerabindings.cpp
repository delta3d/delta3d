// camerabindings.cpp: Camera binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

void initCameraBindings()
{
   Camera* (*CameraGI1)(int) = &Camera::GetInstance;
   Camera* (*CameraGI2)(std::string) = &Camera::GetInstance;
   
   void (Camera::*SetClearColor1)(float, float, float, float) = &Camera::SetClearColor;
   void (Camera::*SetClearColor2)(const osg::Vec4&t) = &Camera::SetClearColor;
   
   void (Camera::*GetClearColor)(osg::Vec4&) = &Camera::GetClearColor;

   Scene* (Camera::*GetScene2)() = &Camera::GetScene;
   const Scene* (Camera::*GetScene1)() const = &Camera::GetScene;
   
   class_<Camera, bases<Transformable>, dtCore::RefPtr<Camera>, boost::noncopyable >("Camera", init<optional<const std::string&> >())
      .def("GetInstanceCount", &Camera::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CameraGI1, return_internal_reference<>())
      .def("GetInstance", CameraGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetEnabled", &Camera::GetEnabled)
      .def("SetEnabled", &Camera::SetEnabled)
      .def("SetWindow", &Camera::SetWindow, with_custodian_and_ward<1, 2>())
      .def("GetWindow", &Camera::GetWindow, return_internal_reference<>())
      .def("Frame", &Camera::Frame)
      .def("SetScene", &Camera::SetScene, with_custodian_and_ward<1, 2>())
      .def("GetScene", GetScene1, return_internal_reference<>())
      .def("GetScene", GetScene2, return_internal_reference<>())
      .def("SetClearColor", SetClearColor1)
      .def("SetClearColor", SetClearColor2)
      .def("GetClearColor", GetClearColor)
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
      .def("SetNextStatisticsType", &Camera::SetNextStatisticsType)
      ;
}

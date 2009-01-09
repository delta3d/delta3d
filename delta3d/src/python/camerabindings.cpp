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

   osg::Camera* (Camera::*GetOSGCamera1)() = &Camera::GetOSGCamera;
   const osg::Camera * (Camera::*GetOSGCamera2)() const = &Camera::GetOSGCamera;

   DeltaWin* (Camera::*GetWindow1)() = &Camera::GetWindow;
   
   scope in_camera = class_<Camera, bases<Transformable>, dtCore::RefPtr<Camera>, boost::noncopyable >("Camera", init<optional<const std::string&> >())
      .def("GetInstanceCount", &Camera::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CameraGI1, return_internal_reference<>())
      .def("GetInstance", CameraGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetEnabled", &Camera::GetEnabled)
      .def("SetEnabled", &Camera::SetEnabled)      
      .def("GetHorizontalFov", &Camera::GetHorizontalFov)
      .def("GetVerticalFov", &Camera::GetVerticalFov)
      .def("SetWindow", &Camera::SetWindow, with_custodian_and_ward<1, 2>())
      .def("GetWindow", GetWindow1, return_internal_reference<>())
      .def("SetClearColor", SetClearColor1)
      .def("SetClearColor", SetClearColor2)
      .def("GetClearColor", GetClearColor)
      .def("SetPerspectiveParams", &Camera::SetPerspectiveParams)
      .def("SetFrustum", &Camera::SetFrustum)
      .def("SetOrtho", &Camera::SetOrtho)
      .def("SetNearFarCullingMode", &Camera::SetNearFarCullingMode)          
      .def("GetOSGCamera", GetOSGCamera1, return_value_policy<reference_existing_object>())
      .def("GetOSGCamera", GetOSGCamera2, return_value_policy<reference_existing_object>())      
	  ;

   enum_<Camera::AutoNearFarCullingMode>("AutoNearFarCullingMode")
      .value("NO_AUTO_NEAR_FAR", Camera::NO_AUTO_NEAR_FAR)
      .value("BOUNDING_VOLUME_NEAR_FAR", Camera::BOUNDING_VOLUME_NEAR_FAR)
      .value("PRIMITIVE_NEAR_FAR", Camera::PRIMITIVE_NEAR_FAR)
      .export_values();
}

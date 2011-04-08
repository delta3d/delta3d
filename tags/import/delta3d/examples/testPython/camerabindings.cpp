// camerabindings.cpp: Camera binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "camera.h"

using namespace boost::python;
using namespace P51;

void initCameraBindings()
{
   Camera* (*CameraGI1)(int) = &Camera::GetInstance;
   Camera* (*CameraGI2)(std::string) = &Camera::GetInstance;

   void (Camera::*SetClearColor1)(float, float, float, float) = &Camera::SetClearColor;
   void (Camera::*SetClearColor2)(sgVec4) = &Camera::SetClearColor;

   void (Camera::*GetClearColor1)(float*, float*, float*, float*) = &Camera::GetClearColor;
   void (Camera::*GetClearColor2)(sgVec4) = &Camera::GetClearColor;

   class_<Camera, bases<Transformable> >("Camera", init<optional<std::string> >())
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
      .def("GetClearColor", GetClearColor2);
}
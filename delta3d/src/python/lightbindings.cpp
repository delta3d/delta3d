// lightbindings.cpp: Light binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/light.h"

using namespace boost::python;
using namespace dtCore;

void initLightBindings()
{
   //non ref_ptr class, use Light*?
   //constructor

   enum_<Light::LightingMode>("LightingMode")
      .value("GLOBAL", Light::GLOBAL)
      .value("LOCAL", Light::LOCAL)
      .export_values();

   class_<Light, Light*, boost::noncopyable>("Light", no_init)
      .def("GetOSGLightSource", &Light::GetOSGLightSource, return_internal_reference<>())
      .def("AddLightChild", &Light::AddLightChild)
      .def("RemoveLightChild", &Light::RemoveLightChild)
      .def("SetLightingMode", &Light::SetLightingMode)
      .def("GetLightingMode", &Light::GetLightingMode)
      .def("SetSceneParent", &Light::SetSceneParent)
      .def("GetSceneParent", &Light::GetSceneParent, return_internal_reference<>())
      .def("SetEnabled", &Light::SetEnabled)
      .def("GetEnabled", &Light::GetEnabled)
      .def("SetLightModel", &Light::SetLightModel)
      .def("SetNumber", &Light::SetNumber)
      .def("GetNumber", &Light::GetNumber)
      .def("SetAmbient", &Light::SetAmbient)
      .def("GetAmbient", &Light::GetAmbient)
      .def("SetDiffuse", &Light::SetDiffuse)
      .def("GetDiffuse", &Light::GetDiffuse)
      .def("SetSpecular", &Light::SetSpecular)
      .def("GetSpecular", &Light::GetSpecular);
}

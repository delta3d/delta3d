// lightbindings.cpp: Light binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/light.h>

using namespace boost::python;
using namespace dtCore;

void initLightBindings()
{
   scope LightScope = class_<Light, bases<Transformable>, dtCore::RefPtr<Light>, boost::noncopyable>("Light", no_init)
      .def("SetLightingMode", &Light::SetLightingMode)
      .def("GetLightingMode", &Light::GetLightingMode)
      .def("SetEnabled", &Light::SetEnabled)
      .def("GetEnabled", &Light::GetEnabled)
      .def("SetNumber", &Light::SetNumber)
      .def("GetNumber", &Light::GetNumber)
      .def("SetAmbient", &Light::SetAmbient)
      .def("GetAmbient", &Light::GetAmbient)
      .def("SetDiffuse", &Light::SetDiffuse)
      .def("GetDiffuse", &Light::GetDiffuse)
      .def("SetSpecular", &Light::SetSpecular)
      .def("GetSpecular", &Light::GetSpecular);

   enum_<Light::LightingMode>("LightingMode")
      .value("GLOBAL", Light::GLOBAL)
      .value("LOCAL", Light::LOCAL)
      .export_values();
}

// lightbindings.cpp: Light binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/light.h>

using namespace boost::python;
using namespace dtCore;

void initLightBindings()
{
   void (Light::*SetAmbient1)(float, float, float, float) = &Light::SetAmbient;
   void (Light::*SetAmbient2)(const osg::Vec4&) = &Light::SetAmbient;
   const osg::Vec4& (Light::*GetAmbient1)() const = &Light::GetAmbient;

   void (Light::*SetDiffuse1)(float, float, float, float) = &Light::SetDiffuse;
   void (Light::*SetDiffuse2)(const osg::Vec4&) = &Light::SetDiffuse;
   const osg::Vec4& (Light::*GetDiffuse1)() const = &Light::GetDiffuse;

   void (Light::*SetSpecular1)(float, float, float, float) = &Light::SetSpecular;
   void (Light::*SetSpecular2)(const osg::Vec4&) = &Light::SetSpecular;
   const osg::Vec4& (Light::*GetSpecular1)() const = &Light::GetSpecular;

   scope LightScope = class_<Light, bases<Transformable>, dtCore::RefPtr<Light>, boost::noncopyable>("Light", no_init)
      .def("SetLightingMode", &Light::SetLightingMode)
      .def("GetLightingMode", &Light::GetLightingMode)
      .def("SetEnabled", &Light::SetEnabled)
      .def("GetEnabled", &Light::GetEnabled)
      .def("SetNumber", &Light::SetNumber)
      .def("GetNumber", &Light::GetNumber)
      .def("SetAmbient", SetAmbient1)
      .def("SetAmbient", SetAmbient2)
      .def("GetAmbient", GetAmbient1, return_value_policy<copy_const_reference>())
      .def("SetDiffuse", SetDiffuse1)
      .def("SetDiffuse", SetDiffuse2)
      .def("GetDiffuse", GetDiffuse1, return_value_policy<copy_const_reference>())
      .def("SetSpecular", SetSpecular1)
      .def("SetSpecular", SetSpecular2)
      .def("GetSpecular", GetSpecular1, return_value_policy<copy_const_reference>())
      .def("AddChild", &Light::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &Light::RemoveChild)
      ;

   enum_<Light::LightingMode>("LightingMode")
      .value("GLOBAL", Light::GLOBAL)
      .value("LOCAL", Light::LOCAL)
      .export_values();
}

// positionallightbindings.cpp: PositionalLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/positionallight.h>

using namespace boost::python;
using namespace dtCore;

void initPositionalLightBindings()
{
   void (PositionalLight::*SetAttenuation1)(float, float, float) = &PositionalLight::SetAttenuation;
   void (PositionalLight::*SetAttenuation2)(const osg::Vec3&) = &PositionalLight::SetAttenuation;
   osg::Vec3 (PositionalLight::*GetAttenuation1)() const = &PositionalLight::GetAttenuation;

   class_<PositionalLight, bases<Light>, dtCore::RefPtr<PositionalLight>, boost::noncopyable >("PositionalLight", init<int, optional<const std::string&, Light::LightingMode> >())
      .def("SetAttenuation", SetAttenuation1)
      .def("SetAttenuation", SetAttenuation2)
      .def("GetAttenuation", GetAttenuation1)
	  ;
}

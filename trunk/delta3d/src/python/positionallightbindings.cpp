// positionallightbindings.cpp: PositionalLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/positionallight.h"

using namespace boost::python;
using namespace dtCore;

void initPositionalLightBindings()
{
   class_<PositionalLight, bases<Light,Transformable>, osg::ref_ptr<PositionalLight> >("PositionalLight", init<int, optional<std::string, Light::LightingMode> >())
      .def("SetAttenuation", &PositionalLight::SetAttenuation)
      .def("GetAttenuation", &PositionalLight::GetAttenuation);
}

// spotlightbindings.cpp: SpotLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/spotlight.h>

using namespace boost::python;
using namespace dtCore;

void initSpotLightBindings()
{
   class_<SpotLight, bases<PositionalLight>, dtCore::RefPtr<SpotLight>, boost::noncopyable >("SpotLight", init<int, optional<std::string, Light::LightingMode> >())
      .def("SetSpotCutoff", &SpotLight::SetSpotCutoff)
      .def("GetSpotCutoff", &SpotLight::GetSpotCutoff)
      .def("SetSpotExponent", &SpotLight::SetSpotExponent)
      .def("GetSpotExponent", &SpotLight::GetSpotExponent);
}

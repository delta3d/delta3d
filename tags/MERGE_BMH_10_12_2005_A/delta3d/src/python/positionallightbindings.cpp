// positionallightbindings.cpp: PositionalLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/positionallight.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

void initPositionalLightBindings()
{
   class_<PositionalLight, bases<Transformable,Light>, dtCore::RefPtr<PositionalLight> >("PositionalLight", init<int, optional<std::string, Light::LightingMode> >())
      .def("SetAttenuation", &PositionalLight::SetAttenuation)
      .def("GetAttenuation", &PositionalLight::GetAttenuation)
      .def("AddChild", &PositionalLight::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &PositionalLight::RemoveChild);
}

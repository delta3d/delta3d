// infinitelightbindings.cpp: InfiniteLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/infinitelight.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

void initInfiniteLightBindings()
{
   class_<InfiniteLight, bases<Light>, dtCore::RefPtr<InfiniteLight> >("InfiniteLight", init<int, optional<std::string, Light::LightingMode> >())
      .def("SetAzimuthElevation", &InfiniteLight::SetAzimuthElevation)
      .def("GetAzimuthElevation", &InfiniteLight::GetAzimuthElevation);
}

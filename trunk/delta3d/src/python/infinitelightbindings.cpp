// infinitelightbindings.cpp: InfiniteLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/infinitelight.h"

using namespace boost::python;
using namespace dtCore;

void initInfiniteLightBindings()
{
   class_<InfiniteLight, bases<Light>, osg::ref_ptr<InfiniteLight> >("InfiniteLight", init<int, optional<std::string, Light::LightingMode> >())
      .def("SetDirection", &InfiniteLight::SetDirection)
      .def("GetDirection", &InfiniteLight::GetDirection);
}

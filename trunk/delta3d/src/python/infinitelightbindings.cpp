// infinitelightbindings.cpp: InfiniteLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/infinitelight.h>

using namespace boost::python;
using namespace dtCore;

void initInfiniteLightBindings()
{
/* 
All the Az/El functions have been removed in favor of using the InfiniteLight's transform.

   void (InfiniteLight::*SetAzimuthElevation1)(float, float) = &InfiniteLight::SetAzimuthElevation;
   void (InfiniteLight::*SetAzimuthElevation2)(const osg::Vec2&) = &InfiniteLight::SetAzimuthElevation;
   osg::Vec2 (InfiniteLight::*GetAzimuthElevation1)() const = &InfiniteLight::GetAzimuthElevation;

   class_<InfiniteLight, bases<Light>, dtCore::RefPtr<InfiniteLight>, boost::noncopyable >("InfiniteLight", init<int, optional<const std::string&, Light::LightingMode> >())
      .def("SetAzimuthElevation", SetAzimuthElevation1)
      .def("SetAzimuthElevation", SetAzimuthElevation2)
      .def("GetAzimuthElevation", GetAzimuthElevation1)
   ;
*/
}

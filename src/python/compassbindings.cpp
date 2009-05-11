// compassbindings.cpp: Compass binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/compass.h>
#include <dtCore/camera.h>

using namespace boost::python;
using namespace dtCore;

void initCompassBindings()
{
   Compass* (*CompassGI1)(int) = &Compass::GetInstance;
   Compass* (*CompassGI2)(std::string) = &Compass::GetInstance;

   osg::Vec2 (Compass::*GetScreenPosition1)() const = &Compass::GetScreenPosition;
   void (Compass::*SetScreenPosition1)(float, float) = &Compass::SetScreenPosition;
   void (Compass::*SetScreenPosition2)(const osg::Vec2&) = &Compass::SetScreenPosition;

   class_<Compass, bases<Transformable>, dtCore::RefPtr<Compass>, boost::noncopyable >("Compass", init<Camera*>())
      .def("GetInstanceCount", &Compass::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CompassGI1, return_internal_reference<>())
      .def("GetInstance", CompassGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetScreenPosition", GetScreenPosition1)
      .def("SetScreenPosition", SetScreenPosition1)
      .def("SetScreenPosition", SetScreenPosition2)
      .def("GetCamera", &Compass::GetCamera, return_internal_reference<>())
      .def("SetCamera", &Compass::SetCamera);
}

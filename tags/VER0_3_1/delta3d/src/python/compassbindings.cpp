// compassbindings.cpp: Compass binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "compass.h"

using namespace boost::python;
using namespace dtCore;

void initCompassBindings()
{
   Compass* (*CompassGI1)(int) = &Compass::GetInstance;
   Compass* (*CompassGI2)(std::string) = &Compass::GetInstance;

   class_<Compass, bases<Transformable, DeltaDrawable>, osg::ref_ptr<Compass> >("Compass", init<Camera*>())
      .def("GetInstanceCount", &Compass::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CompassGI1, return_internal_reference<>())
      .def("GetInstance", CompassGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetScreenPosition", &Compass::GetScreenPosition)
      .def("SetScreenPosition", &Compass::SetScreenPosition)
      .def("GetCamera", &Compass::GetCamera, return_internal_reference<>())
      .def("SetCamera", &Compass::SetCamera);
}

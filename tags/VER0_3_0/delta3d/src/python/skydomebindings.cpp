// skydomebindings.cpp: SkyDome binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "skydome.h"

using namespace boost::python;
using namespace dtCore;

void initSkyDomeBindings()
{
   SkyDome* (*SkyDomeGI1)(int) = &SkyDome::GetInstance;
   SkyDome* (*SkyDomeGI2)(std::string) = &SkyDome::GetInstance;

   class_<SkyDome, bases<EnvEffect>, osg::ref_ptr<SkyDome> >("SkyDome", init<optional<std::string> >())
      .def("GetInstanceCount", &SkyDome::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SkyDomeGI1, return_internal_reference<>())
      .def("GetInstance", SkyDomeGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetBaseColor", &SkyDome::SetBaseColor)
      .def("GetBaseColor", &SkyDome::GetBaseColor);
}
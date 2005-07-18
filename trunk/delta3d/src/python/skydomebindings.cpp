// skydomebindings.cpp: SkyDome binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/skydome.h"

using namespace boost::python;
using namespace dtCore;

void initSkyDomeBindings()
{
   SkyDome* (*SkyDomeGI1)(int) = &SkyDome::GetInstance;
   SkyDome* (*SkyDomeGI2)(std::string) = &SkyDome::GetInstance;

   void (SkyDome::*SetBaseColor1)(const osg::Vec3&) = &SkyDome::SetBaseColor;
   void (SkyDome::*SetBaseColor2)(sgVec3) = &SkyDome::SetBaseColor;

   void (SkyDome::*GetBaseColor1)(osg::Vec3&) const = &SkyDome::GetBaseColor;
   void (SkyDome::*GetBaseColor2)(sgVec3) = &SkyDome::GetBaseColor;

   void (SkyDome::*Repaint1)(const osg::Vec3&, const osg::Vec3&, double, double, double) = &SkyDome::Repaint;
   void (SkyDome::*Repaint2)(sgVec3, sgVec3, double, double, double) = &SkyDome::Repaint;

   class_<SkyDome, bases<EnvEffect>, dtCore::RefPtr<SkyDome> >("SkyDome", init<optional<const std::string&> >())
      .def("GetInstanceCount", &SkyDome::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SkyDomeGI1, return_internal_reference<>())
      .def("GetInstance", SkyDomeGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetBaseColor", SetBaseColor1)
      .def("SetBaseColor", SetBaseColor2)
      .def("GetBaseColor", GetBaseColor1)
      .def("GetBaseColor", GetBaseColor2)
      .def("Repaint", Repaint1)
      .def("Repaint", Repaint2);
}

// environmentbindings.cpp: Environment binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/environment.h"

using namespace boost::python;
using namespace dtCore;

void initEnvironmentBindings()
{
   Environment* (*EnvironmentGI1)(int) = &Environment::GetInstance;
   Environment* (*EnvironmentGI2)(std::string) = &Environment::GetInstance;

   scope Environment_scope = class_<Environment, bases<Base, DeltaDrawable>, osg::ref_ptr<Environment>, boost::noncopyable>("Environment", init<optional<std::string> >())
      .def("GetInstanceCount", &Environment::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", EnvironmentGI1, return_internal_reference<>())
      .def("GetInstance", EnvironmentGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddDrawable", &Environment::AddDrawable)
      .def("AddEffect", &Environment::AddEffect)
      .def("RemEffect", &Environment::RemEffect)
      .def("GetEffect", &Environment::GetEffect, return_internal_reference<>())
      .def("GetNumEffects", &Environment::GetNumEffects)
      .def("SetSkyColor", &Environment::SetSkyColor)
      .def("GetSkyColor", &Environment::GetSkyColor)
      .def("SetFogColor", &Environment::SetFogColor)
      .def("GetFogColor", &Environment::GetFogColor)
      .def("SetFogMode", &Environment::SetFogMode)
      .def("GetFogMode", &Environment::GetFogMode)
      .def("SetAdvFogCtrl", &Environment::SetAdvFogCtrl)
      .def("GetAdvFogCtrl", &Environment::GetAdvFogCtrl)
      .def("SetFogNear", &Environment::SetFogNear)
      .def("GetFogNear", &Environment::GetFogNear)
      .def("SetFogEnable", &Environment::SetFogEnable)
      .def("GetFogEnable", &Environment::GetFogEnable)
      .def("SetVisibility", &Environment::SetVisibility)
      .def("GetVisibility", &Environment::GetVisibility)
      .def("GetSunColor", &Environment::GetSunColor)
      .def("GetSunAzEl", &Environment::GetSunAzEl)
      .def("Repaint", &Environment::Repaint)
      .def("SetDateTime", &Environment::SetDateTime)
      .def("GetDateTime", &Environment::GetDateTime)
      .def("SetRefLatLong", &Environment::SetRefLatLong)
      .def("GetRefLatLong", &Environment::GetRefLatLong);
      
   enum_<Environment::FogMode>("FogMode")
      .value("LINEAR", Environment::LINEAR)
      .value("EXP", Environment::EXP)
      .value("EXP2", Environment::EXP2)
      .value("ADV", Environment::ADV)
      .export_values();
}

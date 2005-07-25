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

   void (Environment::*SetSkyColor1)( const osg::Vec3& ) = &Environment::SetSkyColor;
   void (Environment::*SetSkyColor2)( sgVec3 ) = &Environment::SetSkyColor;

   void (Environment::*GetSkyColor1)( osg::Vec3& ) const = &Environment::GetSkyColor;
   void (Environment::*GetSkyColor2)( sgVec3 ) const = &Environment::GetSkyColor;

   void (Environment::*SetFogColor1)( const osg::Vec3& ) = &Environment::SetFogColor;
   void (Environment::*SetFogColor2)( sgVec3 ) = &Environment::SetFogColor;

   void (Environment::*GetFogColor1)( osg::Vec3& ) const = &Environment::GetFogColor;
   void (Environment::*GetFogColor2)( sgVec3 ) const = &Environment::GetFogColor;

   void (Environment::*GetModFogColor1)( osg::Vec3& ) const = &Environment::GetModFogColor;
   void (Environment::*GetModFogColor2)( sgVec3 ) const = &Environment::GetModFogColor;

   void (Environment::*SetAdvFogCtrl1)( const osg::Vec3& ) = &Environment::SetAdvFogCtrl;
   void (Environment::*SetAdvFogCtrl2)( sgVec3 ) = &Environment::SetAdvFogCtrl;

   void (Environment::*GetAdvFogCtrl1)( osg::Vec3& ) const = &Environment::GetAdvFogCtrl;
   void (Environment::*GetAdvFogCtrl2)( sgVec3 ) = &Environment::GetAdvFogCtrl;

   void (Environment::*GetSunColor1)( osg::Vec3& ) = &Environment::GetSunColor;
   void (Environment::*GetSunColor2)( sgVec3 ) = &Environment::GetSunColor;

   void (Environment::*SetRefLatLong1)( const osg::Vec2& ) = &Environment::SetRefLatLong;
   void (Environment::*SetRefLatLong2)( sgVec2 ) = &Environment::SetRefLatLong;

   void (Environment::*GetRefLatLong1)( osg::Vec2& ) const = &Environment::GetRefLatLong;
   void (Environment::*GetRefLatLong2)( sgVec2 ) = &Environment::GetRefLatLong;


   scope Environment_scope = class_<Environment, bases<DeltaDrawable>, dtCore::RefPtr<Environment>, boost::noncopyable>("Environment", init<optional<const std::string&> >())
      .def("GetInstanceCount", &Environment::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", EnvironmentGI1, return_internal_reference<>())
      .def("GetInstance", EnvironmentGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddChild", &Environment::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &Environment::RemoveChild)
      .def("AddEffect", &Environment::AddEffect)
      .def("RemEffect", &Environment::RemEffect)
      .def("GetEffect", &Environment::GetEffect, return_internal_reference<>())
      .def("GetNumEffects", &Environment::GetNumEffects)
      .def("SetSkyColor", SetSkyColor1)
      .def("SetSkyColor", SetSkyColor2)
      .def("GetSkyColor", GetSkyColor1)
      .def("GetSkyColor", GetSkyColor2)
      .def("SetFogColor", SetFogColor1)
      .def("SetFogColor", SetFogColor2)
      .def("GetFogColor", GetFogColor1)
      .def("GetFogColor", GetFogColor2)
      .def("GetModFogColor", GetModFogColor1)
      .def("GetModFogColor", GetModFogColor2)
      .def("SetFogMode", &Environment::SetFogMode)
      .def("GetFogMode", &Environment::GetFogMode)
      .def("SetAdvFogCtrl", SetAdvFogCtrl1)
      .def("SetAdvFogCtrl", SetAdvFogCtrl2)
      .def("GetAdvFogCtrl", GetAdvFogCtrl1)
      .def("GetAdvFogCtrl", GetAdvFogCtrl2)
      .def("SetFogNear", &Environment::SetFogNear)
      .def("GetFogNear", &Environment::GetFogNear)
      .def("SetFogEnable", &Environment::SetFogEnable)
      .def("GetFogEnable", &Environment::GetFogEnable)
      .def("SetVisibility", &Environment::SetVisibility)
      .def("GetVisibility", &Environment::GetVisibility)
      .def("GetSunColor", GetSunColor1)
      .def("GetSunColor", GetSunColor2)
      .def("GetSunAzEl", &Environment::GetSunAzEl)
      .def("Repaint", &Environment::Repaint)
      .def("SetDateTime", &Environment::SetDateTime)
      .def("GetDateTime", &Environment::GetDateTime)
      .def("SetRefLatLong", SetRefLatLong1)
      .def("SetRefLatLong", SetRefLatLong2)
      .def("GetRefLatLong", GetRefLatLong1)
      .def("GetRefLatLong", GetRefLatLong2);
      
   enum_<Environment::FogMode>("FogMode")
      .value("LINEAR", Environment::LINEAR)
      .value("EXP", Environment::EXP)
      .value("EXP2", Environment::EXP2)
      .value("ADV", Environment::ADV)
      .export_values();
}

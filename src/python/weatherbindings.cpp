// weatherbindings.cpp: Weather binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/weather.h>
#include <dtCore/cloudplane.h>
#include <dtCore/environment.h>

using namespace boost::python;
using namespace dtABC;
using namespace dtCore;

void initWeatherBindings()
{
   Weather* (*WeatherGI1)(int) = &Weather::GetInstance;
   Weather* (*WeatherGI2)(std::string) = &Weather::GetInstance;
   
   //   const dtCore::Environment* GetEnvironment() const {return mEnvironment.get();}
   const dtCore::Environment* (Weather::*GetEnv) () const = &Weather::GetEnvironment;
   dtCore::Environment* (Weather::*GetEnv2) () = &Weather::GetEnvironment;

   scope Weather_scope = class_<Weather, bases<Base>, dtCore::RefPtr<Weather> >("Weather", init<>())
      .def("GetInstanceCount", &Weather::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", WeatherGI1, return_internal_reference<>())
      .def("GetInstance", WeatherGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetBasicCloudType", &Weather::SetBasicCloudType)
      .def("GetBasicCloudType", &Weather::GetBasicCloudType)
      .def("SetBasicWindType", &Weather::SetBasicWindType)
      .def("GetBasicWindType", &Weather::GetBasicWindType)
      .def("SetBasicVisibilityType", &Weather::SetBasicVisibilityType)
      .def("GetBasicVisibilityType", &Weather::GetBasicVisibilityType)
      .def("SetTheme", &Weather::SetTheme)
      .def("GetTheme", &Weather::GetTheme)
      .def("GetEnvironment", GetEnv, return_internal_reference<>())
      .def("GetEnvironment", GetEnv2, return_internal_reference<>())
      .def("SetRateOfChange", &Weather::SetRateOfChange)
      .def("GetRateOfChange", &Weather::GetRateOfChange)
      .def("AddChild", &Weather::AddChild)
      .def("RemoveChild", &Weather::RemoveChild);
      
   enum_<Weather::CloudType>("CloudType")
      .value("CLOUD_CLEAR", Weather::CLOUD_CLEAR)
      .value("CLOUD_FEW", Weather::CLOUD_FEW)
      .value("CLOUD_SCATTERED", Weather::CLOUD_SCATTERED)
      .value("CLOUD_BROKEN", Weather::CLOUD_BROKEN)
      .value("CLOUD_OVERCAST", Weather::CLOUD_OVERCAST)
      .export_values();
      
   enum_<Weather::WindType>("WindType")
      .value("WIND_NONE", Weather::WIND_NONE)
      .value("WIND_BREEZE", Weather::WIND_BREEZE)
      .value("WIND_LIGHT", Weather::WIND_LIGHT)
      .value("WIND_MODERATE", Weather::WIND_MODERATE)
      .value("WIND_HEAVY", Weather::WIND_HEAVY)
      .value("WIND_SEVERE", Weather::WIND_SEVERE)
      .export_values();
      
   enum_<Weather::VisibilityType>("VisibilityType")
      .value("VIS_UNLIMITED", Weather::VIS_UNLIMITED)
      .value("VIS_FAR", Weather::VIS_FAR)
      .value("VIS_MODERATE", Weather::VIS_MODERATE)
      .value("VIS_LIMITED", Weather::VIS_LIMITED)
      .value("VIS_CLOSE", Weather::VIS_CLOSE)
      .export_values();
      
   enum_<Weather::WeatherTheme>("WeatherTheme")
      .value("THEME_CLEAR", Weather::THEME_CLEAR)
      .value("THEME_FAIR", Weather::THEME_FAIR)
      .value("THEME_FOGGY", Weather::THEME_FOGGY)
      .value("THEME_RAINY", Weather::THEME_RAINY)
      .export_values();
}

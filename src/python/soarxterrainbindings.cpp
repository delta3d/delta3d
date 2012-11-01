// soarxterrainbindings.cpp: SOARXTerrain binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <soarx/soarxterrain.h>

#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;
using namespace dtSOARX;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LGI_overloads, LoadGeospecificImage, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LR_overloads, LoadRoads, 1, 6)

void initSOARXTerrainBindings()
{
   SOARXTerrain* (*SOARXTerrainGI1)(int) = &SOARXTerrain::GetInstance;
   SOARXTerrain* (*SOARXTerrainGI2)(std::string) = &SOARXTerrain::GetInstance;

   void (SOARXTerrain::*GetNormal1)(float, float, osg::Vec3&) = &SOARXTerrain::GetNormal;

   scope SOARXTerrain_scope = class_<SOARXTerrain, bases<Transformable, DeltaDrawable, Physical>, dtCore::RefPtr<SOARXTerrain> >("SOARXTerrain", init<optional<std::string> >())
      .def("GetInstanceCount", &SOARXTerrain::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SOARXTerrainGI1, return_internal_reference<>())
      .def("GetInstance", SOARXTerrainGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadConfiguration", &SOARXTerrain::LoadConfiguration)
      .def("ParseConfiguration", &SOARXTerrain::ParseConfiguration)
      .def("LoadConfiguration", &SOARXTerrain::LoadConfiguration)
      .def("SetGeoOrigin", &SOARXTerrain::SetGeoOrigin)
      .def("GetGeoOrigin", &SOARXTerrain::GetGeoOrigin)
      .def("SetLoadDistance", &SOARXTerrain::SetLoadDistance)
      .def("GetLoadDistance", &SOARXTerrain::GetLoadDistance)
      .def("AddDTEDPath", &SOARXTerrain::AddDTEDPath)
      .def("RemoveDTEDPath", &SOARXTerrain::RemoveDTEDPath)
      .def("GetNumDTEDPaths", &SOARXTerrain::GetNumDTEDPaths)
      .def("GetDTEDPath", &SOARXTerrain::GetDTEDPath)
      .def("SetCachePath", &SOARXTerrain::SetCachePath)
      .def("GetCachePath", &SOARXTerrain::GetCachePath)
      .def("Reload", &SOARXTerrain::Reload)
      .def("SetLowerHeightColorMap", &SOARXTerrain::SetLowerHeightColorMap)
      .def("GetLowerHeightColorMap", &SOARXTerrain::GetLowerHeightColorMap, return_internal_reference<>())
      .def("SetUpperHeightColorMap", &SOARXTerrain::SetUpperHeightColorMap)
      .def("GetUpperHeightColorMap", &SOARXTerrain::GetUpperHeightColorMap, return_internal_reference<>())
      .def("SetMaxTextureSize", &SOARXTerrain::SetMaxTextureSize)
      .def("GetMaxTextureSize", &SOARXTerrain::GetMaxTextureSize)
      .def("LoadGeospecificImage", &SOARXTerrain::LoadGeospecificImage, LGI_overloads())
      .def("LoadRoads", &SOARXTerrain::LoadRoads, LR_overloads())
      .def("SetThreshold", &SOARXTerrain::SetThreshold)
      .def("GetThreshold", &SOARXTerrain::GetThreshold)
      .def("SetDetailMultiplier", &SOARXTerrain::SetDetailMultiplier)
      .def("GetDetailMultiplier", &SOARXTerrain::GetDetailMultiplier)
      .def("GetHeight", &SOARXTerrain::GetHeight)
      .def("GetNormal", GetNormal1);
}

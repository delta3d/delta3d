// terrainbindings.cpp: Terrain binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/terrain.h"

using namespace boost::python;
using namespace dtCore;

void initTerrainBindings()
{
   Terrain* (*TerrainGI1)(int) = &Terrain::GetInstance;
   Terrain* (*TerrainGI2)(std::string) = &Terrain::GetInstance;

   scope Terrain_scope = class_<Terrain, bases<Transformable, DeltaDrawable>, osg::ref_ptr<Terrain> >("Terrain", init<optional<std::string> >())
      .def("GetInstanceCount", &Terrain::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TerrainGI1, return_internal_reference<>())
      .def("GetInstance", TerrainGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetGeoOrigin", &Terrain::SetGeoOrigin)
      .def("GetGeoOrigin", &Terrain::GetGeoOrigin)
      .def("SetLoadDistance", &Terrain::SetLoadDistance)
      .def("GetLoadDistance", &Terrain::GetLoadDistance)
      .def("AddDTEDPath", &Terrain::AddDTEDPath)
      .def("RemoveDTEDPath", &Terrain::RemoveDTEDPath)
      .def("GetNumDTEDPaths", &Terrain::GetNumDTEDPaths)
      .def("GetDTEDPath", &Terrain::GetDTEDPath)
      .def("SetCachePath", &Terrain::SetCachePath)
      .def("GetCachePath", &Terrain::GetCachePath)
      .def("Reload", &Terrain::Reload)
      .def("SetLowerHeightColorMap", &Terrain::SetLowerHeightColorMap)
      .def("GetLowerHeightColorMap", &Terrain::GetLowerHeightColorMap, return_internal_reference<>())
      .def("SetUpperHeightColorMap", &Terrain::SetUpperHeightColorMap)
      .def("GetUpperHeightColorMap", &Terrain::GetUpperHeightColorMap, return_internal_reference<>());
}

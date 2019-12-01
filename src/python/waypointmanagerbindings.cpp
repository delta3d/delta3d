#include <dtAI/waypointmanager.h>
#include <dtCore/deltadrawable.h>
#include <python/dtpython.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <string>
#include <map>
using namespace boost::python;
using namespace dtAI;

//const WaypointManager::WaypointMap& (WaypointManager::*GetWaypoint1)() const = &WaypointManager::GetWaypoints;

void init_WaypointManagerBindings()
{
   //class_<WaypointManager::WaypointMap>("WaypointMap")
   //   .def(map_indexing_suite<WaypointManager::WaypointMap>());

   WaypointID (WaypointManager::*AddWaypoint1)(const osg::Vec3&) = &WaypointManager::AddWaypoint;

   const Waypoint *(WaypointManager::*GetWaypointc)(WaypointID pIndex) const = &WaypointManager::GetWaypoint;
   Waypoint *(WaypointManager::*GetWaypointnc)(WaypointID pIndex)            = &WaypointManager::GetWaypoint;

   class_<std::vector<Waypoint*> >("WaypointList")
      .def(vector_indexing_suite<std::vector<Waypoint*> >());

   class_<WaypointManager, bases<dtCore::DeltaDrawable>, dtCore::RefPtr<WaypointManager>, boost::noncopyable>("WaypointManager", no_init)
      .def("GetInstance", &WaypointManager::GetInstance, return_value_policy<reference_existing_object>())
      .staticmethod("GetInstance")
      .def("CreateNavMesh", &WaypointManager::CreateNavMesh)
      //.def("GetWaypoints", GetWaypoint1, return_value_policy<copy_const_reference>())
      .def("GetWaypoints", &WaypointManager::CopyWaypointsIntoVector)
      .def("SetDrawWaypoints", &WaypointManager::SetDrawWaypoints)      
      .def("SetDrawNavMesh", &WaypointManager::SetDrawNavMesh)
      .def("WriteFile", &WaypointManager::WriteFile)
      .def("ReadFile", &WaypointManager::ReadFile)
      .def("AvgDistBetweenWaypoints", &WaypointManager::AvgDistBetweenWaypoints)
      .def("Clear", &WaypointManager::Clear)
      .def("AddWaypoint", AddWaypoint1)
      .def("AddPathSegment", &WaypointManager::AddPathSegment)
      .def("GetWaypoint", GetWaypointc,  return_value_policy<reference_existing_object>())
      .def("GetWaypoint", GetWaypointnc, return_value_policy<reference_existing_object>())
      .def("SetWaypointColor", &WaypointManager::SetWaypointColor)
      .def("SetWaypointSize", &WaypointManager::SetWaypointSize)
      ;
      

}

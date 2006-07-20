#include <dtAI/waypoint.h>
#include <dtAI/waypointpair.h>
#include <boost/python.hpp>
#include <osg/Vec3>

using namespace boost::python;
using namespace dtAI;


void init_WaypointPairBindings()
{

   class_<WaypointPair>("WaypointPair")
      .def(init<const Waypoint*, const Waypoint*>())
      .def("ClosestPointTo", &WaypointPair::ClosestPointTo)
      .def("Calculate2DDistance", &WaypointPair::Calculate2DDistance)
      .def("Get2DDistance", &WaypointPair::Get2DDistance)
      .def("CalculateSlope", &WaypointPair::CalculateSlope)
      .def("GetSlope", &WaypointPair::GetSlope)
      .def("SetFrom", &WaypointPair::SetFrom)
      .def("GetFrom", &WaypointPair::GetFrom, return_value_policy<copy_const_reference>())
      .def("SetTo", &WaypointPair::SetTo)
      .def("GetTo", &WaypointPair::GetTo, return_value_policy<copy_const_reference>())
      .def("GetWaypointFrom", &WaypointPair::GetWaypointFrom, return_internal_reference<>())
      .def("GetWaypointTo", &WaypointPair::GetWaypointTo, return_internal_reference<>())
      ;


}
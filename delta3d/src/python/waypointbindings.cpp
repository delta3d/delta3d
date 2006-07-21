#include <dtAI/waypoint.h>
#include <boost/python.hpp>
#include <osg/Vec3>

using namespace boost::python;
using namespace dtAI;


void init_WaypointBindings()
{
   scope waypointScope = class_<Waypoint>("Waypoint")
      .def("GetPosition", &Waypoint::GetPosition, return_value_policy<copy_const_reference>())
      .def("SetPosition", &Waypoint::SetPosition)
      .def("GetRenderFlag", &Waypoint::GetRenderFlag)
      .def("SetRenderFlag", &Waypoint::SetRenderFlag)
      ;
   

   enum_<Waypoint::RenderFlag>("RenderFlag")
      .value("RENDER_DEFAULT", Waypoint::RENDER_DEFUALT)
      .value("RENDER_RED", Waypoint::RENDER_RED)
      .value("RENDER_GREEN", Waypoint::RENDER_GREEN)
      .value("RENDER_BLUE", Waypoint::RENDER_BLUE)
      .export_values();
}

#include <dtAI/waypoint.h>
#include <boost/python.hpp>
#include <osg/Vec3>

using namespace boost::python;
using namespace dtAI;


void init_WaypointBindings()
{
   scope waypointScope = class_<Waypoint>("Waypoint")
      .def(init<osg::Vec3>())
      .def("GetPosition", &Waypoint::GetPosition, return_value_policy<copy_const_reference>())
      .def("SetPosition", &Waypoint::SetPosition)
      .def("GetRenderFlag", &Waypoint::GetRenderFlag)
      .def("SetRenderFlag", &Waypoint::SetRenderFlag)
      .def("SetGradient", &Waypoint::SetGradient)
	   .def("SetColor", &Waypoint::SetColor)
	   .def("GetColor", &Waypoint::GetColor, return_value_policy<copy_const_reference>())
      .def("GetID", &Waypoint::GetID)
      .def("SetID", &Waypoint::SetID)
      ;

   enum_<Waypoint::RenderFlag>("RenderFlag")
      .value("RENDER_DEFAULT", Waypoint::RENDER_DEFAULT)
      .value("RENDER_RED", Waypoint::RENDER_RED)
      .value("RENDER_GREEN", Waypoint::RENDER_GREEN)
      .value("RENDER_BLUE", Waypoint::RENDER_BLUE)
	  .value("RENDER_CUSTOM", Waypoint::RENDER_CUSTOM)
      .export_values();
}

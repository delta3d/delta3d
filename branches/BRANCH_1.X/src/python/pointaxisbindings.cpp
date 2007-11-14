// pointaxisbindings.cpp: PointAxis binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/pointaxis.h>

using namespace boost::python;
using namespace dtCore;

void initPointAxisBindings()
{
   PointAxis* (*PointAxisGI1)(int) = &PointAxis::GetInstance;
   PointAxis* (*PointAxisGI2)(std::string) = &PointAxis::GetInstance;

   PointAxis::AXISCOLOR (PointAxis::*GetColor1)(PointAxis::AXIS) const = &PointAxis::GetColor;
   void (PointAxis::*GetColor2)(PointAxis::AXIS, osg::Vec4&) const = &PointAxis::GetColor;
   
   void (PointAxis::*SetColor1)(PointAxis::AXIS, PointAxis::AXISCOLOR) = &PointAxis::SetColor;
   void (PointAxis::*SetColor2)(PointAxis::AXIS, const osg::Vec4) = &PointAxis::SetColor;
   
   PointAxis::AXISCOLOR (PointAxis::*GetLabelColor1)(PointAxis::AXIS) const = &PointAxis::GetLabelColor;
   void (PointAxis::*GetLabelColor2)(PointAxis::AXIS, osg::Vec4&) const = &PointAxis::GetLabelColor;
   
   void (PointAxis::*SetLabelColor1)(PointAxis::AXIS, PointAxis::AXISCOLOR) = &PointAxis::SetLabelColor;
   void (PointAxis::*SetLabelColor2)(PointAxis::AXIS, const osg::Vec4) = &PointAxis::SetLabelColor;
   
   scope PointAxis_scope = class_<PointAxis, bases<Transformable>, dtCore::RefPtr<PointAxis>, boost::noncopyable >("PointAxis", init<>())
      .def("GetInstanceCount", &PointAxis::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", PointAxisGI1, return_internal_reference<>())
      .def("GetInstance", PointAxisGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Enable", &PointAxis::Enable)
      .def("Disable", &PointAxis::Disable)
      .def("GetLabel", &PointAxis::GetLabel)
      .def("SetLabel", &PointAxis::SetLabel)
      .def("GetLength", &PointAxis::GetLength)
      .def("SetLength", &PointAxis::SetLength)
      .def("GetColor", GetColor1)
      .def("SetColor", SetColor1)
      .def("GetColor", GetColor2)
      .def("SetColor", SetColor2)
      .def("GetLabelColor", GetLabelColor1)
      .def("SetLabelColor", SetLabelColor1)
      .def("GetLabelColor", GetLabelColor2)
      .def("SetLabelColor", SetLabelColor2);
      
   enum_<PointAxis::AXIS>("AXIS")
      .value("X", PointAxis::X)
      .value("Y", PointAxis::Y)
      .value("Z", PointAxis::Z)
      .value("NUMAXES", PointAxis::NUMAXES)
      .export_values();
      
   enum_<PointAxis::AXISCOLOR>("AXISCOLOR")
      .value("BLACK", PointAxis::BLACK)
      .value("WHITE", PointAxis::WHITE)
      .value("RED", PointAxis::RED)
      .value("YELLOW", PointAxis::YELLOW)
      .value("GREEN", PointAxis::GREEN)
      .value("CYAN", PointAxis::CYAN)
      .value("BLUE", PointAxis::BLUE)
      .value("MAGENTA", PointAxis::MAGENTA)
      .value("CUSTOM_X", PointAxis::CUSTOM_X)
      .value("CUSTOM_Y", PointAxis::CUSTOM_Y)
      .value("CUSTOM_Z", PointAxis::CUSTOM_Z)
      .value("NUMAXISCOLORS", PointAxis::NUMAXISCOLORS)
      .export_values();
      
   enum_<PointAxis::FLAG>("FLAG")
      .value("AXES", PointAxis::AXES)
      .value("LABELS", PointAxis::LABELS)
      .value("LABEL_X", PointAxis::LABEL_X)
      .value("LABEL_Y", PointAxis::LABEL_Y)
      .value("LABEL_Z", PointAxis::LABEL_Z)
      .value("XYZ_LABELS", PointAxis::XYZ_LABELS)
      .value("HPR_LABELS", PointAxis::HPR_LABELS)
      .value("CUSTOM_LABELS", PointAxis::CUSTOM_LABELS)
      .export_values();
}

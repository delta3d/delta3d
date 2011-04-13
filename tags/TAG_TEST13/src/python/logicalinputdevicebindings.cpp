// logicalinputdevicebindings.cpp: LogicalInputDevice binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/logicalinputdevice.h>

using namespace boost::python;
using namespace dtCore;

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AB_overloads, AddButton, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AA_overloads, AddAxis, 1, 2)

void initLogicalInputDeviceBindings()
{
   LogicalInputDevice* (*LogicalInputDeviceGI1)(int) = &LogicalInputDevice::GetInstance;
   LogicalInputDevice* (*LogicalInputDeviceGI2)(std::string) = &LogicalInputDevice::GetInstance;

   //LogicalButton* (LogicalInputDevice::*AddButton1)(const std::string&, int, ButtonMapping*) = &LogicalInputDevice::AddButton;
   LogicalButton* (LogicalInputDevice::*AddButton2)(const std::string&, Button*, int) = &LogicalInputDevice::AddButton;
   
   LogicalAxis* (LogicalInputDevice::*AddAxis1)(const std::string&, AxisMapping*) = &LogicalInputDevice::AddAxis;
   LogicalAxis* (LogicalInputDevice::*AddAxis2)(const std::string&, Axis*) = &LogicalInputDevice::AddAxis;
   
   class_<LogicalInputDevice, bases<InputDevice>, dtCore::RefPtr<LogicalInputDevice> >("LogicalInputDevice", init<optional<const std::string&> >())
      .def("GetInstanceCount", &LogicalInputDevice::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", LogicalInputDeviceGI1, return_internal_reference<>())
      .def("GetInstance", LogicalInputDeviceGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      //.def("AddButton", AddButton1, AB_overloads()[return_internal_reference<>()])
      //.def("AddButton", AddButton2, return_internal_reference<>())
	  .def("AddButton", AddButton2, return_internal_reference<>())
      .def("RemoveButton", &LogicalInputDevice::RemoveButton)
      .def("AddAxis", AddAxis1, AA_overloads()[return_internal_reference<>()])
      .def("AddAxis", AddAxis2, return_internal_reference<>())
      .def("RemoveAxis", &LogicalInputDevice::RemoveAxis);
      
   class_<LogicalButton, bases<Button>, dtCore::RefPtr<LogicalButton> >("LogicalButton", no_init)
      .def("SetMapping", &LogicalButton::SetMapping)
      .def("GetMapping", &LogicalButton::GetMapping, return_internal_reference<>());
      
   class_<ButtonMapping, dtCore::RefPtr<ButtonMapping>, boost::noncopyable>("ButtonMapping", no_init);
   
   class_<ButtonToButton, bases<ButtonMapping>, dtCore::RefPtr<ButtonToButton> >("ButtonToButton", init<Button*>())
      .def("SetSourceButton", &ButtonToButton::SetSourceButton)
      .def("GetSourceButton", &ButtonToButton::GetSourceButton, return_internal_reference<>());
      
   class_<LogicalAxis, bases<Axis>, dtCore::RefPtr<LogicalAxis> >("LogicalAxis", no_init)
      .def("SetMapping", &LogicalAxis::SetMapping)
      .def("GetMapping", &LogicalAxis::GetMapping, return_internal_reference<>());
      
   class_<AxisMapping, dtCore::RefPtr<AxisMapping>, boost::noncopyable>("AxisMapping", no_init);
   
   class_<AxisToAxis, bases<AxisMapping>, dtCore::RefPtr<AxisToAxis> >("AxisToAxis", init<Axis*, optional<double, double> >())
      .def("SetSourceAxis", &AxisToAxis::SetSourceAxis)
      .def("GetSourceAxis", &AxisToAxis::GetSourceAxis, return_internal_reference<>())
      .def("SetTransformationParameters", &AxisToAxis::SetTransformationParameters)
      .def("GetTransformationParameters", &AxisToAxis::GetTransformationParameters);

   class_<AxesToAxis, bases<AxisMapping>, dtCore::RefPtr<AxesToAxis> >("AxesToAxis", init<optional<Axis*, Axis*> >())
      .def("AddSourceAxis", &AxesToAxis::AddSourceAxis)
      .def("RemoveSourceAxis", &AxesToAxis::RemoveSourceAxis)
      .def("GetNumSourceAxes", &AxesToAxis::GetNumSourceAxes)
      .def("GetSourceAxis", &AxesToAxis::GetSourceAxis, return_internal_reference<>());
      
   class_<ButtonsToAxis, bases<AxisMapping>, dtCore::RefPtr<ButtonsToAxis> >("ButtonsToAxis", init<Button*, Button*, optional<double, double, double> >())
      .def("SetSourceButtons", &ButtonsToAxis::SetSourceButtons)
      .def("GetSourceButtons", &ButtonsToAxis::GetSourceButtons)
      .def("SetValues", &ButtonsToAxis::SetValues)
      .def("GetValues", &ButtonsToAxis::GetValues);
      
   class_<ButtonAxisToAxis, bases<AxisMapping>, dtCore::RefPtr<ButtonAxisToAxis> >("ButtonAxisToAxis", init<Button*, Axis*>())
      .def("SetSourceButton", &ButtonAxisToAxis::SetSourceButton)
      .def("GetSourceButton", &ButtonAxisToAxis::GetSourceButton, return_internal_reference<>())
      .def("SetSourceAxis", &ButtonAxisToAxis::SetSourceAxis)
      .def("GetSourceAxis", &ButtonAxisToAxis::GetSourceAxis, return_internal_reference<>());
}

// inputdevicebindings.cpp: InputDevice binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/inputdevice.h"

using namespace boost::python;
using namespace dtCore;

class ButtonListenerWrap : public ButtonListener
{
   public:
   
      ButtonListenerWrap(PyObject* self)
         : mSelf(self)
      {}
      
      virtual void ButtonStateChanged(Button* button, bool oldState, bool newState)
      {
         call_method<void>(mSelf, "ButtonStateChanged", button, oldState, newState);
      }
      
      void DefaultButtonStateChanged(Button* button, bool oldState, bool newState)
      {
         ButtonListener::ButtonStateChanged(button, oldState, newState);
      }
      
   private:
      
      PyObject* mSelf;
};

class AxisListenerWrap : public AxisListener
{
   public:
   
      AxisListenerWrap(PyObject* self)
         : mSelf(self)
      {}
      
      virtual void AxisStateChanged(Axis* axis, double oldState, double newState, double delta)
      {
         call_method<void>(mSelf, "AxisStateChanged", axis, oldState, newState, delta);
      }
      
      void DefaultAxisStateChanged(Axis* axis, double oldState, double newState, double delta)
      {
         AxisListener::AxisStateChanged(axis, oldState, newState, delta);
      }
      
   private:
      
      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SS_overloads, SetState, 1, 2)

void initInputDeviceBindings()
{
   InputDevice* (*InputDeviceGI1)(int) = &InputDevice::GetInstance;
   InputDevice* (*InputDeviceGI2)(std::string) = &InputDevice::GetInstance;

   typedef const dtCore::Axis* (dtCore::InputDevice::*ConstAxesInputDeviceMemFun)(int) const;
   ConstAxesInputDeviceMemFun caxes = &InputDevice::GetAxis;

   typedef dtCore::Axis* (dtCore::InputDevice::*NonConstAxesInputDeviceMemFun)(int);
   NonConstAxesInputDeviceMemFun ncaxes = &InputDevice::GetAxis;

   typedef const dtCore::Button* (InputDevice::*ConstButtonInputDeviceMemFun)(int) const;
   ConstButtonInputDeviceMemFun cbutton = &InputDevice::GetButton;

   typedef Button* (InputDevice::*NonConstButtonInputDeviceMemFun)(int);
   NonConstButtonInputDeviceMemFun ncbutton = &InputDevice::GetButton;

   class_<InputDevice, bases<Base>, dtCore::RefPtr<InputDevice> >("InputDevice", no_init)
      .def("GetInstanceCount", &InputDevice::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", InputDeviceGI1, return_internal_reference<>())
      .def("GetInstance", InputDeviceGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetFeatureCount", &InputDevice::GetFeatureCount)
      .def("GetFeature", &InputDevice::GetFeature, return_internal_reference<>())
      .def("GetButtonCount", &InputDevice::GetButtonCount)
      .def("GetButton", cbutton, return_internal_reference<>())
      .def("GetButton", ncbutton, return_internal_reference<>())
      .def("GetAxisCount", &InputDevice::GetAxisCount)
      .def("GetAxis", caxes, return_internal_reference<>())
      .def("GetAxis", ncaxes, return_internal_reference<>())
      .def("AddButtonListener", &InputDevice::AddButtonListener)
      .def("RemoveButtonListener", &InputDevice::RemoveButtonListener)
      .def("AddAxisListener", &InputDevice::AddAxisListener)
      .def("RemoveAxisListener", &InputDevice::RemoveAxisListener);
      
   class_<InputDeviceFeature, dtCore::RefPtr<InputDeviceFeature>, boost::noncopyable>("InputDeviceFeature", no_init)
      .def("GetOwner", &InputDeviceFeature::GetOwner, return_internal_reference<>())
      .def("SetDescription", &InputDeviceFeature::SetDescription)
      .def("GetDescription", &InputDeviceFeature::GetDescription);
      
   class_<Button, bases<InputDeviceFeature>, dtCore::RefPtr<Button> >("Button", no_init)
      .def("SetState", &Button::SetState)
      .def("GetState", &Button::GetState)
      .def("AddButtonListener", &Button::AddButtonListener)
      .def("RemoveButtonListener", &Button::RemoveButtonListener);
      
   class_<ButtonListener, ButtonListenerWrap, boost::noncopyable>("ButtonListener")
      .def("ButtonStateChanged", &ButtonListener::ButtonStateChanged, &ButtonListenerWrap::DefaultButtonStateChanged);
      
   class_<Axis, bases<InputDeviceFeature>, dtCore::RefPtr<Axis> >("Axis", no_init)
      .def("SetState", &Axis::SetState, SS_overloads())
      .def("GetState", &Axis::GetState)
      .def("AddAxisListener", &Axis::AddAxisListener)
      .def("RemoveAxisListener", &Axis::RemoveAxisListener);
      
   class_<AxisListener, AxisListenerWrap, boost::noncopyable>("AxisListener")
      .def("AxisStateChanged", &AxisListener::AxisStateChanged, &AxisListenerWrap::DefaultAxisStateChanged);
}

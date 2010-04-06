// joystickbindings.cpp: Joystick binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtInputPLIB/joystick.h"

using namespace boost::python;
using namespace dtInputPLIB;
using namespace dtCore;

void initJoystickBindings()
{
   Joystick* (*JoystickGI1)(int) = &Joystick::GetInstance;
   Joystick* (*JoystickGI2)(std::string) = &Joystick::GetInstance;

   class_<Joystick, bases<InputDevice>, dtCore::RefPtr<Joystick> >("Joystick", no_init)
      .def("GetInstanceCount", &Joystick::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", JoystickGI1, return_internal_reference<>())
      .def("GetInstance", JoystickGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("CreateInstances", &Joystick::CreateInstances)
      .staticmethod("CreateInstances")
      .def("DestroyInstances", &Joystick::DestroyInstances)
      .staticmethod("DestroyInstances")
      .def("PollInstances", &Joystick::PollInstances)
      .staticmethod("PollInstances")
      .def("Poll", &Joystick::Poll);
}


BOOST_PYTHON_MODULE(PyDtInputPLIB)
{
   initJoystickBindings();
}


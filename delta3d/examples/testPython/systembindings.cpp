// systembindings.cpp: System binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "system.h"

using namespace boost::python;
using namespace P51;

void initSystemBindings()
{
   System* (*SystemGI1)(int) = &System::GetInstance;
   System* (*SystemGI2)(std::string) = &System::GetInstance;

   class_<System, bases<Base>, boost::noncopyable>("System", no_init)
      .def("GetInstanceCount", &System::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SystemGI1, return_internal_reference<>())
      .def("GetInstance", SystemGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetSystem", &System::GetSystem, return_internal_reference<>())
      .staticmethod("GetSystem")
      .def("Config", &System::Config)
      .def("Run", &System::Run)
      .def("Stop", &System::Stop);
}

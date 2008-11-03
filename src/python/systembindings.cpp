// systembindings.cpp: System binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/system.h"

using namespace boost::python;
using namespace dtCore;

void initSystemBindings()
{
   System* (*SystemGI1)(int) = &System::GetInstance;
   System* (*SystemGI2)(std::string) = &System::GetInstance;

   class_<System, bases<Base>, dtCore::RefPtr<System>, boost::noncopyable>("System", no_init)
      .def("GetInstanceCount", &System::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SystemGI1, return_internal_reference<>())
      .def("GetInstance", SystemGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Config", &System::Config)
      .def("Run", &System::Run)
      .def("Start", &System::Start)
      .def("Step", &System::Step)
      .def("Stop", &System::Stop)
      .def("IsRunning", &System::IsRunning)
      .def("SetShutdownOnWindowClose", &System::SetShutdownOnWindowClose)
      .def("SetShutdownOnWindowClose", &System::GetShutdownOnWindowClose);
}

// trackerbindings.cpp: Tracker binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/tracker.h"

using namespace boost::python;
using namespace dtCore;

void initTrackerBindings()
{
   Tracker* (*TrackerGI1)(int) = &Tracker::GetInstance;
   Tracker* (*TrackerGI2)(std::string) = &Tracker::GetInstance;

   class_<Tracker, bases<InputDevice>, dtCore::RefPtr<Tracker> >("Tracker", no_init)
      .def("GetInstanceCount", &Tracker::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TrackerGI1, return_internal_reference<>())
      .def("GetInstance", TrackerGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("CreateInstances", &Tracker::CreateInstances)
      .staticmethod("CreateInstances")
      .def("DestroyInstances", &Tracker::DestroyInstances)
      .staticmethod("DestroyInstances")
      .def("PollInstances", &Tracker::PollInstances)
      .staticmethod("PollInstances")
      .def("Poll", &Tracker::Poll);
}

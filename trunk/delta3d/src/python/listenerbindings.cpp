// listenerbindings.cpp: Listener binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "listener.h"

using namespace boost::python;
using namespace dtCore;
using namespace dtAudio;

void initListenerBindings()
{
   Listener* (*ListenerGI1)(int) = &Listener::GetInstance;
   Listener* (*ListenerGI2)(std::string) = &Listener::GetInstance;

   class_<Listener, bases<Transformable>, osg::ref_ptr<Listener>, boost::noncopyable>("Listener", no_init)
      .def("GetInstanceCount", &Listener::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ListenerGI1, return_internal_reference<>())
      .def("GetInstance", ListenerGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetTransform", &Listener::SetTransform)
      .def("GetTransform", &Listener::GetTransform)
      .def("SetVelocity", &Listener::SetVelocity)
      .def("GetVelocity", &Listener::GetVelocity)
      .def("SetGain", &Listener::SetGain)
      .def("GetGain", &Listener::GetGain);
}

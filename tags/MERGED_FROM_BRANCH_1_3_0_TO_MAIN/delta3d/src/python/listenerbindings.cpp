// listenerbindings.cpp: Listener binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtAudio/listener.h"

using namespace boost::python;
using namespace dtCore;
using namespace dtAudio;

void initListenerBindings()
{
   Listener* (*ListenerGI1)(int) = &Listener::GetInstance;
   Listener* (*ListenerGI2)(std::string) = &Listener::GetInstance;

   void (Listener::*SetVelocity1)( const osg::Vec3& velocity ) = &Listener::SetVelocity;

   void (Listener::*GetVelocity1)( osg::Vec3& velocity ) const = &Listener::GetVelocity;

   class_<Listener, bases<Transformable>, dtCore::RefPtr<Listener>, boost::noncopyable>("Listener", no_init)
      .def("GetInstanceCount", &Listener::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ListenerGI1, return_internal_reference<>())
      .def("GetInstance", ListenerGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetVelocity", SetVelocity1)
      .def("GetVelocity", GetVelocity1)
      .def("SetGain", &Listener::SetGain)
      .def("GetGain", &Listener::GetGain)
	  ;
}

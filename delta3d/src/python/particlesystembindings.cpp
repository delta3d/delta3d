// particlesystembindings.cpp: ParticleSystem binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "particlesystem.h"

using namespace boost::python;
using namespace dtCore;

void initParticleSystemBindings()
{
   ParticleSystem* (*ParticleSystemGI1)(int) = &ParticleSystem::GetInstance;
   ParticleSystem* (*ParticleSystemGI2)(std::string) = &ParticleSystem::GetInstance;

   class_<ParticleSystem, bases<Transformable, DeltaDrawable>, osg::ref_ptr<ParticleSystem> >("ParticleSystem", init<optional<std::string> >())
      .def("GetInstanceCount", &ParticleSystem::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ParticleSystemGI1, return_internal_reference<>())
      .def("GetInstance", ParticleSystemGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &ParticleSystem::LoadFile)
      .def("GetFilename", &ParticleSystem::GetFilename)
      .def("SetEnabled", &ParticleSystem::SetEnabled)
      .def("IsEnabled", &ParticleSystem::IsEnabled)
      .def("SetParentRelative", &ParticleSystem::SetParentRelative)
      .def("IsParentRelative", &ParticleSystem::IsParentRelative);
}

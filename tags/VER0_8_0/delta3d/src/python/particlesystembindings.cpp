// particlesystembindings.cpp: ParticleSystem binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/particlesystem.h"

using namespace boost::python;
using namespace dtCore;

class ParticleSystemWrap : public ParticleSystem
{

public:
   ParticleSystemWrap(PyObject* self, std::string name = "ParticleSystem")
      :  mSelf(self)
   {}
   
   void LoadFileWrapper1(std::string filename, bool useCache)
   {
      ParticleSystem::LoadFile(filename,useCache); 
   }
   
   void LoadFileWrapper2(std::string filename)
   {
      ParticleSystem::LoadFile(filename);
   }

protected:

   PyObject* mSelf;
};

void initParticleSystemBindings()
{
   ParticleSystem* (*ParticleSystemGI1)(int) = &ParticleSystem::GetInstance;
   ParticleSystem* (*ParticleSystemGI2)(std::string) = &ParticleSystem::GetInstance;

   class_<ParticleSystem, bases<Transformable, Loadable>, osg::ref_ptr<ParticleSystemWrap>, boost::noncopyable>("ParticleSystem", init<optional<std::string> >())
      .def("GetInstanceCount", &ParticleSystem::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ParticleSystemGI1, return_internal_reference<>())
      .def("GetInstance", ParticleSystemGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &ParticleSystemWrap::LoadFileWrapper1)
      .def("LoadFile", &ParticleSystemWrap::LoadFileWrapper2)
      .def("SetEnabled", &ParticleSystem::SetEnabled)
      .def("IsEnabled", &ParticleSystem::IsEnabled)
      .def("SetParentRelative", &ParticleSystem::SetParentRelative)
      .def("IsParentRelative", &ParticleSystem::IsParentRelative);
}

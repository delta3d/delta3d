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
      :  ParticleSystem( name ),
         mSelf(self)
   {}

   virtual bool LoadFile(std::string filename, bool useCache = true)
   {
      return call_method<bool>(mSelf, "LoadFile", filename, useCache);
   }

   void DefaultLoadFile(std::string filename, bool useCache)
   {
      return ParticleSystem::LoadFile(filename, useCache);
   }

   virtual void AddedToScene( Scene *scene )
   {
      call_method<void>(mSelf, "AddedToScene"); 
   }

   void DefaultAddedToScene( Scene *scene )
   {
      ParticleSystem::AddedToScene(scene);
   }
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initParticleSystemBindings()
{
   ParticleSystem* (*ParticleSystemGI1)(int) = &ParticleSystem::GetInstance;
   ParticleSystem* (*ParticleSystemGI2)(std::string) = &ParticleSystem::GetInstance;

   class_<ParticleSystem, bases<Transformable, Loadable>, osg::ref_ptr<ParticleSystem> >("ParticleSystem", init<optional<std::string> >())
      .def("GetInstanceCount", &ParticleSystem::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ParticleSystemGI1, return_internal_reference<>())
      .def("GetInstance", ParticleSystemGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &ParticleSystem::LoadFile, LF_overloads())
      .def("LoadFile", &ParticleSystem::LoadFile, &ParticleSystemWrap::DefaultLoadFile)
      .def("AddedToScene", &ParticleSystem::AddedToScene, &ParticleSystemWrap::AddedToScene);
      .def("SetEnabled", &ParticleSystem::SetEnabled)
      .def("IsEnabled", &ParticleSystem::IsEnabled)
      .def("SetParentRelative", &ParticleSystem::SetParentRelative)
      .def("IsParentRelative", &ParticleSystem::IsParentRelative);
}

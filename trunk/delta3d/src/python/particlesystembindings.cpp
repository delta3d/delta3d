// particlesystembindings.cpp: ParticleSystem binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/particlesystem.h"

using namespace boost::python;
using namespace dtCore;
/*
class ParticleSystemWrap : public ParticleSystem
{

public:
   ParticleSystemWrap(PyObject* self, std::string name = "ParticleSystem")
      :  ParticleSystem( name ),
         mSelf(self)
   {}

   virtual osg::Node* LoadFile(std::string filename, bool useCache=false)
   {
      call_method<osg::Node*>(mSelf, "LoadFile", filename, useCache);
   }

   osg::Node* DefaultLoadFile(std::string filename)
   {
      ParticleSystem::LoadFile(filename);
   }

protected:

   PyObject* mSelf;
};
*/
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
      .def("LoadFile", &ParticleSystem::LoadFile, LF_overloads()[return_internal_reference<>()])
      .def("SetEnabled", &ParticleSystem::SetEnabled)
      .def("IsEnabled", &ParticleSystem::IsEnabled)
      .def("SetParentRelative", &ParticleSystem::SetParentRelative)
      .def("IsParentRelative", &ParticleSystem::IsParentRelative);
}

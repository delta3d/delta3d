// soundeffectbinderbindings.cpp: SoundEffectBinder binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtAudio/soundeffectbinder.h"

using namespace boost::python;
using namespace dtCore;
using namespace dtAudio;

void initSoundEffectBinderBindings()
{
   SoundEffectBinder* (*SoundEffectBinderGI1)(int) = &SoundEffectBinder::GetInstance;
   SoundEffectBinder* (*SoundEffectBinderGI2)(std::string) = &SoundEffectBinder::GetInstance;

   class_<SoundEffectBinder, bases<Base>, dtCore::RefPtr<SoundEffectBinder> >("SoundEffectBinder", init<optional<std::string> >())
      .def("GetInstanceCount", &SoundEffectBinder::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SoundEffectBinderGI1, return_internal_reference<>())
      .def("GetInstance", SoundEffectBinderGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Initialize", &SoundEffectBinder::Initialize)
      .def("Shutdown", &SoundEffectBinder::Shutdown)
      .def("AddEffectManager", &SoundEffectBinder::AddEffectManager)
      .def("RemoveEffectManager", &SoundEffectBinder::RemoveEffectManager)
      .def("AddEffectTypeMapping", &SoundEffectBinder::AddEffectTypeMapping)
      .def("RemoveEffectTypeMapping", &SoundEffectBinder::RemoveEffectTypeMapping);
}

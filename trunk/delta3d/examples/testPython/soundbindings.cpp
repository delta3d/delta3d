// soundbindings.cpp: Sound binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "sound.h"

using namespace boost::python;
using namespace P51;

void initSoundBindings()
{
   Sound* (*SoundGI1)(int) = &Sound::GetInstance;
   Sound* (*SoundGI2)(std::string) = &Sound::GetInstance;

   class_<Sound, bases<Transformable> >("Sound", init<optional<std::string> >())
      .def("GetInstanceCount", &Sound::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SoundGI1, return_internal_reference<>())
      .def("GetInstance", SoundGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetListenerTransform", &Sound::SetListenerTransform)
      .staticmethod("SetListenerTransform")
      .def("GetListenerTransform", &Sound::GetListenerTransform)
      .staticmethod("GetListenerTransform")
      .def("SetListenerGain", &Sound::SetListenerGain)
      .staticmethod("SetListenerGain")
      .def("GetListenerGain", &Sound::GetListenerGain)
      .staticmethod("GetListenerGain")
      .def("LoadFile", &Sound::LoadFile)
      .def("GetFilename", &Sound::GetFilename)
      .def("SetGain", &Sound::SetGain)
      .def("GetGain", &Sound::GetGain)
      .def("SetLooping", &Sound::SetLooping)
      .def("IsLooping", &Sound::IsLooping)
      .def("Play", &Sound::Play)
      .def("IsPlaying", &Sound::IsPlaying)
      .def("Pause", &Sound::Pause)
      .def("Stop", &Sound::Stop)
      .def("Rewind", &Sound::Rewind);
}

// audiomanagerbindings.cpp: AudioManager binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "audiomanager.h"

using namespace boost::python;
using namespace dtCore;
using namespace dtAudio;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(C_overloads, Config, 0, 1)

void initAudioManagerBindings()
{
   Sound* (*SoundGI1)(int) = &Sound::GetInstance;
   Sound* (*SoundGI2)(std::string) = &Sound::GetInstance;

   {
      scope Sound_scope = class_<Sound, bases<Transformable>, osg::ref_ptr<Sound>, boost::noncopyable>("Sound", no_init)
         .def("GetInstanceCount", &Sound::GetInstanceCount)
         .staticmethod("GetInstanceCount")
         .def("GetInstance", SoundGI1, return_internal_reference<>())
         .def("GetInstance", SoundGI2, return_internal_reference<>())
         .staticmethod("GetInstance")
         .def("GetFilename", &Sound::GetFilename)
         .def("LoadFile", &Sound::LoadFile)
         .def("UnloadFile", &Sound::UnloadFile)
         .def("Play", &Sound::Play)
         .def("Pause", &Sound::Pause)
         .def("Stop", &Sound::Stop)
         .def("Rewind", &Sound::Rewind)
         .def("IsPlaying", &Sound::IsPlaying)
         .def("IsPaused", &Sound::IsPaused)
         .def("IsStopped", &Sound::IsStopped)
         .def("SetLooping", &Sound::SetLooping)
         .def("IsLooping", &Sound::IsLooping)
         .def("ListenerRelative", &Sound::ListenerRelative)
         .def("IsListenerRelative", &Sound::IsListenerRelative)
         .def("SetGain", &Sound::SetGain)
         .def("GetGain", &Sound::GetGain)
         .def("SetPitch", &Sound::SetPitch)
         .def("GetPitch", &Sound::GetPitch)
         .def("SetPosition", &Sound::SetPosition)
         .def("GetPosition", &Sound::GetPosition)
         .def("SetDirection", &Sound::SetDirection)
         .def("GetDirection", &Sound::GetDirection)
         .def("SetVelocity", &Sound::SetVelocity)
         .def("GetVelocity", &Sound::GetVelocity)
         .def("SetMinDistance", &Sound::SetMinDistance)
         .def("GetMinDistance", &Sound::GetMinDistance)
         .def("SetMaxDistance", &Sound::SetMaxDistance)
         .def("GetMaxDistance", &Sound::GetMaxDistance)
         .def("SetRolloffFactor", &Sound::SetRolloffFactor)
         .def("GetRolloffFactor", &Sound::GetRolloffFactor)
         .def("SetMinGain", &Sound::SetMinGain)
         .def("GetMinGain", &Sound::GetMinGain)
         .def("SetMaxGain", &Sound::SetMaxGain)
         .def("GetMaxGain", &Sound::GetMaxGain)
         .def_readonly("kCommand", &Sound::kCommand);
         
      enum_<Sound::Command>("Command")
         .value("NONE", Sound::NONE)
         .value("LOAD", Sound::LOAD)
         .value("UNLOAD", Sound::UNLOAD)
         .value("PLAY", Sound::PLAY)
         .value("PAUSE", Sound::PAUSE)
         .value("STOP", Sound::STOP)
         .value("REWIND", Sound::REWIND)
         .value("LOOP", Sound::LOOP)
         .value("UNLOOP", Sound::UNLOOP)
         .value("QUEUE", Sound::QUEUE)
         .value("GAIN", Sound::GAIN)
         .value("PITCH", Sound::PITCH)
         .value("POSITION", Sound::POSITION)
         .value("DIRECTION", Sound::DIRECTION)
         .value("VELOCITY", Sound::VELOCITY)
         .value("ABS", Sound::ABS)
         .value("REL", Sound::REL)
         .value("MIN_DIST", Sound::MIN_DIST)
         .value("MAX_DIST", Sound::MAX_DIST)
         .value("ROL_FACT", Sound::ROL_FACT)
         .value("MIN_GAIN", Sound::MIN_GAIN)
         .value("MAX_GAIN", Sound::MAX_GAIN)
         .value("kNumCommands", Sound::kNumCommands)
         .export_values();
   }
     
   AudioManager* (*AudioManagerGI1)(int) = &AudioManager::GetInstance;
   AudioManager* (*AudioManagerGI2)(std::string) = &AudioManager::GetInstance;
   
   class_<AudioManager, bases<Base>, osg::ref_ptr<AudioManager>, boost::noncopyable>("AudioManager", no_init)
      .def("GetInstanceCount", &AudioManager::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", AudioManagerGI1, return_internal_reference<>())
      .def("GetInstance", AudioManagerGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Instantiate", &AudioManager::Instantiate)
      .staticmethod("Instantiate")
      .def("Destroy", &AudioManager::Destroy)
      .staticmethod("Destroy")
      .def("GetManager", &AudioManager::GetManager, return_internal_reference<>())
      .staticmethod("GetManager")
      .def("GetListener", &AudioManager::GetListener, return_internal_reference<>())
      .staticmethod("GetListener")
      .def("Config", &AudioManager::Config, C_overloads())
      .def("NewSound", &AudioManager::NewSound, return_internal_reference<>())
      .def("FreeSound", &AudioManager::FreeSound)
      .def("LoadWaveFile", &AudioManager::LoadWaveFile)
      .def("UnloadWaveFile", &AudioManager::UnloadWaveFile);
   
   {
      scope ACD_scope = class_<AudioConfigData>("AudioConfigData", init<optional<unsigned int, bool, unsigned int> >())
         .def_readwrite("numSources", &AudioConfigData::numSources)
         .def_readwrite("eax", &AudioConfigData::eax)
         .def_readwrite("distancemodel", &AudioConfigData::distancemodel);
      
      enum_<AudioConfigData::DistanceModel>("DistanceModel")
         .value("dmNONE", AudioConfigData::dmNONE)
         .value("dmINVERSE", AudioConfigData::dmINVERSE)
         .value("dmINVCLAMP", AudioConfigData::dmINVCLAMP)
         .export_values();
   }
}
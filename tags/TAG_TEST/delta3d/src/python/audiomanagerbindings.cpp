// audiomanagerbindings.cpp: AudioManager binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtAudio/audiomanager.h>

using namespace boost::python;
using namespace dtCore;
using namespace dtAudio;

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(C_overloads, Config, 0, 1)

void AudioManagerInstantiate1(const std::string& n="AudioManager") 
{
    dtAudio::AudioManager::Instantiate(n);
}

void AudioManagerInstantiate2() 
{
    dtAudio::AudioManager::Instantiate();
}


void initAudioManagerBindings()
{
   Sound* (*SoundGI1)(int) = &Sound::GetInstance;
   Sound* (*SoundGI2)(std::string) = &Sound::GetInstance;

   void (Sound::*SetPosition1)( const osg::Vec3& ) = &Sound::SetPosition;

   void (Sound::*GetPosition1)( osg::Vec3& ) const = &Sound::GetPosition;

   void (Sound::*SetDirection1)( const osg::Vec3& ) = &Sound::SetDirection;

   void (Sound::*GetDirection1)( osg::Vec3& ) const = &Sound::GetDirection;

   void (Sound::*SetVelocity1)( const osg::Vec3& ) = &Sound::SetVelocity;

   void (Sound::*GetVelocity1)( osg::Vec3& ) const = &Sound::GetVelocity;


   {
      scope Sound_scope = class_<Sound, bases<Transformable>, dtCore::RefPtr<Sound>, boost::noncopyable>("Sound", no_init)
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
         .def("SetListenerRelative", &Sound::SetListenerRelative)
         .def("IsListenerRelative", &Sound::IsListenerRelative)
         .def("SetGain", &Sound::SetGain)
         .def("GetGain", &Sound::GetGain)
         .def("SetPitch", &Sound::SetPitch)
         .def("GetPitch", &Sound::GetPitch)
         .def("SetPosition", SetPosition1)
         .def("GetPosition", GetPosition1)
         .def("SetDirection", SetDirection1)
         .def("GetDirection", GetDirection1)
         .def("SetVelocity", SetVelocity1)
         .def("GetVelocity", GetVelocity1)
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
         .value("MAX_DIST", Sound::MAX_DIST)
         .value("ROL_FACT", Sound::ROL_FACT)
         .value("MIN_GAIN", Sound::MIN_GAIN)
         .value("MAX_GAIN", Sound::MAX_GAIN)
         .value("kNumCommands", Sound::kNumCommands)
         .export_values();
   }

   AudioManager* (*AudioManagerGI1)(int) = &AudioManager::GetInstance;
   AudioManager* (*AudioManagerGI2)(std::string) = &AudioManager::GetInstance;
   
   class_<AudioManager, bases<Base>, dtCore::RefPtr<AudioManager>, boost::noncopyable>("AudioManager", no_init)
      .def("GetInstanceCount", &AudioManager::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", AudioManagerGI1, return_internal_reference<>())
      .def("GetInstance", AudioManagerGI2, return_internal_reference<>())
      .staticmethod("GetInstance")      
      .def("Instantiate", &AudioManagerInstantiate1)
      .def("Instantiate", &AudioManagerInstantiate2)
      .staticmethod("Instantiate")
      .def("Destroy", &AudioManager::Destroy)
      .staticmethod("Destroy")
      .def("GetListener", &AudioManager::GetListener, return_value_policy<reference_existing_object>())
      .staticmethod("GetListener")
      //.def("Config", &AudioManager::Config, C_overloads())
      .def("NewSound", &AudioManager::NewSound, return_internal_reference<>())
      .def("FreeSound", &AudioManager::FreeSound)
      .def("LoadFile", &AudioManager::LoadFile)
      .def("UnloadFile", &AudioManager::UnloadFile);
   
   /*{
      scope ACD_scope = class_<AudioConfigData>("AudioConfigData", init<optional<unsigned int, bool, unsigned int> >())
         .def_readwrite("numSources", &AudioConfigData::numSources)
         .def_readwrite("eax", &AudioConfigData::eax)
         .def_readwrite("distancemodel", &AudioConfigData::distancemodel);
      
      enum_<AudioConfigData::DistanceModel>("DistanceModel")
         .value("dmNONE", AudioConfigData::dmNONE)
         .value("dmINVERSE", AudioConfigData::dmINVERSE)
         .value("dmINVCLAMP", AudioConfigData::dmINVCLAMP)
         .export_values();
   }*/
}

// recorderbindings.cpp: Recorder binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/recorder.h"

using namespace boost::python;
using namespace dtCore;

/// \todo update this to work with new recorder class interface.
// class RecordableWrap : public Recordable
// {
//    public:
   
//       RecordableWrap(PyObject* self)
//          : mSelf(self)
//       {}
      
//       virtual StateFrame* GenerateKeyFrame()
//       {
//          return call_method<StateFrame*>(mSelf, "GenerateKeyFrame");
//       }
      
//       virtual StateFrame* DeserializeFrame(TiXmlElement* element)
//       {
//          return call_method<StateFrame*>(mSelf, "DeserializeFrame");
//       }
      
//    private:
      
//       PyObject* mSelf;
// };

// class StateFrameWrap : public StateFrame
// {
//    public:
   
//       StateFrameWrap(PyObject* self, Recordable* source)
//          : StateFrame(source),
//            mSelf(self)
//       {}
      
//       virtual void ReapplyToSource()
//       {
//          return call_method<void>(mSelf, "ReapplyToSource");
//       }
      
//       virtual TiXmlElement* Serialize() const
//       {
//          return call_method<TiXmlElement*>(mSelf, "Serialize");
//       }
      
//    private:
      
//       PyObject* mSelf;
// };

// void initRecorderBindings()
// {
//    Recorder* (*RecorderGI1)(int) = &Recorder::GetInstance;
//    Recorder* (*RecorderGI2)(std::string) = &Recorder::GetInstance;

//    enum_<RecorderState>("RecorderState")
//       .value("RecorderStopped", RecorderStopped)
//       .value("RecorderRecording", RecorderRecording)
//       .value("RecorderPlaying", RecorderPlaying)
//       .export_values();
      
//    class_<Recorder, bases<Base>, dtCore::RefPtr<Recorder> >("Recorder", init<optional<std::string> >())
//       .def("GetInstanceCount", &Recorder::GetInstanceCount)
//       .staticmethod("GetInstanceCount")
//       .def("GetInstance", RecorderGI1, return_internal_reference<>())
//       .def("GetInstance", RecorderGI2, return_internal_reference<>())
//       .staticmethod("GetInstance")
//       .def("AddSource", &Recorder::AddSource)
//       .def("RemoveSource", &Recorder::RemoveSource)
//       .def("Record", &Recorder::Record)
//       .def("Play", &Recorder::Play)
//       .def("Stop", &Recorder::Stop)
//       .def("GetState", &Recorder::GetState)
//       .def("SaveFile", &Recorder::SaveFile)
//       .def("LoadFile", &Recorder::LoadFile);

//    class_<Recordable, RecordableWrap, boost::noncopyable>("Recordable")
//       .def("IncrementRecorderCount", &Recordable::IncrementRecorderCount)
//       .def("DecrementRecorderCount", &Recordable::DecrementRecorderCount)
//       .def("IsBeingRecorded", &Recordable::IsBeingRecorded)
//       .def("GenerateKeyFrame", &Recordable::GenerateKeyFrame, return_internal_reference<>())
//       .def("DeserializeFrame", &Recordable::DeserializeFrame, return_internal_reference<>());
      
//    class_<StateFrame, StateFrameWrap, boost::noncopyable>("StateFrame", init<Recordable*>())
//       .def("GetSource", &StateFrame::GetSource, return_internal_reference<>())
//       .def("ReapplyToSource", &StateFrame::ReapplyToSource)
//       .def("Serialize", &StateFrame::Serialize, return_internal_reference<>());
// }

// inputmapperbindings.cpp: InputMapper binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/inputmapper.h"

using namespace boost::python;
using namespace dtCore;

class InputMapperCallbackWrap : public InputMapperCallback
{
   public:
   
      InputMapperCallbackWrap(PyObject* self)
         : mSelf(self)
      {}
      
      virtual void ButtonMappingAcquired(ButtonMapping* mapping)
      {
         call_method<void>(mSelf, "ButtonMappingAcquired", mapping);
      }
      
      void DefaultButtonMappingAcquired(ButtonMapping* mapping)
      {
         InputMapperCallback::ButtonMappingAcquired(mapping);
      }
      
      virtual void AxisMappingAcquired(AxisMapping* mapping)
      {
         call_method<void>(mSelf, "AxisMappingAcquired", mapping);
      }
      
      void DefaultAxisMappingAcquired(AxisMapping* mapping)
      {
         InputMapperCallback::AxisMappingAcquired(mapping);
      }
      
   private:
      
      PyObject* mSelf;
};

void initInputMapperBindings()
{
   InputMapper* (*InputMapperGI1)(int) = &InputMapper::GetInstance;
   InputMapper* (*InputMapperGI2)(std::string) = &InputMapper::GetInstance;

   class_<InputMapper, bases<Base>, dtCore::RefPtr<InputMapper> >("InputMapper", init<optional<std::string> >())
      .def("GetInstanceCount", &InputMapper::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", InputMapperGI1, return_internal_reference<>())
      .def("GetInstance", InputMapperGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddDevice", &InputMapper::AddDevice)
      .def("RemoveDevice", &InputMapper::RemoveDevice)
      .def("GetNumDevices", &InputMapper::GetNumDevices)
      .def("GetDevice", &InputMapper::GetDevice, return_internal_reference<>())
      .def("SetCancelButton", &InputMapper::SetCancelButton)
      .def("GetCancelButton", &InputMapper::GetCancelButton, return_internal_reference<>())
      .def("AcquireButtonMapping", &InputMapper::AcquireButtonMapping)
      .def("AcquireAxisMapping", &InputMapper::AcquireAxisMapping);
      
   class_<InputMapperCallback, InputMapperCallbackWrap, boost::noncopyable>("InputMapperCallback")
      .def("ButtonMappingAcquired", &InputMapperCallback::ButtonMappingAcquired, &InputMapperCallbackWrap::DefaultButtonMappingAcquired)
      .def("AxisMappingAcquired", &InputMapperCallback::AxisMappingAcquired, &InputMapperCallbackWrap::DefaultAxisMappingAcquired);
}

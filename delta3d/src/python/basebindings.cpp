// basebindings.cpp: Base binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "base.h"

using namespace boost::python;
using namespace boost::python::detail;
using namespace dtCore;

class BaseWrap : public Base
{
   public:

      BaseWrap(PyObject* self)
         : mSelf(self)
      {}

      virtual void OnMessage(MessageData* data)
      {
         call_method<void>(mSelf, "OnMessage", data); 
      }

      void DefaultOnMessage(MessageData* data)
      {
         Base::OnMessage(data);
      }

   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SM_overloads, SendMessage, 0, 2)

void initBaseBindings()
{
   Base* (*BaseGI1)(int) = &Base::GetInstance;
   Base* (*BaseGI2)(std::string) = &Base::GetInstance;

   scope baseScope = class_<Base, osg::ref_ptr<BaseWrap>, boost::noncopyable>("Base")
      .def("GetInstanceCount", &Base::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", BaseGI1, return_internal_reference<>())
      .def("GetInstance", BaseGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetName", &Base::SetName)
      .def("GetName", &Base::GetName)
      .def("OnMessage", &Base::OnMessage, &BaseWrap::DefaultOnMessage)
      .def("AddSender", &Base::AddSender)
      .def("RemoveSender", &Base::RemoveSender)
      .def("SendMessage", &Base::SendMessage, SM_overloads());

   class_<Base::MessageData>("MessageData")
      .def_readwrite("message", &Base::MessageData::message)
      .def_readwrite("sender", &Base::MessageData::sender)
      .def_readwrite("userData", &Base::MessageData::userData);
}

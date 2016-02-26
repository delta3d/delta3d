// basebindings.cpp: Base binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/base.h>

using namespace boost::python;
using namespace boost::python::detail;
using namespace dtCore;

// Since void* has no real type, and Python requires strong typing,
// we call upon the VoodooPython... err.. BoostPython spirits to guide
// us through the conflict. Basically, we are creating a pseudo-type,
// void_, that will take the place of a void* in the Python world.
// So then we wrap SendMessage and MessageData to use void_'s in Python.
//
// -osb
struct void_
{
};

class BaseWrap : public Base
{
   public:

      BaseWrap(PyObject* self)
         : mSelf(self)
      {}

      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

      {
         call_method<void>(mSelf, "OnMessage", data); 
      }

      void DefaultOnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

      {
         Base::OnMessage(data);
      }

      void SendMessageWrap( const std::string& message="", void_* data = 0 )
      {
         SendMessage( message, data );
      }
   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(void_);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SM_overloads, SendMessageWrap, 0, 2)

void initBaseBindings()
{
   Base* (*BaseGI1)(int) = &Base::GetInstance;
   Base* (*BaseGI2)(std::string) = &Base::GetInstance;

   scope baseScope = class_<Base, dtCore::RefPtr<BaseWrap>, boost::noncopyable>("Base")
      .def("GetInstanceCount", &Base::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", BaseGI1, return_internal_reference<>())
      .def("GetInstance", BaseGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetName", &Base::SetName)
      .def("GetName", &Base::GetName, return_value_policy<copy_const_reference>())
      .def("OnMessage", &Base::OnMessage, &BaseWrap::DefaultOnMessage)
      .def("AddSender", &Base::AddSender)
      .def("RemoveSender", &Base::RemoveSender)
      .def("SendMessage", &BaseWrap::SendMessageWrap, SM_overloads())
      ;

   class_<BaseWrap::MessageData>("MessageData")
      .def_readwrite("message", &BaseWrap::MessageData::message)
      .def_readwrite("sender", &BaseWrap::MessageData::sender)

      // This still doesn't work. One can probably make some sort of wrapper
      // class for MessageData that does the conversion, but it would need to use
      // some sort of Get/SetUserData functions and then use Boost.Python's
      // .add_property to bind them to the userData member.
      //.def_readwrite("userData", &BaseWrap::MessageData::userData)
      ;
}

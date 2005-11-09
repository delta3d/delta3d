#include <python/dtpython.h>
#include <dtDAL/actorproxy.h>
#include <boost/python/register_ptr_to_python.hpp>

using namespace boost::python;
using namespace dtDAL;

class ActorProxyWrap : public ActorProxy, public wrapper<ActorProxy>
{
   public:
   
      dtCore::DeltaDrawable* GetActor()
      {
         if( override GetActor = this->get_override("GetActor") )
         {
            return GetActor();
         }
         return ActorProxy::GetActor();
      }

      dtCore::DeltaDrawable* DefaultGetActor()
      {
         return this->ActorProxy::GetActor();
      }
};

class ActorProxyWrapper: public ActorProxy
{
   public:
   ActorProxyWrapper(PyObject* self_) : ActorProxy(), self(self_) {}
   ~ActorProxyWrapper() {}

   dtCore::DeltaDrawable* GetActor() { return call_method<dtCore::DeltaDrawable*>(self, "GetActor"); }    
   dtCore::DeltaDrawable* DefaultGetActor() { return ActorProxy::GetActor(); }    
   PyObject* self;
};

void initActorProxyBindings()
{
   dtCore::DeltaDrawable* (ActorProxy::*GA1)() = (dtCore::DeltaDrawable* (ActorProxy::*)())&ActorProxy::GetActor;

   class_< ActorProxyWrap, dtCore::RefPtr<ActorProxyWrap>, boost::noncopyable >( "ActorProxy", no_init )
   //class_< ActorProxy, osg::ref_ptr<ActorProxyWrapper>, boost::noncopyable >( "ActorProxy", no_init )
      .def( "GetName", &ActorProxy::GetName, return_value_policy<copy_const_reference>() )
      .def( "SetName", &ActorProxy::SetName ) 
      .def( "GetClassName", &ActorProxy::GetClassName, return_internal_reference<>() )
      .def( "IsInstanceOf", &ActorProxy::IsInstanceOf )
      //.def( "GetActor", GA1, &ActorProxyWrapper::DefaultGetActor, return_internal_reference<>() )
      .def( "GetActor", GA1, &ActorProxyWrap::DefaultGetActor, return_internal_reference<>() )
      ;

   register_ptr_to_python< osg::ref_ptr< ActorProxy > >();
}

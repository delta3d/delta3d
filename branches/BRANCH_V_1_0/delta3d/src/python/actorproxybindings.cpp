#include <python/dtpython.h>
#include <dtDAL/actorproxy.h>
#include <boost/python/register_ptr_to_python.hpp>

using namespace boost::python;
using namespace dtDAL;

class ActorProxyWrap : public ActorProxy, wrapper<ActorProxy>
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

void initActorProxyBindings()
{
   dtCore::DeltaDrawable* (ActorProxy::*GA1)() = (dtCore::DeltaDrawable* (ActorProxy::*)())&ActorProxy::GetActor;

   class_< ActorProxy, osg::ref_ptr<ActorProxy>, boost::noncopyable >( "ActorProxy", no_init )
      .def( "GetName", &ActorProxy::GetName, return_internal_reference<>() )
      .def( "SetName", &ActorProxy::SetName ) 
      .def( "GetClassName", &ActorProxy::GetClassName, return_internal_reference<>() )
      .def( "IsInstanceOf", &ActorProxy::IsInstanceOf )
      //.def( "GetActor", GA1, &ActorProxyWrap::DefaultGetActor, return_internal_reference<>() )
      ;

   register_ptr_to_python< osg::ref_ptr< ActorProxy > >();
}

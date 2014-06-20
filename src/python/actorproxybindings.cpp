#include <python/dtpython.h>
#include <dtCore/actorproxy.h>
#include <dtCore/actorproperty.h>

#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace dtCore;

typedef std::vector< ActorProperty* > ActorPropertyVector;

class ActorProxyWrap : public ActorProxy, public wrapper<ActorProxy>
{
   public:

	   dtCore::DeltaDrawable* GetActor()
	   {
		   if( override GetActor = this->get_override("GetActor") )
		   {
            #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            return call<dtCore::DeltaDrawable*>( GetDrawable.ptr() );
            #else
            return GetActor();
            #endif
		   }
		   return ActorProxy::GetDrawable();
	   }

	   dtCore::DeltaDrawable* DefaultGetActor()
	   {
		   return this->ActorProxy::GetDrawable();
	   }

      const dtCore::DeltaDrawable* GetActor() const
	   {
		   if( override GetActor = this->get_override("GetActor") )
		   {
            #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            return call<const dtCore::DeltaDrawable*>( GetDrawable.ptr() );            
            #else
			   return GetActor();
            #endif
		   }
		   return ActorProxy::GetActor();
	   }

	   const dtCore::DeltaDrawable* DefaultGetActor() const
	   {
		   return this->ActorProxy::GetActor();
	   }
};

void initActorProxyBindings()
{
   class_< ActorPropertyVector >( "ActorPropertyVector" )
      .def(vector_indexing_suite< ActorPropertyVector, true >())
      ;

   ActorProperty* (ActorProxy::*GetPropertyNonConst)( const std::string& ) = &ActorProxy::GetProperty;
   const ActorProperty* (ActorProxy::*GetPropertyConst)( const std::string& ) const = &ActorProxy::GetProperty;

   void (ActorProxy::*GetPropertyListNonConst)( ActorPropertyVector& ) = &ActorProxy::GetPropertyList;

   dtCore::DeltaDrawable* (ActorProxy::*GetActorNonConst)() = &ActorProxy::GetDrawable;
   const dtCore::DeltaDrawable* (ActorProxy::*GetActorConst)() const = &ActorProxy::GetActor;

   dtCore::DeltaDrawable* (ActorProxyWrap::*DefaultGetActorNonConst)() = &ActorProxyWrap::DefaultGetActor;
   const dtCore::DeltaDrawable* (ActorProxyWrap::*DefaultGetActorConst)() const = &ActorProxyWrap::DefaultGetActor;

	class_< ActorProxyWrap, dtCore::RefPtr<ActorProxyWrap>, boost::noncopyable >( "ActorProxy", no_init )
		.def( "GetName", &ActorProxy::GetName, return_value_policy<copy_const_reference>() )
		.def( "SetName", &ActorProxy::SetName ) 
		.def( "GetClassName", &ActorProxy::GetClassName, return_value_policy<copy_const_reference>() )
		.def( "IsInstanceOf", &ActorProxy::IsInstanceOf )
      .def( "IsGameActorProxy", &ActorProxy::IsGameActorProxy )
      .def( "AddProperty", &ActorProxy::AddProperty, with_custodian_and_ward<1, 2>() )
      .def( "GetProperty", GetPropertyNonConst, return_internal_reference<>() )
      .def( "GetProperty", GetPropertyConst, return_internal_reference<>() )
		.def( "GetPropertyList", GetPropertyListNonConst )
		.def( "GetActor", GetActorNonConst, DefaultGetActorNonConst, return_internal_reference<>() )
      .def( "GetActor", GetActorConst, DefaultGetActorConst, return_internal_reference<>() )
		;

	register_ptr_to_python< dtCore::RefPtr< ActorProxy > >();
}

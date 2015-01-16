#include <python/dtpython.h>
#include <dtCore/datatype.h>
#include <dtCore/actorproperty.h>
#include <dtCore/enginepropertytypes.h>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace dtCore;

class ActorPropertyWrap : public ActorProperty, public wrapper<ActorProperty>
{
   protected:

   ActorPropertyWrap(dtCore::DataType &dataType, 
		     const std::string &name,
                     const std::string &label,
                     const std::string &desc,
                     const std::string &groupName,
                     bool  readOnly = false) :
      ActorProperty(dataType, name,label,desc,groupName,readOnly)
   {
   }

   virtual ~ActorPropertyWrap()
   {
   }
   
   public:

      DataType& GetPropertyType() const
      {
         #if defined( _MSC_VER ) // MSVC 7.1 & 8.0
         return call<DataType&>( this->get_override("GetPropertyType").ptr() );
         #else
         return this->get_override( "GetPropertyType" )();
         #endif
      }

      void CopyFrom( ActorProperty* otherProp )
      {
         #if defined( _MSC_VER ) && ( _MSC_VER >= 1400 ) // MSVC 8.0
         call<void>( this->get_override("CopyFrom").ptr(), otherProp );
         #else
         this->get_override( "CopyFrom" )( otherProp );
         #endif
      }
   	
      const std::string ToString()
      {
         #if defined( _MSC_VER ) && ( _MSC_VER >= 1400 ) // MSVC 8.0
         return call<const std::string&>( this->get_override("ToString").ptr() );
         #else
		   return this->get_override("ToString")();
         #endif
      }
};

void initActorPropertyBindings()
{
   class_< ActorPropertyWrap, dtCore::RefPtr< ActorPropertyWrap >, boost::noncopyable >( "ActorProperty", no_init )
      .def( "GetPropertyType", pure_virtual( &ActorProperty::GetPropertyType ), return_internal_reference<>() ) // copy_non_const_reference?
      //.def( "CopyFrom", pure_virtual( &ActorProperty::CopyFrom ) )
      .def( "GetName", &ActorProperty::GetName, return_value_policy<copy_const_reference>() )
      .def( "GetLabel", &ActorProperty::GetLabel, return_value_policy<copy_const_reference>() )
      .def( "GetDescription", &ActorProperty::GetDescription, return_value_policy<copy_const_reference>() )
      .def( "GetGroupName", &ActorProperty::GetGroupName, return_value_policy<copy_const_reference>() )
      .def( "IsReadOnly", &ActorProperty::IsReadOnly )
      .def( "ToString", &ActorProperty::ToString )
      .def( "SetNumberPrecision", &ActorProperty::SetNumberPrecision )
      .def( "GetNumberPrecision", &ActorProperty::GetNumberPrecision )
		;

   ActorProxy* (ActorActorProperty::*GetValueConst)() const = &ActorActorProperty::GetValue;

   class_<  ActorActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< ActorActorProperty >, 
            boost::noncopyable >( "ActorActorProperty", no_init )
      .def( "SetValue", &ActorActorProperty::SetValue, with_custodian_and_ward< 1, 2 >() )
      //.def( "GetValue", GetValueNonConst, return_internal_reference<>() )
      .def( "GetValue", GetValueConst, return_internal_reference<>() )
      .def( "GetDesiredActorClass", &ActorActorProperty::GetDesiredActorClass, return_value_policy<copy_const_reference>() )
      ;

   class_<  FloatActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< FloatActorProperty >,
            boost::noncopyable >( "FloatActorProperty", no_init )
      .def( "SetValue", &FloatActorProperty::SetValue )
      .def( "GetValue", &FloatActorProperty::GetValue )
      ;

   class_<  DoubleActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< DoubleActorProperty >,
            boost::noncopyable >( "DoubleActorProperty", no_init )
      .def( "SetValue", &DoubleActorProperty::SetValue )
      .def( "GetValue", &DoubleActorProperty::GetValue )
      ;

   class_<  IntActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< IntActorProperty >,
            boost::noncopyable >( "IntActorProperty", no_init )
      .def( "SetValue", &IntActorProperty::SetValue )
      .def( "GetValue", &IntActorProperty::GetValue )
      ;   
   
   class_<  LongActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< LongActorProperty >,
            boost::noncopyable >( "LongActorProperty", no_init )
      .def( "SetValue", &LongActorProperty::SetValue )
      .def( "GetValue", &LongActorProperty::GetValue )
      ;    

   class_<  StringActorProperty,
            bases< ActorProperty >,       
            dtCore::RefPtr< StringActorProperty >,
            boost::noncopyable >( "StringActorProperty", no_init )
      .def( "SetMaxLength", &StringActorProperty::SetMaxLength )
      .def( "GetMaxLength", &StringActorProperty::GetMaxLength )
      .def( "SetValue", &StringActorProperty::SetValue )
      .def( "GetValue", &StringActorProperty::GetValue )
      ;       
   
   class_<  BooleanActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< BooleanActorProperty >,
            boost::noncopyable >( "BooleanActorProperty", no_init )
      .def( "SetValue", &BooleanActorProperty::SetValue )
      .def( "GetValue", &BooleanActorProperty::GetValue )
      ;

   class_<  Vec2ActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< Vec2ActorProperty >,
            boost::noncopyable >( "Vec2ActorProperty", no_init )
      .def( "SetValue", &Vec2ActorProperty::SetValue )
      .def( "GetValue", &Vec2ActorProperty::GetValue )
      ;

      //class_<  Vec2fActorProperty, 
      //      bases< ActorProperty >,
      //      dtCore::RefPtr< Vec2fActorProperty >,
      //      boost::noncopyable >( "Vec2fActorProperty", no_init )
      //.def( "SetValue", &Vec2fActorProperty::SetValue )
      //.def( "GetValue", &Vec2fActorProperty::GetValue )
      //;

   class_<  Vec2dActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< Vec2dActorProperty >,
            boost::noncopyable >( "Vec2dActorProperty", no_init )
      .def( "SetValue", &Vec2dActorProperty::SetValue )
      .def( "GetValue", &Vec2dActorProperty::GetValue )
      ;
      
   class_<  Vec3ActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< Vec3ActorProperty >,
            boost::noncopyable >( "Vec3ActorProperty", no_init )
      .def( "SetValue", &Vec3ActorProperty::SetValue )
      .def( "GetValue", &Vec3ActorProperty::GetValue )
      ;

   //class_<  Vec3fActorProperty, 
   //         bases< ActorProperty >,
   //         dtCore::RefPtr< Vec3fActorProperty >,
   //         boost::noncopyable >( "Vec3fActorProperty", no_init )
   //   .def( "SetValue", &Vec3fActorProperty::SetValue )
   //   .def( "GetValue", &Vec3fActorProperty::GetValue )
   //   ;

   class_<  Vec3dActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< Vec3dActorProperty >,
            boost::noncopyable >( "Vec3dActorProperty", no_init )
      .def( "SetValue", &Vec3dActorProperty::SetValue )
      .def( "GetValue", &Vec3dActorProperty::GetValue )
      ;

   class_<  Vec4ActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< Vec4ActorProperty >,
            boost::noncopyable >( "Vec4ActorProperty", no_init )
      .def( "SetValue", &Vec4ActorProperty::SetValue )
      .def( "GetValue", &Vec4ActorProperty::GetValue )
      ;

   //class_<  Vec4fActorProperty, 
   //         bases< ActorProperty >,
   //         dtCore::RefPtr< Vec4fActorProperty >,
   //         boost::noncopyable >( "Vec4fActorProperty", no_init )
   //   .def( "SetValue", &Vec4fActorProperty::SetValue )
   //   .def( "GetValue", &Vec4fActorProperty::GetValue )
   //   ;

   class_<  Vec4dActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< Vec4dActorProperty >,
            boost::noncopyable >( "Vec4dActorProperty", no_init )
      .def( "SetValue", &Vec4dActorProperty::SetValue )
      .def( "GetValue", &Vec4dActorProperty::GetValue )
      ;

   class_<  ColorRgbaActorProperty, 
            bases< ActorProperty >,
            dtCore::RefPtr< ColorRgbaActorProperty >,
            boost::noncopyable >( "ColorRgbaActorProperty", no_init )
      .def( "SetValue", &ColorRgbaActorProperty::SetValue )
      .def( "GetValue", &ColorRgbaActorProperty::GetValue )
      ;

	register_ptr_to_python< ActorProperty* >();
   
   // This doesn't seem to want to compile, so not ActorProxy::GetPropertyList const
   // for now.
   //register_ptr_to_python< const ActorProperty* >();
}

#ifndef GET_SET_MACROS
#define GET_SET_MACROS

#include <dtUtil/typetraits.h>

#define DECLARE_PROPERTY_INLINE(PropertyType, PropertyName) \
   private:\
      PropertyType  m ## PropertyName; \
   public: \
      \
      void Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value)\
   {\
      m ## PropertyName = value; \
   };\
      \
      dtUtil::TypeTraits<PropertyType>::const_return_type Get ## PropertyName() const\
   {\
      return m ## PropertyName;\
   };\
      \

#define DECLARE_PROPERTY(PropertyType, PropertyName) \
   private:\
      PropertyType  m ## PropertyName; \
   public: \
      \
      void Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value);\
      \
      dtUtil::TypeTraits<PropertyType>::const_return_type Get ## PropertyName() const;\
      \

#define IMPLEMENT_PROPERTY_GETTER(ClassName, PropertyType, PropertyName) \
      dtUtil::TypeTraits<PropertyType>::const_return_type ClassName::Get ## PropertyName() const\
      {\
         return m ## PropertyName;\
      }\
      \

#define IMPLEMENT_PROPERTY_SETTER(ClassName, PropertyType, PropertyName) \
      void ClassName::Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value)\
      {\
         m ## PropertyName = value;\
      }\
      \

#define IMPLEMENT_PROPERTY(ClassName, PropertyType, PropertyName) \
      IMPLEMENT_PROPERTY_SETTER(ClassName, PropertyType, PropertyName)\
      \
      IMPLEMENT_PROPERTY_GETTER(ClassName, PropertyType, PropertyName)\
      \


#include <dtUtil/typetraits.h>

#define DECLARE_PROPERTY_INLINE(PropertyType, PropertyName) \
   private:\
      PropertyType  m ## PropertyName; \
   public: \
      \
      void Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value)\
   {\
      m ## PropertyName = value; \
   };\
      \
      dtUtil::TypeTraits<PropertyType>::const_return_type Get ## PropertyName() const\
   {\
      return m ## PropertyName;\
   };\
      \

#define DECLARE_PROPERTY(PropertyType, PropertyName) \
   private:\
      PropertyType  m ## PropertyName; \
   public: \
      \
      void Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value);\
      \
      dtUtil::TypeTraits<PropertyType>::const_return_type Get ## PropertyName() const;\
      \

#define IMPLEMENT_PROPERTY_GETTER(ClassName, PropertyType, PropertyName) \
      dtUtil::TypeTraits<PropertyType>::const_return_type ClassName::Get ## PropertyName() const\
      {\
         return m ## PropertyName;\
      }\
      \

#define IMPLEMENT_PROPERTY_SETTER(ClassName, PropertyType, PropertyName) \
      void ClassName::Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value)\
      {\
         m ## PropertyName = value;\
      }\
      \

#define IMPLEMENT_PROPERTY(ClassName, PropertyType, PropertyName) \
      IMPLEMENT_PROPERTY_SETTER(ClassName, PropertyType, PropertyName)\
      \
      IMPLEMENT_PROPERTY_GETTER(ClassName, PropertyType, PropertyName)\
      \

#endif

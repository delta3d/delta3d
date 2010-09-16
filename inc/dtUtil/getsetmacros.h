#ifndef GET_SET_MACROS
#define GET_SET_MACROS

#include <dtUtil/typetraits.h>

/**
 * Change the definition of this to add virtual or static to generated setters
 * Do
 * #undef PROPERTY_MODIFIERS_SETTER
 * #define PROPERTY_MODIFIERS_SETTER virtual
 * DT_DECLARE_ACCESSOR(float, Silliness)
 * DT_DECLARE_ACCESSOR_INLINE(int, NumWidgets)
 * #undef PROPERTY_MODIFIERS_SETTER
 * #define PROPERTY_MODIFIERS_SETTER
 */
#define PROPERTY_MODIFIERS_SETTER

/**
 * Change the definition of this to add virtual or static to generated getters
 * Do
 * #undef PROPERTY_MODIFIERS_GETTER
 * #define PROPERTY_MODIFIERS_GETTER virtual
 * DT_DECLARE_ACCESSOR(float, Silliness)
 * DT_DECLARE_ACCESSOR_INLINE(int, NumWidgets)
 * #undef PROPERTY_MODIFIERS_GETTER
 * #define PROPERTY_MODIFIERS_GETTER
 */
#define PROPERTY_MODIFIERS_GETTER

#define DT_DECLARE_ACCESSOR_INLINE(PropertyType, PropertyName) \
   private:\
      dtUtil::TypeTraits<PropertyType>::value_type  m ## PropertyName; \
   public: \
      \
      PROPERTY_MODIFIERS_SETTER void Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value)\
   {\
      m ## PropertyName = value; \
   };\
      \
      PROPERTY_MODIFIERS_GETTER dtUtil::TypeTraits<PropertyType>::return_type Get ## PropertyName() const\
   {\
      return m ## PropertyName;\
   };\
      \

#define DT_DECLARE_ACCESSOR_GET_SET(PropertyType, PropertyName) \
      \
      PROPERTY_MODIFIERS_SETTER void Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value);\
      \
      PROPERTY_MODIFIERS_GETTER dtUtil::TypeTraits<PropertyType>::return_type Get ## PropertyName() const;\
      \

/** 
  * Macro that will declare a private data member and declare the accessors for 
  * it.  Note, this only creates the method declarations, not the implementations.
  * Example:
  * @code 
  * DT_DECLARE_ACCESSOR(bool, Lighting)
  * 
  * //equates to: 
  * private:
  *    bool mLighting
  * public:
  *    void SetLighting(bool value);
  *    bool GetLighting() const;
  * @endcode
  * @param PropertyType the data type of the class member (e.g., bool, int)
  * @param PropertyName The name of the class member.  Also used for the accessors. 
  *         (e.g.: "Speed" creates "mSpeed", GetSpeed(), and SetSpeed())
  */
#define DT_DECLARE_ACCESSOR(PropertyType, PropertyName) \
   private:\
      dtUtil::TypeTraits<PropertyType>::value_type  m ## PropertyName; \
   public: \
       DT_DECLARE_ACCESSOR_GET_SET(PropertyType, PropertyName)

#define DT_IMPLEMENT_ACCESSOR_GETTER(ClassName, PropertyType, PropertyName) \
      dtUtil::TypeTraits<PropertyType>::return_type ClassName::Get ## PropertyName() const\
      {\
         return m ## PropertyName;\
      }\
      \

#define DT_IMPLEMENT_ACCESSOR_SETTER(ClassName, PropertyType, PropertyName) \
      void ClassName::Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value)\
      {\
         m ## PropertyName = value;\
      }\
      \

#define DT_IMPLEMENT_ACCESSOR(ClassName, PropertyType, PropertyName) \
      DT_IMPLEMENT_ACCESSOR_SETTER(ClassName, PropertyType, PropertyName)\
      \
      DT_IMPLEMENT_ACCESSOR_GETTER(ClassName, PropertyType, PropertyName)\
      \

#endif

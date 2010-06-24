#ifndef GET_SET_MACROS
#define GET_SET_MACROS

#include <dtUtil/typetraits.h>

/**
 * Change the definition of this to add virtual or static to generated setters
 * Do
 * #undef PROPERTY_MODIFIERS_SETTER
 * #define PROPERTY_MODIFIERS_SETTER virtual
 * DECLARE_PROPERTY(float, Silliness)
 * DECLARE_PROPERTY_INLINE(int, NumWidgets)
 * #undef PROPERTY_MODIFIERS_SETTER
 * #define PROPERTY_MODIFIERS_SETTER
 */
#define PROPERTY_MODIFIERS_SETTER

/**
 * Change the definition of this to add virtual or static to generated getters
 * Do
 * #undef PROPERTY_MODIFIERS_GETTER
 * #define PROPERTY_MODIFIERS_GETTER virtual
 * DECLARE_PROPERTY(float, Silliness)
 * DECLARE_PROPERTY_INLINE(int, NumWidgets)
 * #undef PROPERTY_MODIFIERS_GETTER
 * #define PROPERTY_MODIFIERS_GETTER
 */
#define PROPERTY_MODIFIERS_GETTER

#define DECLARE_PROPERTY_INLINE(PropertyType, PropertyName) \
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

/** 
  * Macro that will declare a private data member and declare the accessors for 
  * it.  Note, this only creates the method declarations, not the implementations.
  * Example:
  * @code 
  * DECLARE_PROPERTY(bool, Lighting)
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
#define DECLARE_PROPERTY(PropertyType, PropertyName) \
   private:\
      dtUtil::TypeTraits<PropertyType>::value_type  m ## PropertyName; \
   public: \
      \
      PROPERTY_MODIFIERS_SETTER void Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value);\
      \
      PROPERTY_MODIFIERS_GETTER dtUtil::TypeTraits<PropertyType>::return_type Get ## PropertyName() const;\
      \

#define IMPLEMENT_PROPERTY_GETTER(ClassName, PropertyType, PropertyName) \
      dtUtil::TypeTraits<PropertyType>::return_type ClassName::Get ## PropertyName() const\
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

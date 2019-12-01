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

#define __DT_DECLARE_ACCESSOR_INLINE_FUNCS__(PropertyType, PropertyName) \
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

#define DT_DECLARE_ACCESSOR_INLINE_WITH_DEFAULT(PropertyType, PropertyName, DefaultVal) \
   private:\
      dtUtil::TypeTraits<PropertyType>::value_type  m ## PropertyName = DefaultVal; \
   __DT_DECLARE_ACCESSOR_INLINE_FUNCS__(PropertyType, PropertyName)

#define DT_DECLARE_ACCESSOR_INLINE(PropertyType, PropertyName) \
   private:\
      dtUtil::TypeTraits<PropertyType>::value_type  m ## PropertyName; \
   __DT_DECLARE_ACCESSOR_INLINE_FUNCS__(PropertyType, PropertyName)

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

#define DT_IMPLEMENT_ACCESSOR_SETTER_WITH_STATEMENT(ClassName, PropertyType, PropertyName, Code) \
      void ClassName::Set ## PropertyName(dtUtil::TypeTraits<PropertyType>::param_type value)\
      {\
         Code\
         m ## PropertyName = value;\
      }\
      \

#define DT_IMPLEMENT_ACCESSOR(ClassName, PropertyType, PropertyName) \
      DT_IMPLEMENT_ACCESSOR_SETTER(ClassName, PropertyType, PropertyName)\
      \
      DT_IMPLEMENT_ACCESSOR_GETTER(ClassName, PropertyType, PropertyName)\
      \

#define DT_IMPLEMENT_ACCESSOR_WITH_STATEMENT(ClassName, PropertyType, PropertyName, Code) \
      DT_IMPLEMENT_ACCESSOR_SETTER_WITH_STATEMENT(ClassName, PropertyType, PropertyName, Code)\
      \
      DT_IMPLEMENT_ACCESSOR_GETTER(ClassName, PropertyType, PropertyName)\
      \

#define DT_DECLARE_ARRAY_ACCESSOR(AccessorType, AccessorName, AccessorNamePlural) \
   private:\
      std::vector<dtUtil::TypeTraits<AccessorType>::value_type>  m ## AccessorNamePlural; \
   public: \
      \
      PROPERTY_MODIFIERS_SETTER void Set ## AccessorName(unsigned idx, dtUtil::TypeTraits<AccessorType>::param_type value);\
      \
      PROPERTY_MODIFIERS_GETTER dtUtil::TypeTraits<AccessorType>::return_type Get ## AccessorName(unsigned idx) const;\
      \
      PROPERTY_MODIFIERS_GETTER unsigned GetNum ## AccessorNamePlural() const { return  m ## AccessorNamePlural.size(); }\
      \
      PROPERTY_MODIFIERS_SETTER void Add ## AccessorName(dtUtil::TypeTraits<AccessorType>::param_type value);\
      \
      PROPERTY_MODIFIERS_SETTER void InsertWithValue ## AccessorName(unsigned idx, dtUtil::TypeTraits<AccessorType>::param_type value);\
      \
      PROPERTY_MODIFIERS_SETTER void Insert ## AccessorName(unsigned idx);\
      \
      PROPERTY_MODIFIERS_SETTER void Remove ## AccessorName(unsigned idx);\
      \
      PROPERTY_MODIFIERS_SETTER void ClearAll ## AccessorNamePlural(); \
      \
      template <typename Func> \
      void ForEach ## AccessorName (Func functor) \
      { \
         std::for_each(m ## AccessorNamePlural.begin(), m ## AccessorNamePlural.end(), functor); \
      }

#define DT_IMPLEMENT_ARRAY_ACCESSOR_WITH_ON_ADD_REMOVE(ClassName, AccessorType, AccessorName, AccessorNamePlural, DefaultNewValue, OnAddCode, OnRemoveCode) \
      void ClassName :: Set ## AccessorName(unsigned idx, dtUtil::TypeTraits<AccessorType>::param_type value)\
      {\
         if (m ## AccessorNamePlural.size() > idx)\
         {\
            OnRemoveCode \
            m ## AccessorNamePlural[idx] = value;\
            OnAddCode \
         }\
      }\
      \
      dtUtil::TypeTraits<AccessorType>::return_type ClassName :: Get ## AccessorName(unsigned idx) const\
      {\
         if (m ## AccessorNamePlural.size() > idx)\
         {\
            return m ## AccessorNamePlural[idx];\
         }\
         throw dtUtil::Exception("Index out of range.", __FUNCTION__, __LINE__);\
      }\
      \
      void ClassName :: Add ## AccessorName(dtUtil::TypeTraits<AccessorType>::param_type value)\
      {\
         m ## AccessorNamePlural.push_back(value);\
         OnAddCode \
      }\
      \
      void ClassName :: InsertWithValue ## AccessorName (unsigned idx, dtUtil::TypeTraits<AccessorType>::param_type value)\
      {\
         m ## AccessorNamePlural.insert(m ## AccessorNamePlural.begin() + idx, value);\
         OnAddCode \
      }\
      \
      void ClassName :: Insert ## AccessorName(unsigned idx)\
      {\
         InsertWithValue ## AccessorName(idx, (DefaultNewValue) );\
      }\
      \
      void ClassName :: Remove ## AccessorName(unsigned idx)\
      {\
         /* Use the result of the macro m ## AccessorNamePlural [idx]; if you want to access the variable */ \
         /* Just as a hint, you COULD call return if you want to veto the remove. */ \
         OnRemoveCode \
         m ## AccessorNamePlural.erase(m ## AccessorNamePlural.begin() + idx);\
      }\
      \
      void ClassName :: ClearAll ## AccessorNamePlural ()\
      {\
         m ## AccessorNamePlural.clear();\
      }\
      \

#define DT_IMPLEMENT_ARRAY_ACCESSOR(ClassName, AccessorType, AccessorName, AccessorNamePlural, DefaultNewValue) \
      DT_IMPLEMENT_ARRAY_ACCESSOR_WITH_ON_ADD_REMOVE(ClassName, AccessorType, AccessorName, AccessorNamePlural, DefaultNewValue,,)

#endif

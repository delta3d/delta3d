#include <prefix/dtcoreprefix.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtCore/datatype.h>

#include <cstdio>
#include <climits>

namespace dtCore
{


static const char OPEN_CHAR = '{';
static const char CLOSE_CHAR = '}';

////////////////////////////////////////////////////////////////////////////////
ArrayActorPropertyBase::ArrayActorPropertyBase(const std::string& name,
                                                      const std::string& label,
                                                      const std::string& desc,
                                                      ActorProperty* propertyType,
                                                      const std::string& groupName,
                                                      const std::string& editorType,
                                                      bool canReorder,
                                                      bool readOnly ) :
ActorProperty(DataType::ARRAY, name, label, desc, groupName, readOnly)
, mPropertyType(propertyType)
, mCanReorder(canReorder)
, mMinSize(-1)
, mMaxSize(-1)
{
}

////////////////////////////////////////////////////////////////////////////////
ArrayActorPropertyBase::~ArrayActorPropertyBase()
{

}

////////////////////////////////////////////////////////////////////////////////
void ArrayActorPropertyBase::InitDefault(const std::string& keyName)
{
   BaseClass::InitDefault(keyName);

   if (mPropertyType.valid())
   {
      Insert(0);
      SetIndex(0);
      mPropertyType->InitDefault(keyName);
      Remove(0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ArrayActorPropertyBase::CopyFrom(const ActorProperty& otherProp)
{
   if (GetDataType() != otherProp.GetDataType())
   {
      LOG_ERROR("Property types are incompatible. Cannot make copy.");
      return;
   }

   const ArrayActorPropertyBase* src = dynamic_cast<const ArrayActorPropertyBase*>(&otherProp);
   if (src)
   {
      FromString(src->ToString());
   }
}


////////////////////////////////////////////////////////////////////////////////
bool ArrayActorPropertyBase::FromString(const std::string& value)
{
   if (IsReadOnly())
   {
      LOG_WARNING("FromString has been called on a property that is read only.");
      return false;
   }

   if (!mPropertyType.valid())
   {
      return false;
   }

   std::string data = value;

   // First read the total size of the array.
   std::string token;
   bool result = dtUtil::TakeToken(data, token, OPEN_CHAR, CLOSE_CHAR);

   if (result)
   {
      // Make sure our array is the proper size.
      const int arraySize = dtUtil::ToType<int>(token);
      const int actualSize = Resize(arraySize);

      for (int index = 0; result && index < arraySize && index < actualSize; index++)
      {
         SetIndex(index);
         result = dtUtil::TakeToken(data, token, OPEN_CHAR, CLOSE_CHAR);
         if (result)
         {
            result = mPropertyType->FromString(token);
         }
      }
   }
   return result;
}

////////////////////////////////////////////////////////////////////////////////
const std::string ArrayActorPropertyBase::ToString() const
{
   if (!mPropertyType.valid())
   {
      return "";
   }

   // Iterate through each index in the array and append the strings.
   int arraySize = GetArraySize();

   char buffer[20] = {0,};
   std::string data;
   data += OPEN_CHAR;
   snprintf(buffer, 20, "%d", arraySize);
   data += buffer;
   data += CLOSE_CHAR;

   for (int index = 0; index < arraySize; index++)
   {
      SetIndex(index);
      data += OPEN_CHAR;
      data += mPropertyType->ToString();
      data += CLOSE_CHAR;
   }

   return data;
}

////////////////////////////////////////////////////////////////////////////////
ActorProperty* ArrayActorPropertyBase::GetArrayProperty()
{
   return mPropertyType.get();
}

////////////////////////////////////////////////////////////////////////////////
const ActorProperty* ArrayActorPropertyBase::GetArrayProperty() const
{
   return mPropertyType.get();
}

void ArrayActorPropertyBase::SetArrayProperty(ActorProperty& property)
{
   mPropertyType = &property;
}

////////////////////////////////////////////////////////////////////////////////
void ArrayActorPropertyBase::SetMinArraySize(int minSize)
{
   mMinSize = minSize;

   if (mMinSize > GetArraySize())
   {
      Resize(mMinSize);
   }
}

////////////////////////////////////////////////////////////////////////////////
int ArrayActorPropertyBase::GetMinArraySize() const
{
   return mMinSize;
}

////////////////////////////////////////////////////////////////////////////////
void ArrayActorPropertyBase::SetMaxArraySize(int maxSize)
{
   mMaxSize = maxSize;

   if (mMaxSize < GetArraySize())
   {
      Resize(mMaxSize);
   }
}

////////////////////////////////////////////////////////////////////////////////
int ArrayActorPropertyBase::GetMaxArraySize() const
{
   return mMaxSize;
}

////////////////////////////////////////////////////////////////////////////////
int ArrayActorPropertyBase::Resize(unsigned newSize)
{
   int arraySize = GetArraySize();

   int min, max;
   min = GetMinArraySize() < 0 ? 0 : GetMinArraySize();
   max = GetMaxArraySize() < 0 ? INT_MAX : GetMaxArraySize();

   // If the array is too small, make it larger
   while (arraySize > int(newSize) && arraySize > min)
   {
      PopBack();
      arraySize = GetArraySize();
   }

   while (arraySize < int(newSize) && arraySize < max)
   {
      PushBack();
      arraySize = GetArraySize();
   }

   return arraySize;
}

////////////////////////////////////////////////////////////////////////////////
bool ArrayActorPropertyBase::CanReorder() const
{
   return mCanReorder;
}
}

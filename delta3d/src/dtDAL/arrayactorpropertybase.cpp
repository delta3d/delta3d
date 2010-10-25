#include <prefix/dtdalprefix.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtDAL/datatype.h>

#include <cstdio>


namespace dtDAL
{


const char OPEN_CHAR = 1;
const char CLOSE_CHAR = 2;

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
   if (!mPropertyType.valid())
   {
      return false;
   }

   std::string data = value;

   // First read the total size of the array.
   std::string token;
   TakeToken(data, token);

   // Make sure our array is the proper size.
   const int arraySize = dtUtil::ToType<int>(token);
   const int actualSize = Resize(arraySize);

   for (int index = 0; index < arraySize && index < actualSize; index++)
   {
      SetIndex(index);
      TakeToken(data, token);
      mPropertyType->FromString(token);
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
void ArrayActorPropertyBase::TakeToken(std::string& data, std::string& outToken)
{
   outToken.clear();

   // If the first character in the data string is not the opening character,
   //  we will just assume the entire data string is the token.
   if (data.c_str()[0] != OPEN_CHAR)
   {
      outToken = data;
      data = "";
   }

   int depth = 0;
   int dataIndex = 0;
   while (data.length() > 1)
   {
      bool appendChar = true;

      // Opening characters increase the depth counter.
      if (data[dataIndex] == OPEN_CHAR)
      {
         depth++;

         if (depth == 1)
         {
            appendChar = false;
         }
      }
      // Closing characters decrease the depth counter.
      else if (data[dataIndex] == CLOSE_CHAR)
      {
         depth--;

         if (depth == 0)
         {
            appendChar = false;
         }
      }

      // All other characters are added to the return buffer.
      if (appendChar)
      {
         outToken.append(data.c_str(), 1);
      }

      // Remove the left most character from the data string.
      data = &data[1];

      // We are done once our depth returns to 0.
      if (depth <= 0)
      {
         break;
      }
   }
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

}

////////////////////////////////////////////////////////////////////////////////
bool ArrayActorPropertyBase::CanReorder() const
{
   return mCanReorder;
}
}

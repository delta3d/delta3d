#include <dtDAL/arrayactorpropertybase.h>
#include <dtUtil/log.h>
#include <dtUtil/macros.h>
#include <dtUtil/stringutils.h>
#include <dtDAL/datatype.h>

using namespace dtDAL;

const char OPEN_CHAR = 1;
const char CLOSE_CHAR = 2;

////////////////////////////////////////////////////////////////////////////////
dtDAL::ArrayActorPropertyBase::ArrayActorPropertyBase(const std::string& name,
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
dtDAL::ArrayActorPropertyBase::~ArrayActorPropertyBase()
{

}

////////////////////////////////////////////////////////////////////////////////
bool dtDAL::ArrayActorPropertyBase::FromString(const std::string& value)
{
   if (!mPropertyType.valid())
   {
      return false;
   }

   std::string data = value;

   // First read the total size of the array.
   std::string token = TakeToken(data);

   // Make sure our array is the proper size.
   const int arraySize = dtUtil::ToType<int>(token);
   while (GetArraySize() < arraySize)
   {
      Insert(0);
   }

   while (GetArraySize() > arraySize)
   {
      Remove(0);
   }

   for (int index = 0; index < arraySize; index++)
   {
      SetIndex(index);
      token = TakeToken(data);
      mPropertyType->FromString(token);
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
std::string dtDAL::ArrayActorPropertyBase::TakeToken(std::string& data)
{
   std::string returnData;

   // If the first character in the data string is not the opening character,
   //  we will just assume the entire data string is the token.
   if (data.c_str()[0] != OPEN_CHAR)
   {
      returnData = data;
      data = "";
   }

   int depth = 0;
   int dataIndex = 0;
   while (data.length() > 0)
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
         returnData.append(data.c_str(), 1);
      }

      // Remove the left most character from the data string.
      data = &data[1];

      // We are done once our depth returns to 0.
      if (depth <= 0)
      {
         break;
      }
   }

   return returnData;
}

////////////////////////////////////////////////////////////////////////////////
const std::string dtDAL::ArrayActorPropertyBase::ToString() const
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
void dtDAL::ArrayActorPropertyBase::CopyFrom(const ActorProperty& otherProp)
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
ActorProperty* dtDAL::ArrayActorPropertyBase::GetArrayProperty()
{
   return mPropertyType.get();
}

////////////////////////////////////////////////////////////////////////////////
const ActorProperty* dtDAL::ArrayActorPropertyBase::GetArrayProperty() const
{
   return mPropertyType.get();
}

////////////////////////////////////////////////////////////////////////////////
void dtDAL::ArrayActorPropertyBase::SetMinArraySize(int minSize)
{
   mMinSize = minSize;

   // TODO ARRAY: Make sure to add indexes if the minimum size is larger than the current size.
}

////////////////////////////////////////////////////////////////////////////////
int dtDAL::ArrayActorPropertyBase::GetMinArraySize() const
{
   return mMinSize;
}

////////////////////////////////////////////////////////////////////////////////
void dtDAL::ArrayActorPropertyBase::SetMaxArraySize(int maxSize)
{
   mMaxSize = maxSize;

   // TODO ARRAY: Make sure to remove indexes if the maximum size exceeds the current size.
}

////////////////////////////////////////////////////////////////////////////////
int dtDAL::ArrayActorPropertyBase::GetMaxArraySize() const
{
   return mMaxSize;
}

////////////////////////////////////////////////////////////////////////////////
bool dtDAL::ArrayActorPropertyBase::CanReorder() const
{
   return mCanReorder;
}

////////////////////////////////////////////////////////////////////////////////
int dtDAL::ArrayActorPropertyBase::GetArraySize() const
{
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
void dtDAL::ArrayActorPropertyBase::SetIndex(int index) const
{

}

////////////////////////////////////////////////////////////////////////////////
bool dtDAL::ArrayActorPropertyBase::Insert(int index)
{
   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool dtDAL::ArrayActorPropertyBase::Remove(int index)
{
   return false;
}

////////////////////////////////////////////////////////////////////////////////
void dtDAL::ArrayActorPropertyBase::Clear()
{

}

////////////////////////////////////////////////////////////////////////////////
void dtDAL::ArrayActorPropertyBase::Swap(int first, int second)
{

}

////////////////////////////////////////////////////////////////////////////////
void dtDAL::ArrayActorPropertyBase::Copy(int src, int dst)
{

}


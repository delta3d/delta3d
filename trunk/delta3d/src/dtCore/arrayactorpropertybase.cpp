#include <prefix/dtcoreprefix.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtCore/datatype.h>
#include <dtCore/namedarrayparameter.h>

#include <cstdio>
#include <climits>

namespace dtCore
{


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
void ArrayActorPropertyBase::InitDefault(ObjectType& type)
{
   BaseClass::InitDefault(type);

   if (mPropertyType.valid())
   {
      Insert(0);
      SetIndex(0);
      mPropertyType->InitDefault(type);
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
      dtCore::RefPtr<dtCore::NamedArrayParameter> param = new dtCore::NamedArrayParameter(GetName());
      param->SetFromProperty(*src);
      param->ApplyValueToProperty(*this);
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

   dtCore::RefPtr<dtCore::NamedArrayParameter> param = new dtCore::NamedArrayParameter(GetName());

   bool result = param->FromString(value);
   if (result)
      param->ApplyValueToProperty(*this);
   return result;
}

////////////////////////////////////////////////////////////////////////////////
const std::string ArrayActorPropertyBase::ToString() const
{
   std::string result;
   if (mPropertyType.valid())
   {

      dtCore::RefPtr<dtCore::NamedArrayParameter> param = new dtCore::NamedArrayParameter(GetName());

      param->SetFromProperty(*this);
      result = param->ToString();
   }
   return result;
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

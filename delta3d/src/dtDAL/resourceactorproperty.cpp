#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

using namespace dtDAL;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
ResourceActorProperty::ResourceActorProperty(ActorProxy& actorProxy,
                                             DataType& type,
                                             const dtUtil::RefString& name,
                                             const dtUtil::RefString& label,
                                             SetFuncType Set,
                                             const dtUtil::RefString& desc,
                                             const dtUtil::RefString& groupName)
: ActorProperty(type, name, label, desc, groupName)
, mProxy(&actorProxy)
, SetPropFunctor(Set)
, mHasGetFunctor(false)
, mUsingDescFunctors(false)
{
}

////////////////////////////////////////////////////////////////////////////
ResourceActorProperty::ResourceActorProperty(ActorProxy& actorProxy,
                                             DataType& type,
                                             const dtUtil::RefString& name,
                                             const dtUtil::RefString& label,
                                             SetFuncType Set,
                                             GetFuncType Get,
                                             const dtUtil::RefString& desc,
                                             const dtUtil::RefString& groupName)
: ActorProperty(type, name, label, desc, groupName)
, mProxy(&actorProxy)
, SetPropFunctor(Set)
, mHasGetFunctor(true)
, GetPropFunctor(Get)
, mUsingDescFunctors(false)
{
}

////////////////////////////////////////////////////////////////////////////////
ResourceActorProperty::ResourceActorProperty(DataType& type,
                                             const dtUtil::RefString& name,
                                             const dtUtil::RefString& label,
                                             SetDescFuncType Set,
                                             GetDescFuncType Get,
                                             const dtUtil::RefString& desc,
                                             const dtUtil::RefString& groupName)
: ActorProperty(type, name, label, desc, groupName)
, mProxy(NULL)
, mHasGetFunctor(false)
, mUsingDescFunctors(true)
, SetDescPropFunctor(Set)
, GetDescPropFunctor(Get)
{
}

////////////////////////////////////////////////////////////////////////////
void ResourceActorProperty::CopyFrom(const ActorProperty& otherProp)
{
   if (GetDataType() != otherProp.GetDataType())
   {
      LOG_ERROR("Property types are incompatible. Cannot make copy.");
   }

   const ResourceActorProperty& prop =
      static_cast<const ResourceActorProperty&>(otherProp);

   SetValue(prop.GetValue());
}

////////////////////////////////////////////////////////////////////////////
void ResourceActorProperty::SetValue(ResourceDescriptor* value)
{
   DEPRECATE("void ResourceActorProperty::SetValue(ResourceDescriptor*)",
             "void ResourceActorProperty::SetValue(const ResourceDescriptor&)");

   if (value)
   {
      SetValue(*value);
   }
   else
   {
      SetValue(dtDAL::ResourceDescriptor::NULL_RESOURCE);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ResourceActorProperty::SetValue(const ResourceDescriptor& value)
{
   if (IsReadOnly())
   {
      LOG_WARNING("SetValue has been called on a property that is read only.");
      return;
   }

   if (mProxy)
   {
      mProxy->SetResource(GetName(), value);
   }

   if (mUsingDescFunctors)
   {
      SetDescPropFunctor(value);
   }
   else
   {
      if (value.IsEmpty())
      {
         SetPropFunctor("");
      }
      else
      {
         try
         {
            std::string path = Project::GetInstance().GetResourcePath(value);
            if (dtUtil::Log::GetInstance("EnginePropertyTypes.h").IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               dtUtil::Log::GetInstance("EnginePropertyTypes.h").LogMessage(dtUtil::Log::LOG_DEBUG,
               __FUNCTION__, __LINE__,
               "Path to resource is: %s",
               path.c_str());
            SetPropFunctor(path);
         }
         catch(const dtUtil::Exception& ex)
         {
            if (mProxy)
            {
               mProxy->SetResource(GetName(), dtDAL::ResourceDescriptor::NULL_RESOURCE);
            }
            SetPropFunctor("");
            dtUtil::Log::GetInstance("EnginePropertyTypes.h").LogMessage(dtUtil::Log::LOG_WARNING,
               __FUNCTION__, __LINE__, "Resource %s not found.  Setting property %s to NULL. Error Message %s.",
               value.GetResourceIdentifier().c_str(), GetName().c_str(), ex.What().c_str());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////
ResourceDescriptor ResourceActorProperty::GetValue() const
{
   if (mUsingDescFunctors)
   {
      return GetDescPropFunctor();
   }

   if (mHasGetFunctor)
   {
      std::string resName = GetPropFunctor();
      dtDAL::ResourceDescriptor descriptor(resName);
      if (mProxy)
      {
         mProxy->SetResource(GetName(), descriptor);
      }
   }

   if (mProxy)
   {
      return mProxy->GetResource(GetName());
   }
   else
   {
      return dtDAL::ResourceDescriptor::NULL_RESOURCE;
   }
}

////////////////////////////////////////////////////////////////////////////
bool ResourceActorProperty::FromString(const std::string& value)
{
   if (IsReadOnly())
   {
      LOG_WARNING("FromString has been called on a property that is read only.");
      return false;
   }

   bool result = true;
   if (value.empty() || value == "NULL")
   {
      SetValue(dtDAL::ResourceDescriptor::NULL_RESOURCE);
   }
   else
   {
      std::vector<std::string> tokens;
      dtUtil::StringTokenizer<dtUtil::IsSlash> stok;

#ifdef _MSC_VER
      stok = stok; // Silence unsed variable warning in MSVC.
#endif

      stok.tokenize(tokens, value);

      std::string displayName;
      std::string identifier;

      if (tokens.size() == 2)
      {
         displayName = tokens[0];
         identifier = tokens[1];
      }
      else
      {
         //assume the value is a descriptor and use it for both the
         //data and the display name.
         displayName = tokens[0];
         identifier = tokens[0];
      }

      dtUtil::Trim(identifier);
      dtUtil::Trim(displayName);

      dtDAL::ResourceDescriptor descriptor(displayName, identifier);
      SetValue(descriptor);

   }

   return result;
}

////////////////////////////////////////////////////////////////////////////
const std::string ResourceActorProperty::ToString() const
{
   ResourceDescriptor r = GetValue();
   if (r.IsEmpty())
      return "";
   else
      return r.GetDisplayName() + "/" + r.GetResourceIdentifier();
}

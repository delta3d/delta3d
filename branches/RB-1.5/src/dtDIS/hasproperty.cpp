#include <dtDIS/hasproperty.h>

#include <cstddef>   // for NULL definition

using namespace dtDIS::details;

HasProperty::HasProperty(const std::string& prop)
   : mPropName(prop)
{
}

HasProperty::~HasProperty()
{
}

HasProperty::HasProperty(const HasProperty& hp)
   : mActors(hp.mActors)
   , mPropName(hp.mPropName)
{
}

void HasProperty::operator ()(dtDAL::ActorProxy *&proxy)
{
   const dtDAL::ActorProperty* ap = proxy->GetProperty( mPropName );
   if( ap == NULL )
   {
      return;
   }

   mActors.push_back( proxy );
}

HasProperty::ActorVector& HasProperty::GetPassedActors()
{
   return mActors;
}

const HasProperty::ActorVector& HasProperty::GetPassedActors() const
{
   return mActors;
}


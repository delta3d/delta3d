#include <dtActors/envactor.h>
#include <dtCore/environment.h>
#include <dtDAL/enginepropertytypes.h>

using namespace dtActors;

dtActors::EnvActor::EnvActor( dtGame::GameActorProxy &proxy ):
dtGame::IEnvGameActor(proxy),
mEnv(new dtCore::Environment())
{
   SetName("EnvActor");
}


EnvActor::~EnvActor()
{

}

void EnvActor::SetFogEnable( bool enabled )
{
   mEnv->SetFogEnable(enabled);
}

bool EnvActor::GetFogEnable() const
{
  return mEnv->GetFogEnable();
}

void EnvActor::AddActor( dtCore::DeltaDrawable &dd )
{
   mEnv->AddChild(&dd);
}

void EnvActor::RemoveActor( dtCore::DeltaDrawable &dd )
{
   mEnv->RemoveChild(&dd);
}

void EnvActor::RemoveAllActors()
{
   while (mEnv->GetNumChildren() > 0)
   {
      mEnv->RemoveChild( mEnv->GetChild(0) );
   }
}

bool EnvActor::ContainsActor( dtCore::DeltaDrawable &dd ) const
{
   if (mEnv->GetChildIndex(&dd) == mEnv->GetNumChildren())
   {
      return false;
   }
   else
   {
      return true;
   }
}

void EnvActor::GetAllActors( std::vector<dtCore::DeltaDrawable*> &vec )
{
   vec.clear();

   for(unsigned int i = 0; i < mEnv->GetNumChildren(); i++)
      vec.push_back(mEnv->GetChild(i));
}

void EnvActor::SetTimeAndDate( const int year, const int month, const int day, const int hour, const int min, const int sec )
{
   mEnv->SetDateTime(year, month, day, hour, min, sec);
}

void EnvActor::GetTimeAndDate( int &year, int &month, int &day, int &hour, int &min, int &sec ) const
{
   mEnv->GetDateTime(year, month, day, hour, min, sec);
}

unsigned int EnvActor::GetNumEnvironmentChildren() const
{
   return mEnv->GetNumChildren();
}




EnvActorProxy::EnvActorProxy()
{
}

EnvActorProxy::~EnvActorProxy()
{
}

void EnvActorProxy::BuildPropertyMap()
{
   using namespace dtDAL;
   dtGame::GameActorProxy::BuildPropertyMap();

   EnvActor *env;
   this->GetActor(env);

   AddProperty( new BooleanActorProperty("Enable Fog", "Enable Fog",
      MakeFunctor(*env, &EnvActor::SetFogEnable),
      MakeFunctorRet(*env, &EnvActor::GetFogEnable),
      "Enable or disable the fog"));
}

void EnvActorProxy::CreateActor()
{
   dtActors::EnvActor *env = new dtActors::EnvActor( *this );
   SetActor( *env );
}

void EnvActorProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}
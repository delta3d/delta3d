#include <dtActors/envactor.h>
#include <dtCore/environment.h>
#include <dtCore/skydome.h>
#include <dtDAL/enginepropertytypes.h>

using namespace dtActors;

EnvActor::EnvActor( dtGame::GameActorProxy &proxy ):
dtGame::IEnvGameActor(proxy),
mEnv(new dtCore::Environment()),
mSkyDome(NULL)
{
   SetName("EnvActor");

   //Add the Environment as a child to EnvActor, so when EnvActor
   //gets added to the scene, so does the Environment
   AddChild( mEnv.get() );
}


EnvActor::~EnvActor()
{

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


unsigned int EnvActor::GetNumEnvironmentChildren() const
{
   return mEnv->GetNumChildren();
}


void EnvActor::SetFogEnable( bool enabled )
{
   mEnv->SetFogEnable(enabled);
}


bool EnvActor::GetFogEnable() const
{
   return mEnv->GetFogEnable();
}


void EnvActor::SetTimeAndDate( const int year, const int month, const int day, const int hour, const int min, const int sec )
{
   mEnv->SetDateTime(year, month, day, hour, min, sec);
}


void EnvActor::GetTimeAndDate( int &year, int &month, int &day, int &hour, int &min, int &sec ) const
{
   mEnv->GetDateTime(year, month, day, hour, min, sec);
}


void EnvActor::SetVisibility( float distance )
{
   mEnv->SetVisibility(distance);
}


float EnvActor::GetVisbility() const
{
   return mEnv->GetVisibility();
}

void EnvActor::SetSkyDomeEnable( bool enabled )
{
   //no change
   if (enabled == mSkyDome.valid()) return;


   if (enabled == true)
   {
      mSkyDome = new dtCore::SkyDome("sky dome");
      mEnv->AddEffect( mSkyDome.get() );
   }
   else
   {
      mEnv->RemEffect( mSkyDome.get() );
      mSkyDome = NULL;
   }
}


bool EnvActor::GetSkyDomeEnable() const
{
   return mSkyDome.valid();
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
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

   AddProperty( new BooleanActorProperty("Enable Sky Dome", "Enable Sky Dome",
      MakeFunctor(*env, &EnvActor::SetSkyDomeEnable),
      MakeFunctorRet(*env, &EnvActor::GetSkyDomeEnable),
      "Enable or disable the rendering of the Sky Dome"));
}

void EnvActorProxy::CreateActor()
{
   dtActors::EnvActor *env = new dtActors::EnvActor( *this );
   SetActor( *env );
}


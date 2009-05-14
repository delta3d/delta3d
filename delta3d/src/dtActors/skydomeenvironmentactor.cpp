#include <dtActors/skydomeenvironmentactor.h>
#include <dtCore/environment.h>
#include <dtCore/skydome.h>
#include <dtDAL/enginepropertytypes.h>

using namespace dtActors;

SkyDomeEnvironmentActor::SkyDomeEnvironmentActor( dtGame::GameActorProxy &proxy ):
dtGame::IEnvGameActor(proxy),
mEnv(new dtCore::Environment()),
mSkyDome(NULL)
{
   SetName("SkyDomeEnvironmentActor");

   //Add the Environment as a child to SkyDomeEnvironmentActor, so when SkyDomeEnvironmentActor
   //gets added to the scene, so does the Environment
   AddChild( mEnv.get() );
}


SkyDomeEnvironmentActor::~SkyDomeEnvironmentActor()
{

}


void SkyDomeEnvironmentActor::AddActor( dtCore::DeltaDrawable &dd )
{
   mEnv->AddChild(&dd);
}


void SkyDomeEnvironmentActor::RemoveActor( dtCore::DeltaDrawable &dd )
{
   mEnv->RemoveChild(&dd);
}


void SkyDomeEnvironmentActor::RemoveAllActors()
{
   while (mEnv->GetNumChildren() > 0)
   {
      mEnv->RemoveChild( mEnv->GetChild(0) );
   }
}


bool SkyDomeEnvironmentActor::ContainsActor( dtCore::DeltaDrawable &dd ) const
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


void SkyDomeEnvironmentActor::GetAllActors( std::vector<dtCore::DeltaDrawable*> &vec )
{
   vec.clear();

   for(unsigned int i = 0; i < mEnv->GetNumChildren(); i++)
      vec.push_back(mEnv->GetChild(i));
}


unsigned int SkyDomeEnvironmentActor::GetNumEnvironmentChildren() const
{
   return mEnv->GetNumChildren();
}


void SkyDomeEnvironmentActor::SetFogEnable( bool enabled )
{
   mEnv->SetFogEnable(enabled);
}


bool SkyDomeEnvironmentActor::GetFogEnable() const
{
   return mEnv->GetFogEnable();
}


void SkyDomeEnvironmentActor::SetTimeAndDate( const unsigned year, const unsigned month, const unsigned day, const unsigned hour, const unsigned min, const unsigned sec )
{
   mEnv->SetDateTime(year, month, day, hour, min, sec);
}


void SkyDomeEnvironmentActor::GetTimeAndDate( unsigned &year, unsigned &month, unsigned &day, unsigned &hour, unsigned &min, unsigned &sec ) const
{
   mEnv->GetDateTime(year, month, day, hour, min, sec);
}


void SkyDomeEnvironmentActor::SetVisibility( float distance )
{
   mEnv->SetVisibility(distance);
}


float SkyDomeEnvironmentActor::GetVisbility() const
{
   return mEnv->GetVisibility();
}

void SkyDomeEnvironmentActor::SetSkyDomeEnable( bool enabled )
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


bool SkyDomeEnvironmentActor::GetSkyDomeEnable() const
{
   return mSkyDome.valid();
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
SkyDomeEnvironmentActorProxy::SkyDomeEnvironmentActorProxy()
{
}

SkyDomeEnvironmentActorProxy::~SkyDomeEnvironmentActorProxy()
{
}

void SkyDomeEnvironmentActorProxy::BuildPropertyMap()
{
   using namespace dtDAL;
   dtGame::GameActorProxy::BuildPropertyMap();

   SkyDomeEnvironmentActor *env;
   this->GetActor(env);

   AddProperty( new BooleanActorProperty("Enable Fog", "Enable Fog",
      MakeFunctor(*env, &SkyDomeEnvironmentActor::SetFogEnable),
      MakeFunctorRet(*env, &SkyDomeEnvironmentActor::GetFogEnable),
      "Enable or disable the fog"));

   AddProperty( new BooleanActorProperty("Enable Sky Dome", "Enable Sky Dome",
      MakeFunctor(*env, &SkyDomeEnvironmentActor::SetSkyDomeEnable),
      MakeFunctorRet(*env, &SkyDomeEnvironmentActor::GetSkyDomeEnable),
      "Enable or disable the rendering of the Sky Dome"));

   AddProperty( new FloatActorProperty("Visibility", "Visibility",
      MakeFunctor(*env, &SkyDomeEnvironmentActor::SetVisibility),
      MakeFunctorRet(*env, &SkyDomeEnvironmentActor::GetVisbility),
      "Adjusts the max visibility distance (meters)"));
}

void SkyDomeEnvironmentActorProxy::CreateActor()
{
   dtActors::SkyDomeEnvironmentActor *env = new dtActors::SkyDomeEnvironmentActor( *this );
   SetActor( *env );
}


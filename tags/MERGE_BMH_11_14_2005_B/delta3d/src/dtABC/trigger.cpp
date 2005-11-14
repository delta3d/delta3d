#include <dtABC/trigger.h>
#include <dtCore/system.h>

using namespace dtABC;

IMPLEMENT_MANAGEMENT_LAYER(Trigger)

Trigger::Trigger( const std::string& name )
   :  DeltaDrawable(name),
      mEnabled(false),
      mTimeDelay(0.0),
      mTimeLeft(mTimeDelay),
      mActionToFire(0)
{
   RegisterInstance(this);
}

Trigger::~Trigger()
{
   DeregisterInstance(this);
}

void Trigger::OnMessage( dtCore::Base::MessageData* data )
{
   if( data->message == "preframe" )
   {
      double dt = *static_cast<double*>(data->userData);
      Update(dt);
   }
}

void Trigger::Fire()
{
   if( mEnabled )
   {  
      AddSender( dtCore::System::Instance() );
   }
}

void Trigger::Update( double time )
{
   if( mEnabled ) 
   {
      mTimeLeft -= time;

      if( mTimeLeft <= 0 )
      {
         if(mActionToFire.valid())
         {
            mActionToFire->Start();
         }
         mTimeLeft = mTimeDelay;
         RemoveSender( dtCore::System::Instance() );
      }
   }
}

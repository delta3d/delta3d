#include <dtABC/trigger.h>
#include <dtCore/system.h>

using namespace dtABC;

IMPLEMENT_MANAGEMENT_LAYER(Trigger)

////////////////////////////////////////////////////////////////////////////////
Trigger::Trigger(const std::string& name)
   : DeltaDrawable(name)
   , mEnabled(false)
   , mTimeDelay(0.0)
   , mTimeLeft(mTimeDelay)
   , mTimesActive(1)
   , mTimesTriggered(0)
   , mActionToFire(0)
{
   RegisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
Trigger::~Trigger()
{
   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void Trigger::OnMessage(dtCore::Base::MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      double dt = *static_cast<double*>(data->userData);
      Update(dt);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Trigger::Fire()
{
   if (mEnabled)
   {
      if (mTimesActive < 0)
      {
         // Infinite activations
         AddSender(&dtCore::System::GetInstance());
      }
      else if (mTimesTriggered < mTimesActive)
      {
         AddSender(&dtCore::System::GetInstance());
         ++mTimesTriggered;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Trigger::Update(double time)
{
   if (mEnabled)
   {
      mTimeLeft -= time;

      if (mTimeLeft <= 0)
      {
         if (mActionToFire.valid())
         {
            mActionToFire->Start();
         }
         mTimeLeft = mTimeDelay;
         RemoveSender(&dtCore::System::GetInstance());
      }
   }
}

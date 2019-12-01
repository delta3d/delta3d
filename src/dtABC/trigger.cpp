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
void Trigger::OnSystem(const dtUtil::RefString& str, double deltaSim, double)

{
   if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      Update(deltaSim);
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
         dtCore::System::GetInstance().TickSignal.connect_slot(this, &Trigger::OnSystem);
      }
      else if (mTimesTriggered < mTimesActive)
      {
         dtCore::System::GetInstance().TickSignal.connect_slot(this, &Trigger::OnSystem);
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
         dtCore::System::GetInstance().TickSignal.disconnect(this);
      }
   }
}

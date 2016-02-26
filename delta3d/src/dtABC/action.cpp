#include <dtABC/action.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>


namespace dtABC
{

////////////////////////////////////////////////////////////////////////////////
Action::Action()
   : mTimeStep(0.0f)
   , mTotalTime(0.0f)
   , mAccumTime(0.0f)
   , mIsRunning(false)
   , mTickOncePerFrame(false)
{
   dtCore::System::GetInstance().TickSignal.connect_slot(this, &Action::OnSystem);
}

////////////////////////////////////////////////////////////////////////////////
Action::~Action()
{
}

////////////////////////////////////////////////////////////////////////////////
bool Action::Start()
{
   // we cannot be started after we are already running
   if (!mIsRunning && CanStart())
   {
      mTotalTime = 0.0f;
      mAccumTime = 0.0f;
      if (mTimeStep < 0.000001f) { mTimeStep = float(1.0 / 60.0); }
      OnStart();
      mIsRunning = true;
   }

   return mIsRunning;
}

////////////////////////////////////////////////////////////////////////////////
void Action::Pause()
{
   mIsRunning = false;
   OnPause();
}

////////////////////////////////////////////////////////////////////////////////
void Action::UnPause()
{
   mIsRunning = true;
   OnUnPause();
}

////////////////////////////////////////////////////////////////////////////////
void Action::OnSystem(const dtUtil::RefString& str, double deltaSim, double)
{
   if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      Update(deltaSim);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Action::Update(double dt)
{
   if (mIsRunning)
   {
      mTotalTime += dt;

      if (mTickOncePerFrame)
      {
         if (!OnNextStep())
         {
            mIsRunning = false;
         }
      }
      else
      {
         mAccumTime += dt;

         while (mAccumTime >= mTimeStep && mIsRunning)
         {
            if (!OnNextStep())
            {
               mIsRunning = false;
            }

            mAccumTime -= mTimeStep;
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////

} // namespace dtABC

#include <dtABC/action.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>


namespace dtABC
{

Action::Action()
   : mTimeStep(0.0f)
   , mTotalTime(0.0f)
   , mAccumTime(0.0f)
   , mIsRunning(false)
   , mTickOncePerFrame(false)
{
   AddSender(&dtCore::System::GetInstance());
}

Action::~Action()
{
   RemoveSender(&dtCore::System::GetInstance());
}

void Action::Start()
{
   // we cannot be started after we are already running
   if (mIsRunning) { return; }

   mTotalTime = 0.0f;
   mAccumTime = 0.0f;
   if (mTimeStep < 0.000001f) { mTimeStep = float(1.0 / 60.0); }
   OnStart();
   mIsRunning = true;
}

void Action::Pause()
{
   mIsRunning = false;
   OnPause();
}

void Action::UnPause()
{
   mIsRunning = true;
   OnUnPause();
}


void Action::OnMessage(MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      Update(*(double*)data->userData);
   }
}


void Action::Update(double dt)
{
   if (mIsRunning)
   {
      if (mTickOncePerFrame)
      {
         if (!OnNextStep())
         {
            mIsRunning = false;
         }
      }
      else
      {
         mTotalTime += dt;
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

} // namespace dtABC


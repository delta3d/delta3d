#include <dtABC/action.h>
#include <dtCore/system.h>


namespace dtABC
{

Action::Action()
{
   mTimeStep = 0.0f;
   mTotalTime = 0.0f;
   mAccumTime = 0.0f;
   mIsRunning = false;

   dtCore::System::Instance()->AddSender(this);
}

Action::~Action()
{
   dtCore::System::Instance()->RemoveSender(this);
}

void Action::Start()
{
   mTotalTime = 0.0f;
   mAccumTime = 0.0f;
   mIsRunning = true;
   OnStart();
}

void Action::Pause()
{
   mIsRunning = false;
   OnPause();
}

void Action::UnPause()
{
   mIsRunning = false;
   OnUnPause();
}


void Action::OnMessage( MessageData* data )
{
   if( data->message == "preframe" )
   {
      Update( *(double*)data->userData );
   }
}


void Action::Update(double dt)
{
   if(mIsRunning)
   {
      mTotalTime += dt;
      mAccumTime += dt;

      while(mAccumTime >= mTimeStep)
      {
         bool lastEvent = !(OnNextStep());

         if(lastEvent)
         {
            mIsRunning = false;
         }

         mAccumTime -= mTimeStep;
      }
   }

}

}//namespace dtABC


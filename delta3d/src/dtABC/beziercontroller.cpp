#include <dtABC/beziercontroller.h>
#include <assert.h>

namespace dtABC
{


BezierController::BezierController()
{
   mPath = new BezierPath();
}

BezierController::~BezierController()
{

}


bool BezierController::OnNextStep()
{
   StepObject(*mCurrentPoint);

   ++mCurrentPoint;
   return (mCurrentPoint != mEndPoint);
}


void BezierController::SetStartNode(BezierNode* pStart)
{
   mPath->SetStartNode(pStart);
   mPath->CreatePath();

   //since we are creating the path on this assign, we will warn the user
   // if the path does not contain the valid data yet
   assert(mPath->GetPath().size() > 0);

   mCurrentPoint = mPath->GetPath().begin();
   mEndPoint = mPath->GetPath().end();
}

void BezierController::OnStart()
{
   
}


void BezierController::OnPause()
{


}

void BezierController::OnRestart()
{
  mCurrentPoint = mPath->GetPath().begin();
  mEndPoint = mPath->GetPath().end();
}

}//namespace dtABC


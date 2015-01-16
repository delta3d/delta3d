#include <dtABC/beziercontroller.h>
#include <dtCore/scene.h>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/LineWidth>

#include <cmath>
#include <assert.h>

namespace dtABC
{

const std::string BezierController::BEZIER_CONTROLLER_GEODE_ID("__DELTA3D_BEZIER_CONTROLLER__");

////////////////////////////////////////////////////////////////////////////////
BezierController::BezierController()
   : mRenderGeode(false)
   , mPathChanged(false)
   , mShouldLoop(false)
   , mFollowPath(false)
   , mLastPathPoint(NULL)
{  
   mDrawable = new BezierPathDrawable;
   mDrawable->SetPath(this);

   mGeode = new osg::Geode();
   mGeode->setName(BEZIER_CONTROLLER_GEODE_ID);
   mGeode->addDrawable(mDrawable.get());

   osg::StateSet* ss = mGeode->getOrCreateStateSet();
   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   osg::LineWidth* lineWidth = new osg::LineWidth(3.0f);
   ss->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);

   mNode = new osg::Group;
}

////////////////////////////////////////////////////////////////////////////////
BezierController::~BezierController()
{

}

////////////////////////////////////////////////////////////////////////////////
void BezierController::CreatePath()
{
   if (!mStartNode || !mStartNode->GetNext())
   {
      return;
   }

   //we set this flag, so the draw implementation
   //knows not to touch this
   mPathChanged = false;

   mPath.clear();

   const BezierNode* pCurrentNode = mStartNode->GetBezierInterface();
   const BezierNode* pNextNode = pCurrentNode->GetNext()->GetBezierInterface();

   float elapsedTime = 0.0;

   while (pCurrentNode && pNextNode && pCurrentNode->GetExit() && pNextNode->GetEntry())
   {
      float dt = pCurrentNode->GetStep();
      float totalTime = pCurrentNode->GetTimeToNext();
      float multiply = 1.0f / totalTime;

      // Magical Epsilon
      if (dt < 0.000001f)
      {
         dt = 0.05f;
      }

      for (float j = 0.0f; j < totalTime; j+= dt)
      {
         //note this is simplified through extending PathPointConverter
         //which has an implicit conversion from a dtCore::Transformable to a PathPoint
         MakeSegment(elapsedTime,
                     j * multiply, pCurrentNode->GetPathPoint(),
                     pCurrentNode->GetExit()->GetPathPoint(),
                     pNextNode->GetEntry()->GetPathPoint(),
                     pNextNode->GetPathPoint());

         elapsedTime += dt;
      }

      pCurrentNode = pNextNode;

      if (pCurrentNode->GetNext())
      {
         pNextNode = pCurrentNode->GetNext()->GetBezierInterface();
      }
      else
      {
         pNextNode = NULL;
      }
   }

   mPathChanged = true;
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::ResetIterators()
{
   // Reset our iterators for the new list
   mLastPathPoint = NULL;
   mCurrentPoint  = mPath.begin();
   mEndPoint      = mPath.end();

   if (mPath.size() > 2)
   {
      StepObject((*mCurrentPoint).mPoint);
      mLastPathPoint = &(*mCurrentPoint);
      ++mCurrentPoint;
   }
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::MakeSegment(float time, 
                                   float t,
                                   const PathPoint& p1CurrentNode,
                                   const PathPoint& p2CurrentNodeExit,
                                   const PathPoint& p3NextNodeEntry,
                                   const PathPoint& p4NextNode)
{
   osg::Vec3 pos, tangent;

   for (int i = 0; i < 3; ++i)
   {
      pos[i] = (BlendFunction(t,0) * p1CurrentNode.GetPosition()[i]) +
               (BlendFunction(t,1) * p2CurrentNodeExit.GetPosition()[i]) +
               (BlendFunction(t,2) * p3NextNodeEntry.GetPosition()[i]) +
               (BlendFunction(t,3) * p4NextNode.GetPosition()[i]);

      //tangent[i] = (TangentFunction(inc,1) * p2.GetPosition()[i]) + (TangentFunction(inc,2) * p3.GetPosition()[i]) + (TangentFunction(inc,3) * p4.GetPosition()[i]);

      if(mFollowPath) // for path following we calculate tangent to curve using derivative of blend function
      {
         tangent[i] = (DerivativeFunction(t,0) * p1CurrentNode.GetPosition()[i]) +
                      (DerivativeFunction(t,1) * p2CurrentNodeExit.GetPosition()[i]) +
                      (DerivativeFunction(t,2) * p3NextNodeEntry.GetPosition()[i]) +
                      (DerivativeFunction(t,3) * p4NextNode.GetPosition()[i]);
      }
   }

   osg::Quat quat;
   if(!mFollowPath)	   // interpolate between node orientations
   {
      quat.slerp(1.0 - t, p4NextNode.GetOrientation(), p1CurrentNode.GetOrientation());
   }
   else	// align along path
   {
      //rotating half a pi to rotate forward to be along the +y axis
      quat.makeRotate(atan2(tangent.y(),tangent.x())- osg::PI_2, 0.0,0.0,1.0);
      quat.conj();
      // Note: the LocalTransform of the Bezier controller can be used to orient the objects forward direction
   }

   PathData pd;
   pd.mTime = time;
   pd.mPoint.SetPosition(pos);
   pd.mPoint.SetOrientation(quat);

   mPath.push_back(pd);
}

////////////////////////////////////////////////////////////////////////////////
float BezierController::BlendFunction(float t, int index)
{
   // Calculates each part of Cubic B�zier curve depending on the indexed part of the function
   float result = 0.0f;

   switch (index)
   {
   case 0: // CurrentNode * (1-t)^3
      result = powf(1.0f - t, 3.0f);
      break;

   case 1: // CurrentNodeExit *  3 * (1 - t)^2 * t 
      result = 3.0f * t * powf(1.0f - t, 2.0f);
      break;

   case 2: // NextNodeEntry * 3 * (1 - t) * t^2
      result = 3.0f * powf(t, 2.0f) * (1.0f - t);
      break;

   case 3: // NextNode * t^3
      result = powf(t, 3.0f);
      break;

   default:
      assert(0);
      break;
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
float BezierController::DerivativeFunction(float t, int index) const
{  
   // This is the derivative of the blendfunction and calculates the 
   // derivative (tangent) of each part of Cubic B�zier curve 
   // depending on the indexed part of the function
   float result = 0.0f;

   switch (index)
   {
   case 0: // CurrentNode * (-3)*(t - 1)^2
      result = -3.0f * powf(t - 1.0f, 2.0f);
      break;

   case 1: // CurrentNodeExit * 3*t*(2*t - 2) + 3*(t - 1)^2
      result = 3.0f * t * (2.0f * t - 2.0f) + 3.0f * powf(t - 1.0f, 2.0f);
      break;

   case 2: // NextNodeEntry * (-2)*t*(3*t - 3) - 3*t^2
      result = -2.0f * t * (3.0f * t - 3.0f) - 3.0f * powf(t, 2.0f);
      break;

   case 3: // NextNode * 3*t^2
      result = 3.0f * powf(t, 2.0f);
      break;

   default:
      assert(0);
      break;
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
float BezierController::TangentFunction(float t, int index)
{
   // note on this code - I don't think this works?
   // The curve is calculated using 4 points and I see only 3 used below
   float result = 0.0f;

   switch (index)
   {
   case 1:
      result = -3.0f + ((3.0f * t) * (2.0f - t));
      break;

   case 2:
      result = 3.0f - ( (3.0f * t) * (4.0f + (3.0f * t)) );
      break;

   case 3:
      result = 3.0f * powf(t, 2.0f);
      break;

   default:
      assert(0);
      break;
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::CheckCreatePath()
{
   bool shouldCreatePath = false;

   CurveNode* current = mStartNode.get();

   while (current)
   {
      if (current->GetDirtyFlag())
      {
         shouldCreatePath = true;
         current->SetDirtyFlag(false);
         //note: we wont break cause we need
         //to reset all dirty flags
      }

      current = current->GetNext();
   }

   if (shouldCreatePath)
   {
      CreatePath();
      ResetIterators();
   }
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::RenderProxyNode(bool pEnable)
{
  mRenderGeode = pEnable;
  if (mRenderGeode)
  {
     GetOSGNode()->asGroup()->addChild(mGeode.get());
  }
  else
  {
     GetOSGNode()->asGroup()->removeChild(mGeode.get());
  }
}

////////////////////////////////////////////////////////////////////////////////
bool BezierController::OnNextStep()
{
   if (!mLastPathPoint) { return false; }

   while (mCurrentPoint != mEndPoint && mTotalTime > (*mCurrentPoint).mTime)
   {
      mLastPathPoint = &(*mCurrentPoint);
      ++mCurrentPoint;
   }

   //if we reached the end of the path0
   //it means our time step was greater than
   //our path point resolution so we'll just stop
   //at the last valid path point
   if (mCurrentPoint == mEndPoint)
   {
      StepObject(mLastPathPoint->mPoint);

      mPath.clear();

      if (mShouldLoop)
      {
         // This needs to be false in order to restart
         mIsRunning = false;

         // Notify that the restart is happening.
         OnRestart();

         // Restart from the beginning
         Start();   

         return true;
      }

      OnEnd();

      return false;
   }

   //else if our elapsed time is equal to the next points time
   //ie. the step for the controller = the step for the last BezierNode
   //then we just move to the next point
   if (std::abs(mCurrentPoint->mTime - mTotalTime) < 0.0001f)
   {
      StepObject(mCurrentPoint->mPoint);
   }

   //else if our elapsed time is equal to the previous points time
   //just move the the previous point
   else if (std::abs(mLastPathPoint->mTime - mTotalTime) < 0.0001f)
   {
      StepObject(mLastPathPoint->mPoint);
   }

   //if our elapsed time is between the last and next pathpoint
   //we will interpolate between the two points
   else if (mLastPathPoint->mTime <= mTotalTime && mCurrentPoint->mTime >= mTotalTime)
   {
      float ratio = (mTotalTime - mLastPathPoint->mTime) / (mCurrentPoint->mTime - mLastPathPoint->mTime);
      
      // position
      osg::Vec3 from = mLastPathPoint->mPoint.GetPosition();
      osg::Vec3 to = mCurrentPoint->mPoint.GetPosition();
      osg::Vec3 position = (from * (1.0f - ratio)) + (to * ratio);

      // rotation
      osg::Quat rotation;
      rotation.slerp(ratio, mLastPathPoint->mPoint.GetOrientation(), mCurrentPoint->mPoint.GetOrientation());

      StepObject(PathPoint(position, rotation));
   }
   else
   {
      //else something went wrong
      assert(0);
   }

   SignalNextStep.emit_signal(*this);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::SetStartNode(BezierNode* pStart)
{
   mStartNode = pStart;
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::OnStart()
{
   CreatePath();
   ResetIterators();

   SignalStarted.emit_signal(*this);
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::OnRestart()
{
   SignalRestarted.emit_signal(*this);
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::OnPause()
{
   SignalPaused.emit_signal(*this);
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::OnUnPause()
{
   SignalUnPaused.emit_signal(*this);
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::OnEnd()
{
   SignalEnded.emit_signal(*this);
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::SetFollowPath(bool pFollowPath)
{
   mFollowPath = pFollowPath;
}

////////////////////////////////////////////////////////////////////////////////
bool BezierController::GetFollowPath() const
{
   return mFollowPath;
}

////////////////////////////////////////////////////////////////////////////////
void BezierController::SetLooping(bool shouldLoop)
{
   mShouldLoop = shouldLoop;
}

////////////////////////////////////////////////////////////////////////////////
bool BezierController::GetLooping() const
{
   return mShouldLoop;
}
////////////////////////////////////////////////////////////////////////////////
//our drawable
void BezierController::BezierPathDrawable::drawImplementation(osg::RenderInfo& /*renderInfo*/) const
{
   mController->CheckCreatePath();

   //we must maintain a copy of the path
   //since this is called on a separate thread
   if (mController->GetPathChanged())
   {
      mController->GetCopyPath(mPath);
      mController->SetPathChanged(false);
   }

   if (mPath.empty())
   {
      return;
   }

   std::list<PathData>::const_iterator iter = mPath.begin();
   std::list<PathData>::const_iterator endOfList = mPath.end();


   glBegin(GL_LINES);

   glColor3f(0.0f, 1.0f, 0.35f);

   osg::Vec3 lastPoint = (*iter).mPoint.GetPosition();
   ++iter;

   while (iter != endOfList)
   {
      osg::Vec3 point = (*iter).mPoint.GetPosition();
      glVertex3fv(&lastPoint[0]);
      glVertex3fv(&point[0]);

      lastPoint = point;
      ++iter;
   }

   glEnd();
}

////////////////////////////////////////////////////////////////////////////////

} // namespace dtABC

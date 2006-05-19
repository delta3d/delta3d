#include <dtABC/beziercontroller.h>
#include <dtCore/scene.h>
#include <osg/Vec3>
#include <osg/Quat>

#include <assert.h>

namespace dtABC
{

const std::string BezierController::BEZIER_CONTROLLER_GEODE_ID("__DELTA3D_BEZIER_CONTROLLER__");

BezierController::BezierController()
{
   mLastPathPoint = NULL;
   mPathChanged = false;

   mDrawable = new BezierPathDrawable;
   mDrawable->SetPath(this);
   mGeode = new osg::Geode();
   mGeode->setName(BEZIER_CONTROLLER_GEODE_ID);
   mGeode->addDrawable(mDrawable.get());
   osg::StateSet* ss = mGeode->getOrCreateStateSet();
   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   mRenderGeode = false;

   SetOSGNode( new osg::Group );
}

BezierController::~BezierController()
{

}

void BezierController::CreatePath()
{
   if(!mStartNode || !mStartNode->GetNext())
      return;

   //we set this flag, so the draw implementation 
   //knows not to touch this
   mPathChanged = false;

   mPath.clear();

   const BezierNode* pCurrentNode = mStartNode->GetBezierInterface();
   const BezierNode* pNextNode = pCurrentNode->GetNext()->GetBezierInterface();

   float elapsedTime = 0.0;

   while(pCurrentNode && pNextNode && pCurrentNode->GetExit() && pNextNode->GetEntry())
   {
      float dt = pCurrentNode->GetStep();
      float totalTime = pCurrentNode->GetTimeToNext();
      float multiply = 1.0f / totalTime;

      if(dt < 0.000001f) dt = 0.05f;

      for(float j = 0.0f; j < totalTime; j+= dt)
      {
         //note this is simplified through extending PathPointConverter
         //which has an implicit conversion from a dtCore::Transformable to a PathPoint
         MakeSegment(elapsedTime, j * multiply, pCurrentNode->GetPathPoint(), pCurrentNode->GetExit()->GetPathPoint(), pNextNode->GetEntry()->GetPathPoint(), pNextNode->GetPathPoint());

         elapsedTime += dt;
      }

      pCurrentNode = pNextNode;
      if(pCurrentNode->GetNext())
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


void BezierController::MakeSegment(float time, float inc, const PathPoint& p1, const PathPoint& p2, const PathPoint& p3, const PathPoint& p4)
{

   osg::Vec3 pos, tangent;

   for(int i = 0; i < 3; ++i)
   {
      pos[i] = (BlendFunction(inc,0) * p1.GetPosition()[i]) + (BlendFunction(inc,1) * p2.GetPosition()[i]) + (BlendFunction(inc,2) * p3.GetPosition()[i]) + (BlendFunction(inc,3) * p4.GetPosition()[i]); 
      //tangent[i] = (TangentFunction(inc,1) * p2.GetPosition()[i]) + (TangentFunction(inc,2) * p3.GetPosition()[i]) + (TangentFunction(inc,3) * p4.GetPosition()[i]); 
   }

   osg::Quat quat;
   quat.slerp(1.0 - inc, p4.GetOrientation(), p1.GetOrientation());

   PathData pd;
   pd.mTime = time;
   pd.mPoint.SetPosition(pos);
   pd.mPoint.SetOrientation(quat);

   mPath.push_back(pd);
}


float BezierController::BlendFunction(float t, int index)
{
   float result = 0.0f;

   switch(index)
   {

   case 0:
      result = powf(1.0f - t, 3.0f);
      break;

   case 1:
      result = 3.0f * t * powf(1.0f - t, 2.0f);
      break;

   case 2:
      result = 3.0f * powf(t, 2.0f) * (1.0f - t);
      break;

   case 3:
      result = powf(t, 3.0f);
      break;


   default:
      assert(0);
      break;
   }

   return result;
}



float BezierController::TangentFunction(float t, int index)
{
   float result = 0.0f;

   switch(index)
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


void BezierController::CheckCreatePath()
{

   bool shouldCreatePath = false;

   CurveNode* current = mStartNode.get();

   while(current)
   {
      if(current->GetDirtyFlag())
      {
         shouldCreatePath = true;
         current->SetDirtyFlag(false);
         //note: we wont break cause we need 
         //to reset all dirty flags
      }
       
      current = current->GetNext();
   }

   if(shouldCreatePath)
   {
      CreatePath();
   }
}


void BezierController::RenderProxyNode(bool pEnable)
{
  mRenderGeode = pEnable;
  if(mRenderGeode)
  {
     GetOSGNode()->asGroup()->addChild(mGeode.get());
  }
  else
  {
     GetOSGNode()->asGroup()->removeChild(mGeode.get());
  }
}


bool BezierController::OnNextStep()
{
   if(!mLastPathPoint) return false;


   while(mCurrentPoint != mEndPoint && mTotalTime > (*mCurrentPoint).mTime)
   {
      mLastPathPoint = &(*mCurrentPoint);
      ++mCurrentPoint;
   }

   const PathPoint& p = (*mCurrentPoint).mPoint;
   float currentTime = (*mCurrentPoint).mTime;

   //if we reached the end of the path
   //it means our time step was greater than
   //our path point resolution so we'll just stop 
   //at the last valid path point
   if(mCurrentPoint == mEndPoint)
   {
      StepObject(mLastPathPoint->mPoint);
      mPath.clear();
      return false;
   }

   //else if our elapsed time is equal to the next points time
   //ie. the step for the controller = the step for the last BezierNode
   //then we just move to the next point
   else if(fabs(currentTime - mTotalTime) < 0.0001)
   {
      StepObject(p);      
   }
   
   //else if our elapsed time is equal to the previous points time
   //just move the the previous point
   else if(fabs(mLastPathPoint->mTime - mTotalTime) < 0.0001)
   {
      StepObject(mLastPathPoint->mPoint);      
   }

   //if our elapsed time is between the last and next pathpoint
   //we will interpolate between the two points 
   else if(mLastPathPoint->mTime <= mTotalTime && currentTime >= mTotalTime)
   {
      float perc = (mTotalTime - mLastPathPoint->mTime) / (currentTime  -  mLastPathPoint->mTime );
      
      osg::Vec3 from = p.GetPosition();
      osg::Vec3 to = mLastPathPoint->mPoint.GetPosition();

      osg::Vec3 vec;
      vec[0] = ((1.0 - perc) * from[0]) + (perc * to[0]);
      vec[1] = ((1.0 - perc) * from[1]) + (perc * to[1]);
      vec[2] = ((1.0 - perc) * from[2]) + (perc * to[2]);

      osg::Quat quat;
      quat.slerp(perc, mLastPathPoint->mPoint.GetOrientation(), p.GetOrientation());
      
      StepObject(PathPoint(vec, quat));
   }
   else
   {
      //else something went wrong 
      assert(0);
   }

   return true;
}


void BezierController::SetStartNode(BezierNode* pStart)
{
   mStartNode = pStart;
}

void BezierController::OnStart()
{
   CreatePath();
   mLastPathPoint = NULL;

   mCurrentPoint = mPath.begin();
   mEndPoint = mPath.end();

   if(mPath.size() > 2)
   {
      StepObject((*mCurrentPoint).mPoint);
      mLastPathPoint = &(*mCurrentPoint);
      ++mCurrentPoint;
   }
}


void BezierController::OnPause()
{


}

void BezierController::OnUnPause()
{


}



//our drawable
void BezierController::BezierPathDrawable::drawImplementation(osg::State& state) const 
{
   mController->CheckCreatePath();

   //we must maintain a copy of the path
   //since this is called on a separate thread
   if(mController->GetPathChanged())
   {
      mController->GetCopyPath(mPath);
      mController->SetPathChanged(false);
   }

   std::list<PathData>::const_iterator iter = mPath.begin();
   std::list<PathData>::const_iterator endOfList = mPath.end();

   glLineWidth(3.0f);


   glBegin(GL_LINES);

   glColor3f(1.0f, 1.0f, 1.0f);
   glColor3f(0.0f, 1.0f, 0.35f);

   osg::Vec3 lastPoint = (*iter).mPoint.GetPosition();
   ++iter;

   while(iter != endOfList)
   {  
      osg::Vec3 point = (*iter).mPoint.GetPosition();
      glVertex3fv(&lastPoint[0]);
      glVertex3fv(&point[0]);

      lastPoint = point;
      ++iter;
   }

   glEnd();
}



}//namespace dtABC


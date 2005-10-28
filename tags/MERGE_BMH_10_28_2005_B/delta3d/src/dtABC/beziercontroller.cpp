#include <dtABC/beziercontroller.h>
#include <dtCore/scene.h>
#include <assert.h>

namespace dtABC
{


BezierController::BezierController()
{
   mDrawable.SetPath(this);
   osg::Geode* pGeode = new osg::Geode();
   pGeode->addDrawable(&mDrawable);
   mProxyNode = pGeode;
}

BezierController::~BezierController()
{

}

void BezierController::CreatePath()
{
   mPath.clear();

   const BezierNode* pCurrentNode = mStartNode->GetBezierInterface();
   const BezierNode* pNextNode = pCurrentNode->GetNext()->GetBezierInterface();

   while(pCurrentNode && pNextNode)
   {
      float dt = pCurrentNode->GetStep();
      float totalTime = pCurrentNode->GetTimeToNext();
      float multiply = 1.0f / totalTime;

      for(float j = 0; j < totalTime; j+= dt)
      {
         //note this is simplified through extending PathPointConverter
         //which has an implicit conversion from a dtCore::Transformable to a PathPoint
         MakeSegment(j * multiply, pCurrentNode->GetPathPoint(), pCurrentNode->GetExit()->GetPathPoint(), pNextNode->GetEntry()->GetPathPoint(), pNextNode->GetPathPoint());
      }

      pCurrentNode = pNextNode;
      pNextNode = pCurrentNode->GetNext()->GetBezierInterface();
   }

}


void BezierController::MakeSegment(float inc, const PathPoint& p1, const PathPoint& p2, const PathPoint& p3, const PathPoint& p4)
{

   osg::Vec3 pos, tangent;

   for(int i = 0; i < 3; ++i)
   {
      pos[i] = (BlendFunction(inc,0) * p1.GetPosition()[i]) + (BlendFunction(inc,1) * p2.GetPosition()[i]) + (BlendFunction(inc,2) * p3.GetPosition()[i]) + (BlendFunction(inc,3) * p4.GetPosition()[i]); 
      tangent[i] = (TangentFunction(inc,1) * p2.GetPosition()[i]) + (TangentFunction(inc,2) * p3.GetPosition()[i]) + (TangentFunction(inc,3) * p4.GetPosition()[i]); 
   }

   osg::Quat quat;
   quat.slerp(inc, p1.GetOrientation(), p4.GetOrientation());

   mPath.push_back(PathPoint(pos, quat));
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


//our drawable
void BezierController::BezierPathDrawable::drawImplementation(osg::State& state) const
{

   std::list<PathPoint>::iterator iter = mController->mPath.begin();
   std::list<PathPoint>::iterator endOfList = mController->mPath.end();

   glBegin(GL_LINES);

   glColor3f(1.0f, 0.35f, 0.35f);

   while(iter != endOfList)
   {  
      osg::Vec3 point = (*iter).GetPosition();
      glVertex3fv(&point[0]);

      ++iter;
   }

   glEnd();
}


void BezierController::RenderProxyNode(bool pEnable)
{
   if(pEnable)
   {
      mNode = mProxyNode.get();
   }
   else
   {
      mNode = 0;
   }
}





bool BezierController::OnNextStep()
{
   StepObject(*mCurrentPoint);

   ++mCurrentPoint;
   return (mCurrentPoint != mEndPoint);
}


void BezierController::SetStartNode(BezierNode* pStart)
{
   mStartNode = pStart;
}

void BezierController::OnStart()
{
   CreatePath();

   mCurrentPoint = mPath.begin();
   mEndPoint = mPath.end();
}


void BezierController::OnPause()
{


}

void BezierController::OnUnPause()
{


}



}//namespace dtABC


#include <dtABC/bezierpath.h>
#include <assert.h>


namespace dtABC
{

BezierPath::BezierPath()
{

}

BezierPath::~BezierPath()
{

}

void BezierPath::CreatePath()
{
   mPath.clear();

   BezierNode* pCurrentNode = mStartNode->GetBezierInterface();
   BezierNode* pNextNode = pCurrentNode->GetNext()->GetBezierInterface();

   while(pCurrentNode && pNextNode)
   {
      float dt = pCurrentNode->GetStep();
      float totalTime = pCurrentNode->GetTimeToNext();
      float multiply = 1.0f / totalTime;

      for(float j = 0; j < totalTime; j+= dt)
      {
         //note this is simplified through extending PathPointConverter
         //which has an implicit conversion from a dtCore::Transformable to a PathPoint
         MakeSegment(j * multiply, *pCurrentNode, *(pCurrentNode->GetExit()), *(pNextNode->GetEntry()), *pNextNode);
      }
      
      pCurrentNode = pNextNode;
      pNextNode = pCurrentNode->GetNext()->GetBezierInterface();
   }

}


void BezierPath::MakeSegment(float inc, const PathPoint& p1, const PathPoint& p2, const PathPoint& p3, const PathPoint& p4)
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


float BezierPath::BlendFunction(float t, int index)
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



float BezierPath::TangentFunction(float t, int index)
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


}//namespace dtABC


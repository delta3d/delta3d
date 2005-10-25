#ifndef __BEZIER_PATH_H__
#define __BEZIER_PATH_H__

#include <list>
#include <osg/Drawable>
#include <dtCore/refptr.h>
#include <dtCore/deltadrawable.h>
#include "export.h"
#include "beziernode.h"
#include "pathpoint.h"


/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* @author Bradley Anderegg
*/

namespace dtABC
{

/***
* BezierPath is a class used to represent a Bezier Curve
* 
*/

class DT_ABC_EXPORT BezierPath: public dtCore::DeltaDrawable
{
private:
   class BezierPathDrawable: public osg::Drawable
   {
      public:
         /*virtual*/ osg::Object* cloneType() const {return 0;}; 
         /*virtual*/ osg::Object* clone(const osg::CopyOp& copyop) const{return 0;} 

         /*virtual*/ void drawImplementation(osg::State& state) const;
         void SetPath(BezierPath* pPath){mBezierPath = pPath;}
      private:
         BezierPath* mBezierPath;
   };

   friend class BezierPathDrawable;

public:

   BezierPath();
   
   /***
   * Creates the path using the start node to traverse the curve
   * the time step and time between nodes is encapsulated by the 
   * BezierNodes
   */
   void CreatePath();

   /***
   * @return a const reference to the path created on CreatePath()
   */
   const std::list<PathPoint>& GetPath() const {return mPath;}

   const BezierNode* GetStartNode() const {return mStartNode.get();}
   BezierNode* GetStartNode(){return mStartNode.get();}

   void SetStartNode(BezierNode* pStart){mStartNode = pStart;}

   /*virtual*/ void RenderProxyNode(bool enable);

protected:
  /*virtual*/~BezierPath();
   BezierPath(const BezierPath&); //not implemented by design
   BezierPath operator =(const BezierPath&); //not implemented by design



private:

   void MakeSegment(float inc, const PathPoint& p1, const PathPoint& p2, const PathPoint& p3, const PathPoint& p4);
   float BlendFunction(float t, int index);
   float TangentFunction(float t, int index);

   dtCore::RefPtr<BezierNode> mStartNode;
   std::list<PathPoint> mPath;

   BezierPathDrawable   mDrawable;
};


}//namespace dtABC


#endif //__BEZIER_PATH_H__



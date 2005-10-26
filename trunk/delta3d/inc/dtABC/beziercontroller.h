#ifndef __BEZIER_CONTROLLER_H__
#define __BEZIER_CONTROLLER_H__

#include <list>
#include <osg/Drawable>
#include <osg/Geode>

#include "export.h"
#include "pathpoint.h"
#include "beziernode.h"
#include "motionaction.h"

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

class DT_ABC_EXPORT BezierController: public MotionAction
{

private:
   class BezierPathDrawable: public osg::Drawable
   {
   public:
      /*virtual*/ osg::Object* cloneType() const {return 0;}; 
      /*virtual*/ osg::Object* clone(const osg::CopyOp& copyop) const{return 0;} 

      /*virtual*/ void drawImplementation(osg::State& state) const;
      void SetPath(BezierController* pPath){mController = pPath;}
   private:
      BezierController* mController;
   };

   friend class BezierPathDrawable;

public:
   BezierController();
   
   const BezierNode* GetStartNode() const {return mStartNode.get();}
   void SetStartNode(BezierNode* pStart);

   /*virtual*/ void RenderProxyNode(bool enable);

   
protected:
   ~BezierController();
   BezierController(const BezierController&); //not implemented by design
   BezierController operator =(const BezierController&); //not implemented by design

   /***
   * Creates the path using the start node to traverse the curve
   * the time step and time between nodes is encapsulated by the 
   * BezierNodes
   */
   virtual void CreatePath();

   /*virtual*/ bool OnNextStep();
   /*virtual*/ void OnStart();
   /*virtual*/ void OnPause();
   /*virtual*/ void OnUnPause();
   /*virtual*/ void OnRestart();


private:

   void MakeSegment(float inc, const PathPoint& p1, const PathPoint& p2, const PathPoint& p3, const PathPoint& p4);
   float BlendFunction(float t, int index);
   float TangentFunction(float t, int index);



   dtCore::RefPtr<BezierNode>                mStartNode;

   BezierPathDrawable                        mDrawable;

   std::list<PathPoint>                      mPath;
   std::list<PathPoint>::const_iterator      mCurrentPoint;
   std::list<PathPoint>::const_iterator      mEndPoint;

};



} //namespace dtABC




#endif //__BEZIER_CONTROLLER_H__


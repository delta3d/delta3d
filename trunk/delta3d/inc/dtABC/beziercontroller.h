#ifndef __BEZIER_CONTROLLER_H__
#define __BEZIER_CONTROLLER_H__

#include <list>
#include <osg/Drawable>
#include <osg/Geode>

#include "export.h"
#include <osg/Object>
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

   typedef struct 
   {
      PathPoint mPoint;
      float mTime;

   }PathData;

   
   class BezierPathDrawable: public osg::Drawable
   {
   public:
      
      META_Object(dtABC, BezierPathDrawable);
      BezierPathDrawable(const BezierPathDrawable& bd, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
      {
         mController = bd.GetController();
      }

      BezierPathDrawable(){setUseDisplayList(false);}
   
      /*virtual*/ void drawImplementation(osg::State& state) const;
      void SetPath(BezierController* pPath){mController = pPath;}
      BezierController* GetController()const{return mController;}
   
   private:
      BezierController*                     mController;
      mutable std::list<PathData>           mPath;
   };

   friend class BezierPathDrawable;

public:

   ///Used to identify the BezierController.
   static const std::string BEZIER_CONTROLLER_GEODE_ID;

   BezierController();
   
   BezierNode* GetStartNode() {return mStartNode.get();}
   const BezierNode* GetStartNode() const {return mStartNode.get();}
   void SetStartNode(BezierNode* pStart);

   /*virtual*/ void RenderProxyNode(bool enable);

   
   /***
   * Creates the path using the start node to traverse the curve
   * the time step and time between nodes is encapsulated by the 
   * BezierNodes
   */
   virtual void CreatePath();

   /*virtual*/ osg::Node* GetOSGNode(); 

   void CheckCreatePath();

   void GetCopyPath(std::list<PathData>& pPathIn) const{pPathIn = mPath;}

 
protected:
   ~BezierController();
   BezierController(const BezierController&); //not implemented by design
   BezierController operator =(const BezierController&); //not implemented by design


   /*virtual*/ bool OnNextStep();
   /*virtual*/ void OnStart();
   /*virtual*/ void OnPause();
   /*virtual*/ void OnUnPause();
   /*virtual*/ void OnRestart();


private:

   bool GetPathChanged() const{return mPathChanged;}
   void SetPathChanged(bool b){mPathChanged = b;}


   void MakeSegment(float time, float inc, const PathPoint& p1, const PathPoint& p2, const PathPoint& p3, const PathPoint& p4);
   float BlendFunction(float t, int index);
   float TangentFunction(float t, int index);



   bool                                      mRenderGeode;
   bool                                      mPathChanged;
   dtCore::RefPtr<BezierNode>                mStartNode;
   dtCore::RefPtr<osg::Geode>                mGeode;

   dtCore::RefPtr<BezierPathDrawable>        mDrawable;

   const PathData*                          mLastPathPoint;
   std::list<PathData>                      mPath;
   std::list<PathData>::const_iterator      mCurrentPoint;
   std::list<PathData>::const_iterator      mEndPoint;

};



} //namespace dtABC




#endif //__BEZIER_CONTROLLER_H__


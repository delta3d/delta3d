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
 * Bradley Anderegg
 */

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

#include <dtCore/sigslot.h>

namespace dtABC
{

class DT_ABC_EXPORT BezierController: public MotionAction
{
public:

   struct PathData
   {
      PathPoint mPoint;
      float mTime;
   };

private:

   class BezierPathDrawable: public osg::Drawable
   {
   public:

      META_Object(dtABC, BezierPathDrawable);
      BezierPathDrawable(const BezierPathDrawable& bd, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
      {
         mController = bd.GetController();
      }

      BezierPathDrawable() { setUseDisplayList(false); }


      /*virtual*/ void drawImplementation(osg::RenderInfo& renderInfo) const;
      void SetPath(BezierController* pPath)  { mController = pPath; }
      BezierController* GetController()const { return mController; }

   private:
      BezierController*           mController;
      mutable std::list<PathData> mPath;
   };

   friend class BezierPathDrawable;

public:

   ///Used to identify the BezierController.
   static const std::string BEZIER_CONTROLLER_GEODE_ID;

   sigslot::signal1<BezierController&> SignalStarted;
   sigslot::signal1<BezierController&> SignalPaused;
   sigslot::signal1<BezierController&> SignalUnPaused;
   sigslot::signal1<BezierController&> SignalRestarted;
   sigslot::signal1<BezierController&> SignalNextStep;
   sigslot::signal1<BezierController&> SignalEnded;

   BezierController();

   BezierNode* GetStartNode()             { return mStartNode.get(); }
   const BezierNode* GetStartNode() const { return mStartNode.get(); }
   void SetStartNode(BezierNode* pStart);

   /*virtual*/ void RenderProxyNode(bool enable);
   bool GetRenderProxyNode() { return mRenderGeode; }

   /** Will orient the path points to align with the bezier curve, if enabled.
     * Otherwise, the orientation of the path points will be slerp'd to the next
     * point.
     * @param pFollowPath True, to enable the path following - the orientation of
     *        the nodes will be ignored.  False, to use the orientation of the
     *        nodes to calculate the path point orientation.
     */
   void SetFollowPath(bool pFollowPath);

   /** @return true if the BezierController is using the "Follow Path" mode.
     *         False otherwise.
     */
   bool GetFollowPath() const;

   /***
    * Creates the path using the start node to traverse the curve
    * the time step and time between nodes is encapsulated by the
    * BezierNodes
    */
   virtual void CreatePath();

   void CheckCreatePath();

   void GetCopyPath(std::list<PathData>& pPathIn) const { pPathIn = mPath; }

   /** Set the BezierController to automatically restart the path when it reaches
     * the end.
     * @param shouldLoop True to automatically loop the path, false to increment
     *                   through the path once.
     */
   void SetLooping(bool shouldLoop);

   /** Is the BezierController set to automatically restart the path.
     * @return true if looping, false otherwise.
     */
   bool GetLooping() const;

protected:
   ~BezierController();
   BezierController(const BezierController&); //not implemented by design
   BezierController operator =(const BezierController&); //not implemented by design

   /*virtual*/ bool OnNextStep();
   /*virtual*/ void OnStart();
   /*virtual*/ void OnPause();
   /*virtual*/ void OnUnPause();
   /*virtual*/ void OnRestart();
   /*virtual*/ void OnEnd();

private:

   bool GetPathChanged() const { return mPathChanged; }
   void SetPathChanged(bool b) { mPathChanged = b; }

   void ResetIterators();

   void MakeSegment(float time,
                    float t,
                    const PathPoint& p1CurrentNode,
                    const PathPoint& p2CurrentNodeExit,
                    const PathPoint& p3NextNodeEntry,
                    const PathPoint& p4NextNode);

   float BlendFunction(float t, int index);
   float TangentFunction(float t, int index); 
   float DerivativeFunction(float t, int index) const;
   bool                                mRenderGeode;
   bool                                mPathChanged;
   bool                                mShouldLoop;
   bool                                mFollowPath; //we orient along path instead of slerping nodes
   dtCore::RefPtr<BezierNode>          mStartNode;
   dtCore::RefPtr<osg::Geode>          mGeode;

   dtCore::RefPtr<BezierPathDrawable>  mDrawable;

   const PathData*                     mLastPathPoint;
   std::list<PathData>                 mPath;
   std::list<PathData>::const_iterator mCurrentPoint;
   std::list<PathData>::const_iterator mEndPoint;
};


} // namespace dtABC

#endif //__BEZIER_CONTROLLER_H__


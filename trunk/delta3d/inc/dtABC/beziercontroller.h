#ifndef __BEZIER_CONTROLLER_H__
#define __BEZIER_CONTROLLER_H__

#include <list>

#include "export.h"
#include "pathpoint.h"
#include "beziernode.h"
#include "bezierpath.h"
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

public:
   BezierController();
   

   /***
   * This sets the start node for the encapsulated BezierPath
   * @note the path is created when this node is assigned
   *     and must be linked with all appropriate child nodes
   */
   void SetStartNode(BezierNode* pStart);

   BezierNode* GetStartNode();

   /***
   * Sets this to use RenderProxyNode from its Path
   * @return returns the path as a drawable
   *   that can be added to the scene
   */
   void SetRenderProxyNode(bool pEnable);
   
protected:
   ~BezierController();
   BezierController(const BezierController&); //not implemented by design
   BezierController operator =(const BezierController&); //not implemented by design

   /*virtual*/ bool OnNextStep();
   /*virtual*/ void OnStart();
   /*virtual*/ void OnPause();
   /*virtual*/ void OnRestart();



private:

   /***
   * Holds our actual path
   */
   dtCore::RefPtr<BezierPath>                mPath;

   /***
   * Holds the current position in our path
   */
   std::list<PathPoint>::const_iterator      mCurrentPoint;
   std::list<PathPoint>::const_iterator      mEndPoint;

};



} //namespace dtABC




#endif //__BEZIER_CONTROLLER_H__


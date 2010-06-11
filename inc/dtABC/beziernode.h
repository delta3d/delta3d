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

#ifndef __BEZIER_NODE_H__
#define __BEZIER_NODE_H__

#include <dtCore/refptr.h>
#include "curvenode.h"
#include "beziercontrolpoint.h"

namespace dtABC
{


/***
 *  BezierNode is a class used to represent a waypoint
 *  on a BezierCurve, it derives from CurveNode and has
 *  an entry and exit control point attached to it
 */
class BezierNode : public CurveNode
{
public:
   BezierNode() { mDirtyFlag = true; }

   BezierControlPoint* GetEntry() { return mEntry.get(); }
   const BezierControlPoint* GetEntry() const { return mEntry.get(); }

   BezierControlPoint* GetExit() { return mExit.get(); }
   const BezierControlPoint* GetExit() const { return mExit.get(); }

   void SetEntry(BezierControlPoint* pEntry) {mEntry = pEntry; mDirtyFlag = true;}
   void SetExit(BezierControlPoint* pExit) {mExit = pExit; mDirtyFlag = true;}

   /*virtual*/ const BezierNode* GetBezierInterface() const { return this; }
   /*virtual*/ BezierNode* GetBezierInterface() { return this; }

protected:
   /*virtual*/ ~BezierNode(){};
   BezierNode(const BezierNode&); //not implemented by design
   BezierNode operator=(const BezierNode&); //not implemented by design


   dtCore::RefPtr<BezierControlPoint> mEntry;
   dtCore::RefPtr<BezierControlPoint> mExit;
};

} // namespace dtABC

#endif //__BEZIER_NODE_H__



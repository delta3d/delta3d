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

#ifndef __CURVE_NODE_H__
#define __CURVE_NODE_H__

#include <dtCore/refptr.h>
#include "pathpointconverter.h"

namespace dtABC
{
   class BezierNode;

/***
 * CurveNode class is the base class for a curve,
 * it has a next, previous, time to next point and step time
 */

class CurveNode: public PathPointConverter
{
  public:
     CurveNode() : mPrev(0), mNext(0), mTimeToNext(0.0f), mStep(0) {}

     const CurveNode* GetPrev() const { return mPrev.get(); }
     const CurveNode* GetNext() const { return mNext.get(); }

     CurveNode* GetPrev() { return mPrev.get(); }
     CurveNode* GetNext() { return mNext.get(); }

     void SetNext(CurveNode* pNext){ mNext = pNext; mDirtyFlag = true; }
     void SetPrev(CurveNode* pPrev){ mPrev = pPrev; mDirtyFlag = true; }

     float GetTimeToNext() const { return mTimeToNext; }
     float GetStep() const { return mStep; }

     void SetTimeToNext(float pTimeToNext){ mTimeToNext = pTimeToNext; mDirtyFlag = true; }
     void SetStep(float pStep){ mStep = pStep; mDirtyFlag = true; }

     void SetDirtyFlag(bool pDirty){ mDirtyFlag = pDirty; }
     bool GetDirtyFlag() const { return mDirtyFlag; }

     // overloads to avoid casting
     virtual const BezierNode* GetBezierInterface() const { return 0; }
     virtual BezierNode* GetBezierInterface() { return 0; }


  protected:
   /*virtual*/ ~CurveNode(){}
   CurveNode(const CurveNode&); //not implemented by design
   CurveNode operator=(const CurveNode&); //not implemented by design


   dtCore::RefPtr<CurveNode> mPrev;
   dtCore::RefPtr<CurveNode> mNext;

   float mTimeToNext;
   float mStep;

   bool mDirtyFlag;

};

} // namespace dtABC


#endif // __CURVE_NODE_H__


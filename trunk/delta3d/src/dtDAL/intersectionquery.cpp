/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Matthew W. Campbell
*/
#include "dtDAL/intersectionquery.h"
#include "dtDAL/exception.h"
#include <stack>
#include <osgDB/WriteFile>
#include <osgUtil/IntersectVisitor>
#include <iostream>

namespace dtDAL 
{

    class IntersectionVisitor : public osgUtil::IntersectVisitor 
    {
    public:
        IntersectionVisitor() { }
        virtual ~IntersectionVisitor() { }

        virtual void apply(osg::Billboard &node) 
        {
            osgUtil::IntersectVisitor::apply((osg::Geode &)node);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    IntersectionQuery::IntersectionQuery() : mStart(0,0,0), mDirection(0,1,0)
    {
        mLineSegment = new osg::LineSegment();
        mLineLength = 1000000.0f;
        mUpdateLineSegment = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    IntersectionQuery::IntersectionQuery(const osg::Vec3 &start,
        const osg::Vec3 &dir) : mStart(start), mDirection(dir)
    {
        mLineSegment = new osg::LineSegment();
        mLineLength = 1000000.0f;
        mUpdateLineSegment = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool IntersectionQuery::Exec()
    {
        if(!mSceneRoot.valid()) 
        {
            EXCEPT(ExceptionEnum::BaseException,
                   "Attempting to perform an intersection query on an invalid scene.");
        }

        //Make sure our line segment is correct.
        if(mUpdateLineSegment)
            CalcLineSegment();

        //We could do this recursively, but to save a few clock cycles, lets use
        //our own stack object and do it in an interative fashion.
        std::stack<dtCore::DeltaDrawable *> objects;
        IntersectionVisitor iv;
        unsigned int i;
        bool hitFlag = false;

        objects.push(mSceneRoot.get());
        iv.addLineSegment(mLineSegment.get());

        while (!objects.empty()) 
        {
            dtCore::DeltaDrawable *drawable = objects.top();
            objects.pop();

            //We should only test intersections with leaf nodes in the scene.
            //So if its an internal node, just push its children on the stack.
            //If its a leaf node, then test its bounding volume information.
            if (drawable->GetNumChildren() != 0) 
            {
                //Internal node.
                for (i=0; i<drawable->GetNumChildren(); i++)
                    objects.push(drawable->GetChild(i));
            }
            else 
            {
                drawable->GetOSGNode()->accept(iv);
                if (iv.hits()) 
                {
                    HitRecord hit;
                    hitFlag = true;
                    hit.drawable = drawable;
                    hit.ratio = iv.getHitList(mLineSegment.get())[0]._ratio;
                    hit.intersectionPoint = iv.getHitList(mLineSegment.get())[0].getWorldIntersectPoint();
                    mHitList.push_back(hit);
                }
            }
        }

        return hitFlag;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void IntersectionQuery::Reset()
    {
        mHitList.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::vector<IntersectionQuery::HitRecord> &IntersectionQuery::GetHitList()
    {
        std::sort(mHitList.begin(), mHitList.end());
        return mHitList;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void IntersectionQuery::CalcLineSegment()
    {
        //Make sure the current direction vector is normalized.
        mDirection.normalize();

        //Since we are working with line segments, we need to convert our ray
        //representation to a finite line.
        osg::Vec3 endPoint = mStart + (mDirection*mLineLength);
        mLineSegment->set(mStart,endPoint);
        mUpdateLineSegment = false;
    }
}

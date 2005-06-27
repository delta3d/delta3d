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
#ifndef __IntersectionQuery__h
#define __IntersectionQuery__h

#include <osg/Vec3>
#include <osg/LineSegment>
#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <dtCore/export.h>

namespace dtDAL {

    class DT_EXPORT IntersectionQuery 
    {
    public:
        struct HitRecord 
        {
            bool operator<(const HitRecord &rhs) const 
            {
                return ratio < rhs.ratio;
            }

            osg::ref_ptr<dtCore::DeltaDrawable> drawable;
            float ratio;
            osg::Vec3 intersectionPoint;
        };

        IntersectionQuery();
        IntersectionQuery(const osg::Vec3 &start, const osg::Vec3 &dir);

        void SetQueryRoot(dtCore::DeltaDrawable *drawable) 
        {
            mSceneRoot = drawable;
        }

        void SetStartPos(const osg::Vec3 &start) 
        {
            mStart = start;
            mUpdateLineSegment = true;
        }

        const osg::Vec3 &GetStartPos() const { return mStart; }

        void SetDirection(const osg::Vec3 &dir)
        {
            mDirection = dir;
            mUpdateLineSegment = true;
        }

        const osg::Vec3 &GetDirection() const { return mDirection; }

        bool Exec();

        void Reset();

        std::vector<HitRecord> &GetHitList();

    private:
        osg::Vec3 mStart;
        osg::Vec3 mDirection;
        float mLineLength;
        bool mUpdateLineSegment;

        osg::ref_ptr<osg::LineSegment> mLineSegment;
        osg::ref_ptr<dtCore::DeltaDrawable> mSceneRoot;
        std::vector<HitRecord> mHitList;

        void CalcLineSegment();
    };

}

#endif

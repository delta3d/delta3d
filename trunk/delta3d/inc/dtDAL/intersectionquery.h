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
#include <osgUtil/IntersectVisitor>
#include <dtCore/deltadrawable.h>
#include <dtCore/export.h>

namespace dtCore
{
    class Scene;
}

namespace dtDAL {

    /**
     * This class exposes an API for intersecting a ray with an underlying Delta3D
     * scene.
     */
    class DT_EXPORT IntersectionQuery
    {
    public:

        /**
         * Constructs a new intersection query.
         * @param scene A Delta3D scene to intersect.  If this is NULL, a root
         *  drawable must be set.
         * @note The default ray has a starting position of (0,0,0) and a
         *  direction of (0,1,0).
         */
        IntersectionQuery(dtCore::Scene *scene = NULL);

        /**
         * Constructs a new intersection query using a ray constructed with the
         * specified parameters.
         * @param start The start of the ray.
         * @param dir The direction the ray is traveling.
         * @param scene The Delta3D scene to intersect.
         */
        IntersectionQuery(const osg::Vec3 &start, const osg::Vec3 &dir,
                          dtCore::Scene *scene = NULL);

        /**
         * Constructs a new intersection query using a line segment with the
         * specified parameters.
         * @param scene The Delta3D scene to intersect.
         * @param start The start of the line segment.
         * @param end The end point of the line segment.
         */
        IntersectionQuery(dtCore::Scene *scene, const osg::Vec3 &start,
            const osg::Vec3 &end);

        /**
         * Sets a drawable as the root of the intersection tests.  If this is specified,
         * it will take precedence over the currently assigned Delta3D scene.
         * @param drawable The drawable to intersect.
         */
        void SetQueryRoot(dtCore::DeltaDrawable *drawable) {
            mSceneRoot = drawable;
        }

        /**
         * Clears the currently assigned root drawable of the intersection tests.
         */
        void ClearQueryRoot() {
            mSceneRoot = NULL;
        }

        /**
         * Sets the starting position of the intersection ray.
         * @param start The start position.
         */
        void SetStartPos(const osg::Vec3 &start)
        {
            mStart = start;
            mUpdateLineSegment = true;
        }

        /**
         * Gets the starting position of the intersection ray.
         * @return A vector containing the start position of the intersection ray.
         */
        const osg::Vec3 &GetStartPos() const {
            return mStart;
        }

        /**
         * Sets the direction of the intersection ray.
         * @param dir The direction vector.  This is normalized before being assigned.
         */
        void SetDirection(const osg::Vec3 &dir)
        {
            mDirection = dir;
            mUpdateLineSegment = true;
        }

        /**
         * Gets the direction of the intersection ray.
         * @return The direction unit vector.
         */
        const osg::Vec3 &GetDirection() const {
            return mDirection;
        }

        /**
         * Ray traces the scene.
         * @return True if any intersections were detected.
         * @note If the query root has been set, only the query root drawable and its
         *  children are candidates for intersection.  If not, all drawables in the scene
         *  are possibilities.
         */
        bool Exec();

        /**
         * Resets the intersection query.  Call this in between disjoint intersection
         * executions.
         */
        void Reset();

        /**
         * Gets the DeltaDrawable that is closest to the query's starting point.
         * @return A valid DeltaDrawable.
         */
        dtCore::DeltaDrawable *GetClosestDeltaDrawable() {
            return mClosestDrawable.get();
        }

        /**
         * Gets the DeltaDrawable that is closest to the query's starting point.
         * @return A valid DeltaDrawable.
         */
        const dtCore::DeltaDrawable *GetClosestDeltaDrawable() const {
            return mClosestDrawable.get();
        }

        /**
         * Gets the line segment used for this intersection query.
         * @return
         */
        const osg::LineSegment *GetLineSegment() const {
            return mLineSegment.get();
        }

        /**
         * Gives access to the underlying intersect visitor.  This is useful when a more
         * detailed description of the intersection tests are required.
         * @return
         */
        osgUtil::IntersectVisitor &GetIntersectVisitor()
        {
            return mIntersectVisitor;
        }

        /**
         * Finds the DeltaDrawable that contains the given geometry node.
         * @param geode The node to search for.
         * @return A valid DeltaDrawable if one was found or NULL otherwise.
         */
        dtCore::DeltaDrawable *MapNodePathToDrawable(osg::NodePath &geode);

    private:
        osg::Vec3 mStart;
        osg::Vec3 mDirection;
        float mLineLength;
        bool mUpdateLineSegment;

        osg::ref_ptr<dtCore::Scene> mScene;
        osg::ref_ptr<osg::LineSegment> mLineSegment;
        osg::ref_ptr<dtCore::DeltaDrawable> mSceneRoot;
        osg::ref_ptr<dtCore::DeltaDrawable> mClosestDrawable;
        osgUtil::IntersectVisitor mIntersectVisitor;

        void CalcLineSegment();
    };

}

#endif

/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Chris Rodgers
 */

#ifndef __DELTA_ANIMCLIPPATH_H__
#define __DELTA_ANIMCLIPPATH_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <osg/AnimationPath>
#include <osg/NodeVisitor>

namespace dtAnim
{
    /////////////////////////////////////////////////////////////////////////////////
    // ANIM CLIP CALLBACK
    /////////////////////////////////////////////////////////////////////////////////
    class AnimClipCallback : public osg::Referenced
    {
    public:
        virtual void onTimeEvent(double time) = 0;

        void operator() (double time)
        {
            onTimeEvent(time);
        }

    protected:
        virtual ~AnimClipCallback() {}
    };



    /////////////////////////////////////////////////////////////////////////////////
    // ANIM CLIP PATH
    /////////////////////////////////////////////////////////////////////////////////
    class DT_ANIM_EXPORT AnimClipPath : public osg::AnimationPath
    {
    public:

        typedef osg::AnimationPath::ControlPoint ControlPoint;

        AnimClipPath();

        AnimClipPath(osg::AnimationPath& other);

        int getNumFrames() const;

        /**
         * The overriden method that allows a section/clip of
         * an animation curve to be played instead of the whole
         * curve being played.
         */
        virtual bool getInterpolatedControlPoint(double time, ControlPoint& controlPoint) const;

        void setTimeOffset(double time);
        double getTimeOffset() const;

        void setBeginTime(double time);
        double getBeginTime() const;

        void setEndTime(double time);
        double getEndTime() const;

        double getClipTimeOffset(double time) const;

        void setLoopLimit(int loopLimit);
        int getLoopLimit() const;

        int getLoopCount() const;

        void reset();

    private:
        double _beginTime;
        mutable double _endTime;
        mutable double _timeOffset;
        mutable int _loopLimit;
        mutable int _loopCount;
    };



    ////////////////////////////////////////////////////////////////////////////////
    // ANIM CALLBACK VISITOR
    ////////////////////////////////////////////////////////////////////////////////
    class DT_ANIM_EXPORT AnimCallbackVisitor : public osg::NodeVisitor
    {
    public:
        typedef std::vector<dtCore::RefPtr<osg::Node> > AnimNodeVector;
        typedef std::vector<dtCore::RefPtr<osg::AnimationPathCallback> > AnimCallbackVector;

        AnimCallbackVisitor();

        virtual void apply(osg::Node& node);

        void SetPaused(bool paused);

        void SetSpeed(double speed);

        void SetTimeOffset(double timeOffset);

        void SetLoopLimit(int loopLimit);

        void SetPlayMode(osg::AnimationPath::LoopMode curPlayMode);

        void ResetCallbacks();

        void Clear();

        AnimNodeVector& GetNodes() { return mAnimNodes; }
        const AnimNodeVector& GetNodes() const { return mAnimNodes; }

        AnimCallbackVector& GetAnimCallbacks() { return mAnimCallbacks; }
        const AnimCallbackVector& GetAnimCallbacks() const { return mAnimCallbacks; }

    private:
        AnimNodeVector mAnimNodes;
        AnimCallbackVector mAnimCallbacks;
    };
}

#endif

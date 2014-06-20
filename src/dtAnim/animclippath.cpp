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

#include <dtAnim/animclippath.h>



namespace dtAnim
{
    /////////////////////////////////////////////////////////////////////////////////
    // ANIM CLIP PATH
    /////////////////////////////////////////////////////////////////////////////////
    AnimClipPath::AnimClipPath()
            : _beginTime(0.0)
            , _endTime(0.0)
    {
    }

    AnimClipPath::AnimClipPath(osg::AnimationPath& other)
            : _beginTime(0.0)
            , _endTime(0.0)
    {
            setTimeControlPointMap(other.getTimeControlPointMap());
            setLoopMode(other.getLoopMode());
    }

    int AnimClipPath::getNumFrames() const
    {
        return (int)(_timeControlPointMap.size());
    }

    // Override the interpolation behavior so that only
    // a segment of an animation is played.
    bool AnimClipPath::getInterpolatedControlPoint(double time, ControlPoint& controlPoint) const
    {
            if (_timeControlPointMap.empty()) return false;

        double firstTime = getFirstTime();
        double lastTime = getLastTime();
        double period = getPeriod();
        bool periodChange = false;

        if (firstTime < _beginTime)
        {
            firstTime = _beginTime;
            periodChange = true;
        }
        if (lastTime > _endTime && _endTime != 0.0)
        {
            lastTime = _endTime;
            periodChange = true;
        }
        if (_endTime > lastTime || _endTime == 0.0)
        {
            _endTime = lastTime;
        }

        if(periodChange)
        {
            period = lastTime - firstTime;
        }
        
        switch(_loopMode)
        {
            case(osg::AnimationPath::SWING):
            {
                double modulated_time = (time - firstTime)/(period*2.0);
                double fraction_part = modulated_time - floor(modulated_time);
                if (fraction_part>0.5) fraction_part = 1.0-fraction_part;

                time = firstTime+(fraction_part*2.0) * period;
                break;
            }
            case(osg::AnimationPath::LOOP):
            {
                double modulated_time = (time - firstTime)/period;
                double fraction_part = modulated_time - floor(modulated_time);
                time = firstTime+fraction_part * period;
                break;
            }
            case(osg::AnimationPath::NO_LOOPING):
            {
                double newTime = time + firstTime;
                if (newTime > lastTime)
                {
                    newTime = lastTime;
                }
                time = newTime;
            }
            break;

            default:
                break;
        }



        TimeControlPointMap::const_iterator second = _timeControlPointMap.lower_bound(time);
        if (second==_timeControlPointMap.begin())
        {
            controlPoint = second->second;
        }
        else if (second!=_timeControlPointMap.end())
        {
            TimeControlPointMap::const_iterator first = second;
            --first;

            // we have both a lower bound and the next item.

            // delta_time = second.time - first.time
            double delta_time = second->first - first->first;

            if (delta_time==0.0)
                controlPoint = first->second;
            else
            {
                controlPoint.interpolate((time - first->first)/delta_time,
                                    first->second,
                                    second->second);
            }
        }
        else // (second==_timeControlPointMap.end())
        {
            controlPoint = _timeControlPointMap.rbegin()->second;
        }
        return true;
    }

    void AnimClipPath::setBeginTime(double tm)
    {
        _beginTime = tm;
    }
    double AnimClipPath::getBeginTime() const
    {
        return _beginTime;
    }

    void AnimClipPath::setEndTime(double tm)
    {
        _endTime = tm;
    }
    double AnimClipPath::getEndTime() const
    {
        return _endTime;
    }



    ////////////////////////////////////////////////////////////////////////////////
    // ANIM CALLBACK VISITOR
    ////////////////////////////////////////////////////////////////////////////////
    AnimCallbackVisitor::AnimCallbackVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
    }

    void AnimCallbackVisitor::apply(osg::Node& node)
    {
        if (node.getUpdateCallback() != NULL)
        {
            osg::AnimationPathCallback* animCallback
            = dynamic_cast<osg::AnimationPathCallback*>(node.getUpdateCallback());
            if (animCallback != NULL)
            {
            mAnimNodes.push_back(&node);
            mAnimCallbacks.push_back(animCallback);
            }
        }

        traverse(node);
    }

    void AnimCallbackVisitor::SetPaused(bool paused)
    {
        AnimCallbackVector::iterator iter = mAnimCallbacks.begin();
        AnimCallbackVector::iterator iterEnd = mAnimCallbacks.end();
        for ( ; iter != iterEnd; ++iter)
        {
            iter->get()->setPause(paused);
        }
    }

    void AnimCallbackVisitor::SetSpeed(double speed)
    {
        if (speed == 0.0)
        {
            return;
        }

        AnimCallbackVector::iterator iter = mAnimCallbacks.begin();
        AnimCallbackVector::iterator iterEnd = mAnimCallbacks.end();
        for ( ; iter != iterEnd; ++iter)
        {
            iter->get()->setTimeMultiplier(speed);
        }
    }

    void AnimCallbackVisitor::ResetCallbacks()
    {
        AnimCallbackVector::iterator iter = mAnimCallbacks.begin();
        AnimCallbackVector::iterator iterEnd = mAnimCallbacks.end();
        for ( ; iter != iterEnd; ++iter)
        {
            iter->get()->reset();
        }
    }

    void AnimCallbackVisitor::SetPlayMode(osg::AnimationPath::LoopMode curPlayMode)
    {
        AnimCallbackVector::iterator iter = mAnimCallbacks.begin();
        AnimCallbackVector::iterator iterEnd = mAnimCallbacks.end();
        for ( ; iter != iterEnd; ++iter)
        {
            iter->get()->getAnimationPath()->setLoopMode(curPlayMode);
        }
    }

    void AnimCallbackVisitor::Clear()
    {
        mAnimNodes.clear();
        mAnimCallbacks.clear();
    }
}

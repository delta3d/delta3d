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
            , _timeOffset(0.0)
            , _loopLimit(0)
            , _loopCount(0)
    {
    }

    AnimClipPath::AnimClipPath(osg::AnimationPath& other)
            : _beginTime(0.0)
            , _endTime(0.0)
            , _timeOffset(0.0)
            , _loopLimit(0)
            , _loopCount(0)
    {
        AnimClipPath* clipPath = dynamic_cast<AnimClipPath*>(&other);
        if (clipPath != NULL)
        {
            _beginTime = clipPath->_beginTime;
            _endTime = clipPath->_endTime;
            _loopLimit = clipPath->_loopLimit;
            _timeOffset = clipPath->_timeOffset;
        }

        setTimeControlPointMap(other.getTimeControlPointMap());
        setLoopMode(other.getLoopMode());
    }

    int AnimClipPath::getNumFrames() const
    {
        return (int)(_timeControlPointMap.size());
    }

    void AnimClipPath::setTimeOffset(double timeOffset)
    {
        _timeOffset = timeOffset;
    }
    
    double AnimClipPath::getTimeOffset() const
    {
        return _timeOffset;
    }

    void AnimClipPath::setBeginTime(double time)
    {
        _beginTime = time;
    }
    double AnimClipPath::getBeginTime() const
    {
        return _beginTime;
    }

    void AnimClipPath::setEndTime(double time)
    {
        _endTime = time;
    }
    double AnimClipPath::getEndTime() const
    {
        return _endTime;
    }

    double AnimClipPath::getClipTimeOffset(double time) const
    {
        double relativeTimeOffset = time - _beginTime;
        
        if (relativeTimeOffset < 0.0)
        {
            relativeTimeOffset = 0.0;
        }
        else if (relativeTimeOffset > _endTime)
        {
            relativeTimeOffset = _endTime;
        }

        return relativeTimeOffset;
    }

    void AnimClipPath::setLoopLimit(int loopLimit)
    {
        _loopLimit = loopLimit;
    }
    int AnimClipPath::getLoopLimit() const
    {
        return _loopLimit;
    }

    int AnimClipPath::getLoopCount() const
    {
        return _loopCount;
    }

    // Override the interpolation behavior so that only
    // a segment of an animation is played.
    bool AnimClipPath::getInterpolatedControlPoint(double time, ControlPoint& controlPoint) const
    {
        if (_timeControlPointMap.empty()) return false;

        // Determine if animation is complete.
        bool stopLooping = _loopLimit > 0 && _loopCount >= _loopLimit;

        double firstTime = getFirstTime();
        double lastTime = getLastTime();

        if (firstTime < _beginTime)
        {
            firstTime = _beginTime;
        }

        if (lastTime > _endTime && _endTime != 0.0)
        {
            lastTime = _endTime;
        }
        if (_endTime > lastTime || _endTime == 0.0)
        {
            _endTime = lastTime;
        }

        double period = lastTime - firstTime;
        
        int prevLoopCount = _loopCount;
        switch(_loopMode)
        {
            case(osg::AnimationPath::SWING):
            {
                if (stopLooping)
                {
                   time = firstTime;
                }
                else
                {
                   time += firstTime + _timeOffset;
                   double modulated_time = (time - firstTime)/(period*2.0);
                   _loopCount = int(floor(modulated_time));
                   double fraction_part = modulated_time - _loopCount;
                   if (fraction_part>0.5) fraction_part = 1.0-fraction_part;

                   time = firstTime+(fraction_part*2.0) * period;
                }

                break;
            }
            case(osg::AnimationPath::LOOP):
            {
                if (stopLooping)
                {
                   time = lastTime;
                }
                else
                {
                   time += firstTime + _timeOffset;
                   double modulated_time = (time - firstTime)/period;
                   _loopCount = int(floor(modulated_time));
                   double fraction_part = modulated_time - _loopCount;

                   time = firstTime+fraction_part * period;
                }

                break;
            }
            case(osg::AnimationPath::NO_LOOPING):
            {
                // Determin if the time has already passed for the clip.
                if (time > lastTime)
                {
                    return false;
                }

                time += firstTime + _timeOffset;
                if (time > lastTime)
                {
                    time = lastTime;
                }
            }
            break;

            default:
                break;
        }

        // Was a loop completed this pass?
        if (_loopCount >= 0 && prevLoopCount != _loopCount)
        {
            // TODO: Signal that a loop is complete.
        }

        TimeControlPointMap::const_iterator second = _timeControlPointMap.lower_bound(time);
        // Before the begin time?
        if (second==_timeControlPointMap.begin())
        {
            controlPoint = second->second;
        }
        // In the middile?
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
        // At or past the end time?
        else // (second==_timeControlPointMap.end())
        {
            controlPoint = _timeControlPointMap.rbegin()->second;
        }
        return true;
    }

    void AnimClipPath::reset()
    {
       _loopCount = 0;
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

    void AnimCallbackVisitor::SetTimeOffset(double timeOffset)
    {
        AnimCallbackVector::iterator iter = mAnimCallbacks.begin();
        AnimCallbackVector::iterator iterEnd = mAnimCallbacks.end();
        for ( ; iter != iterEnd; ++iter)
        {
            AnimClipPath* path = dynamic_cast<AnimClipPath*>(iter->get()->getAnimationPath());
            if (path != NULL)
            {
                path->setTimeOffset(timeOffset);
            }
        }
    }
    
    void AnimCallbackVisitor::SetLoopLimit(int loopLimit)
    {
        AnimCallbackVector::iterator iter = mAnimCallbacks.begin();
        AnimCallbackVector::iterator iterEnd = mAnimCallbacks.end();
        for ( ; iter != iterEnd; ++iter)
        {
            AnimClipPath* path = dynamic_cast<AnimClipPath*>(iter->get()->getAnimationPath());
            if (path != NULL)
            {
                path->setLoopLimit(loopLimit);
            }
        }
    }

    void AnimCallbackVisitor::ResetCallbacks()
    {
        AnimCallbackVector::iterator iter = mAnimCallbacks.begin();
        AnimCallbackVector::iterator iterEnd = mAnimCallbacks.end();
        for ( ; iter != iterEnd; ++iter)
        {
            iter->get()->reset();
            
            AnimClipPath* path = dynamic_cast<AnimClipPath*>(iter->get()->getAnimationPath());
            if (path != NULL)
            {
               path->reset();
            }
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

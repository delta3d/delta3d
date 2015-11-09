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
 * Bradley Anderegg 04/20/2007
 */

#ifndef __DELTA_ANIMATABLE_H__
#define __DELTA_ANIMATABLE_H__

#include <dtAnim/export.h>

#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>
#include <dtUtil/refstring.h>

#include <string>
#include <vector>
#include <map>

namespace dtAnim
{
   class BaseModelWrapper;
   class Animatable;
   class AnimationInterface;

   typedef dtUtil::Functor<void, TYPELIST_1(const dtAnim::Animatable&) > AnimationCallback;

   /**
    * This class is used to specify the base class of an object which has semantics for
    * animating.
    */
   class DT_ANIM_EXPORT Animatable: public osg::Referenced
   {
      public:
         static const float INFINITE_TIME;

         typedef std::multimap<std::string, float> TimeEventMap;
         typedef std::vector<float> TimeOffsetArray;
         typedef std::vector<std::string> EventNameArray;

         Animatable();

         Animatable(const Animatable& pAnim);
         Animatable& operator=(const Animatable& pAnim);

      protected:
         virtual ~Animatable();

      public:

         /**
          * The ID of the internal animation object
          * @return the animation ID
          */
         int GetID() const;

         /**
          * Set the ID of the animation that this wrapper refers to.
          * NOTE: This does not change the id of the referenced animation.
          *    and should only be set if the animation ID has changed
          *    outside the control of this object.
          */
         void SetID(int id);

         /**
          * Set the time that the animatable is inserted into a parent sequence,
          * usually the root sequence.
          * @param t Time relative to the parent sequence this animatable is add to.
          */
         void SetInsertTime(double t);
         double GetInsertTime() const;

         /**
          * The start time is the time in seconds this animation will start playing
          * after it was added to an AnimationSequence.
          */
         float GetStartTime() const;

         /**
          * The start delay specifies how long in seconds this animation will delay once
          * its parent has started playing.
          */
         void SetStartDelay(float t);
         float GetStartDelay() const;

         /**
          * The end time is the time in seconds that this animation will start
          * fading out relative to when it was added to an AnimationSequence.
          */
         float GetEndTime() const;

         /**
          * The elapsed time is the time in seconds since this animation has been
          * added to the play list.
          */
         double GetElapsedTime() const;

         /**
          * The FadeIn time, is the amount of time takes for an animation to blend
          * linearally from a weight of 0 to the weight specified by BaseWeight
          * after the animation starts playing specified by StartTime.
          */
         float GetFadeIn() const;
         void SetFadeIn(float f);

         /**
          * The FadeOut time is the amount of time it will take for an animation to
          * blend linearly from its BaseWeight to 0 after the EndTime.
          */
         float GetFadeOut() const;
         void SetFadeOut(float f);

         /**
          * The BaseWeight specifies the weight of this animation without
          * being affected by blending.
          */
         float GetBaseWeight() const;
         void SetBaseWeight(float f);

         /**
          * The CurrentWeight is the weight of this animation in the current frame
          * this is calculated from the BaseWeight, linear blending from fades, and the parent weight
          */
         float GetCurrentWeight() const;

         /**
          * An animation is active if it is currently playing.
          */
         bool IsActive() const;

         /**
          * The speed of an animation is the percentage relative to the actual speed
          * of playback.  It defaults to 1.0, a speed of 2.0 would play twice as fast.
          */
         float GetSpeed() const;
         void SetSpeed(float speed);

         /**
          * Method that determines if the animation will eventually con to an end.
          * NOTE: This method has a cost in that it traverses some or all of its
          *       potential child animations to determine if the overall animation
          *       has a definite end.
          * @return TRUE if no part of the animation or its potential children
          *         endlessly loop.
          */
         virtual bool HasDefiniteEnd() const;

         /**
          * This flag specifies whether or not this animation has stopped playing
          */
         bool ShouldPrune() const;


         /**
          * This virtual function is called before this animation is removed from
          * the system
          */
         virtual void Prune() = 0;

         /**
          * This function is used to copy Animatables
          */
         virtual dtCore::RefPtr<Animatable> Clone(BaseModelWrapper* modelWrapper) const = 0;


         /**
          * The virtual update, should be called every frame
          *
          * @param delta time
          */
         virtual void Update(float dt) = 0;

         /**
          * ForceFadeOut will ignore the EndTime and automatically fade out
          * this animation over the time specified.
          *
          * @param the time to fade out over
          */
         virtual void ForceFadeOut(float time) = 0;

         /**
          * @return the name of this animation
          */
         const std::string& GetName() const;

         /**
          * @param the name to set this animation to
          */
         void SetName(const std::string&);


         /**
          * These functions are only to be used by AnimationController
          * on Update() and Recalculate
          */
         void SetStartTime(float t);
         void SetCurrentWeight(float weight);
         void SetElapsedTime(double t);
         virtual void Recalculate() = 0;

         /**
          * Method to determine the actual length of the animation.
          * @return Length of the animation measured in seconds. The value
          *         that is returned may be a stored value or one that is
          *         calculated per method call.
          */
         virtual float CalculateDuration() const = 0;

         /**
          * Get the elapsed time relative to the scope of the animation.
          * In the case of a looping animation, the value returned will
          * cycle between 0 to its duration for one iteration. However,
          * if no specific duration is available (such as for a sequence),
          * the elapsed time may continue indefinitely.
          * @return A value between 0 and duration of one iteration of
          *         of the animation; 0 is at the beginning and the
          *         duration is the end time. By default this returns
          *         the elapsed time offset by its start time.
          */
         float GetRelativeElapsedTimeInAnimationScope() const;

         /**
          * Convenience method for turning an absolute time to a time
          * relative to the animation's scope.
          * @param timeToConvert Absolute time that should be mapped to this
          *        animation's time line scope.
          * @return A value between 0 and duration of one iteration of
          *         of the animation; 0 is at the beginning and the
          *         duration is the end time.
          */
         virtual float ConvertToRelativeTimeInAnimationScope(double timeToConvert) const;

         /**
          * Set the functor to be called when the animation ends.
          * @param callback The functor to be called when this animation ends.
          *        The Animatable will be passed into the callback so that
          *        the callee can determine what animation has ended.
          */
         void SetEndCallback(AnimationCallback callback);

         /**
          * Convenience method.
          * Set/get the name of the event to be fired when the animation starts.
          * This is the same as setting the event name for time 0.
          * @param eventName Name of the event that ought to be fired for the start time.
          * @return TRUE if the event was successfully added.
          */
         bool AddEventOnStart(const std::string& eventName);

         /**
          * Convenience method.
          * Set/get the name of the event to be fired when the animation completes.
          * This is the same as setting the event name for the end time.
          * NOTE: Call this only after this object is completely valid, so that it
          * can calculate the appropriate end time.
          * @param eventName Name of the event that ought to be fired for the end time.
          * @return TRUE if the event was successfully added.
          */
         bool AddEventOnEnd(const std::string& eventName);

         /**
          * Add an event to be triggered at a particular time relative to the animation.
          * @param eventName Event name to be added for the specified time offset.
          * @param timeOffset Time relative to the animation at which the specified
          *        event ought to be fired.
          * @return TRUE if the event name was added successfully.
          */
         bool AddEventOnTime(const std::string& eventName, float timeOffset);

         /**
          * Remove an event that was set to be triggered at a particular time.
          * @param eventName Event name to be removed for the specified time offset.
          * @param timeOffset Time relative to the animation where the event may have been added.
          * @return TRUE if the event name was found and removed successfully.
          */
         bool RemoveEventOnTime(const std::string& eventName, float timeOffset);

         /**
          * Remove an event name if it exists.
          * @param eventName The event to be removed.
          * @param outArray Time offsets to which the event was assigned.
          * @return Count of time offsets found for the specified event name.
          */
         unsigned RemoveEvent(const std::string& eventName, TimeOffsetArray* outArray);

         /**
          * Determine if an event exists at the specified time offset.
          * @param eventName Name of the event to find.
          * @param timeOffset Time offset into the animation where the event may be.
          * @return TRUE if the event is found.
          */
         bool HasEventOnTime(const std::string& eventName, float timeOffset) const;

         /**
          * Determine if an event exists at the specified time range.
          * @param eventName Name of the event to find.
          * @param startTime Start of the time range.
          * @param endTime End of the time range.
          * @return TRUE if the event is found.
          */
         bool HasEventForTimeRange(const std::string& eventName,
            float startTime, float endTime) const;

         /**
          * Determine if an event name was registered.
          * @param eventName The event to be found.
          * @param outArray Time offsets to which the event was assigned.
          * @return Count of time offsets found for the specified event name.
          */
         unsigned GetEventTimeOffsets(const std::string& eventName, TimeOffsetArray& outArray) const;

         /**
          * Acquire event names that should be fired within a specific time range.
          * @param startTime Start of the time range.
          * @param endTime End of the time range.
          * @param outEventArray List to gather the event names that ought to be
          *        fired for the specified time range.
          * @return Count of how many were actually gathered.
          */
         unsigned GetEventsForTimeRange(float startTime, float endTime,
            EventNameArray& outEventArray) const;

         /**
          * Acquire event names that should be fired at a specific time.
          * @param timeOffset Time where an event is expected to be. 
          * @param outEventArray List to gather the event names that ought to be
          *        fired for the specified time.
          * @return Count of how many were actually gathered.
          */
         unsigned GetEvents(float timeOffset,
            EventNameArray& outEventArray) const;

         /**
          * Remove events for a specified time range.
          * @param startTime Start of the time range.
          * @param endTime End of the time range.
          * @param outEventArray List to gather the event names that will be removed.
          * @return Count of how many were actually removed.
          */
         unsigned RemoveEventsForTimeRange(float startTime, float endTime,
            EventNameArray* outEventArray = NULL);

         /**
          * Remove an event that was set to be triggered at a particular time.
          * @param eventName Event name to be removed for the specified time range.
          * @param startTime Start of the time range.
          * @param endTime End of the time range.
          * @return TRUE if the event name was found and removed successfully.
          */
         bool RemoveEventForTimeRange(const std::string& eventName,
            float startTime, float endTime);

         /**
          * Remove all events.
          * @return Count of how many were actually removed.
          */
         unsigned ClearEvents();

         /**
          * Convenience accessors for the underlying time-to-event-name map.
          */
         const TimeEventMap& GetTimeEventMap() const;

      protected:

         /**
          * When this flag is set the parent sequence will call Prune() at the end of its update
          * and then delete this animation.
          */
         void SetPrune(bool b);

         void SetEndTime(float t);
         void SetActive(bool b);

      private:
         int mID;

         // user editable fields are: fade in, fade out, base weight, and speed

         float mSpeed;
         double mInsertTime;
         float mStartTime;
         float mStartDelay;
         float mEndTime;
         float mFadeIn;
         float mFadeOut;
         double mElapsedTime;
         float mBaseWeight;
         float mCurrentWeight;

         bool mActive;
         bool mShouldPrune;

         std::string mName;

         // NOTE: This event map should only be valid from the original instance of
         // this class and should not be cloned for reasons of memory and performance.
         TimeEventMap mTimeEventMap;
         
         AnimationCallback mEndCallback;

   };

} // namespace dtAnim

#endif // __DELTA_ANIMATABLE_H__


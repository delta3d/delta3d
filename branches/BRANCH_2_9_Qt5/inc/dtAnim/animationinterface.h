/*
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2007 MOVES Institute 
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
*/

#ifndef __DELTA_ANIMATIONINTERFACE_H__
#define __DELTA_ANIMATIONINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/referencedinterface.h>
// OSG
#include <osg/Vec3>
#include <osg/Quat>
// STL
#include <string>
#include <vector>



namespace dtAnim
{
   ////////////////////////////////////////////////////////////////////////////////
   // FORWARD DECLARATIONS
   ////////////////////////////////////////////////////////////////////////////////
   class BoneInterface;



   ////////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   ////////////////////////////////////////////////////////////////////////////////
   typedef unsigned int Keyframe;
   typedef osg::Vec3 Location;
   typedef osg::Quat Rotation;
   


   ////////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT PlayModeEnum : public dtUtil::Enumeration
   {
         DECLARE_ENUM(PlayModeEnum);
      public:
         typedef dtUtil::Enumeration BaseClass;

         static PlayModeEnum NONE;
         static PlayModeEnum ONCE;
         static PlayModeEnum LOOP;
         static PlayModeEnum SWING;
         static PlayModeEnum POSE;

      private:
         PlayModeEnum(const std::string& name) 
            : BaseClass(name)
         {}
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT AnimationStateEnum : public dtUtil::Enumeration
   {
         DECLARE_ENUM(AnimationStateEnum);
      public:
         typedef dtUtil::Enumeration BaseClass;

         static AnimationStateEnum NONE;
         static AnimationStateEnum FADE_IN;
         static AnimationStateEnum STEADY;
         static AnimationStateEnum FADE_OUT;
         static AnimationStateEnum STOPPED;

      private:
         AnimationStateEnum(const std::string& name)
            : BaseClass(name)
         {}
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT TrackInterface : virtual public dtUtil::ReferencedInterface
   {
   public:
      /**
       * Returns the bone that is associated with this track.
       */
      virtual dtAnim::BoneInterface* GetBone() const = 0;

      /**
       * Returns the location and rotaion information of a bone at a specified time.
       * @param time The time in seconds of the animation between 0.0 and animation duration.
       * @param outLocation Variable to receive the translation information.
       * @param outRotation Variable to receive the rotational information.
       * @return TRUE if 
       */
      virtual bool GetTransformAtTime(float time, dtAnim::Location& outLocation, dtAnim::Rotation& outRotation) const = 0;

   protected:
      virtual ~TrackInterface() {}
   };

   typedef std::vector<dtCore::RefPtr<dtAnim::TrackInterface> > TrackArray;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT AnimationInterface : virtual public dtUtil::ReferencedInterface
   {
   public:
      /**
       * Returns a number that represents this object within the context of a model.
       * This usually is an index within a collection.
       * Use of this as a key is not recommended.
       */
      virtual int GetID() const = 0;

      /**
       * Name identifier for this object that should be unique
       * within the context of a model.
       */
      virtual void SetName(const std::string& name) = 0;
      virtual const std::string& GetName() const = 0;

      virtual void SetPlayMode(const dtAnim::PlayModeEnum& playMode) = 0;
      virtual const dtAnim::PlayModeEnum& GetPlayMode() const = 0;

      virtual const dtAnim::AnimationStateEnum& GetState() const = 0;
      
      /**
       * Peform a one time animation.
       * @param delayIn : how long it takes to fade in this animation to full strength (seconds)
       * @param delayOut: how long it takes to fade out this animation (seconds)
       * @param weightTgt : the strength of this animation
       * @param autoLock : true prevents the action from being reset and removed on the last
       *                   key frame
       * @return true if successful, false if an error happened.
       */
      virtual bool PlayAction(float delayIn, float delayOut,
         float weight = 1.0f, bool autoLock = false) = 0;
      
      /**
       * Play an animation that is meant to repeat indefinitely.
       * @param weight : the strength of this animation in relation to the other
       *                 animations already being blended.
       * @param delay : how long it takes for this animation to become full strength (seconds)
       * @return true if successful, false if an error happened.
       */
      virtual bool PlayCycle(float weight, float delay) = 0;
      
      /**
       * Remove an existing one-time animation from the animation mixer.
       * @return true if successful.
       */
      virtual bool ClearAction() = 0;
      
      /**
       * @param delay : how long it takes to fade this animation out (seconds)
       * @return true if successful.
       */
      virtual bool ClearCycle(float delay) = 0;
      
      /**
       * Remove the animation, but determine internally if the animation is a cycle or action.
       * @param delay : how long it takes to fade this animation out (seconds)
       * @return true if successful.
       */
      virtual bool Clear(float delay) = 0;

      /**
       * Returns the duration of the animation in seconds.
       */
      virtual float GetDuration() const = 0;

      /**
       * Returns the current weight factor of the instanced animation.
       * @return Value between 0.0 to 1.0.
       */
      virtual float GetWeight() const = 0;

      /**
       * Returns the relative time in seconds of the instanced animation.
       * @return Time between 0.0 to duration in seconds.
       */
      virtual float GetTime() const = 0;
      
      /**
       * Returns the number of keyframes for all tracks contained in this animation.
       */
      virtual int GetKeyframeCount() const = 0;

      /**
       * Returns the number of keyframes associated with the specified bone.
       */
      virtual int GetKeyframeCountForBone(const dtAnim::BoneInterface& bone) const = 0;

      /**
       * Returns the number of tracks that this animation controls.
       */
      virtual int GetTrackCount() const = 0;

      /**
       * Returns a collection of objects that represent all tracks associated with this animation.
       * @param tracks Container to receive all track objects, sorted.
       * @return The number of objects added, which should be the same as the number of tracks contained by this animation.
       */
      virtual int GetTracks(dtAnim::TrackArray& outTracks) = 0;

      /**
       * Returns true if a specified bone is controlled by this animation.
       * @param bone The bone in question that may be affected by this animation.
       * @return TRUE if there is a track associated with the specified bone.
       */
      virtual bool HasTrackForBone(const dtAnim::BoneInterface& bone) const = 0;
      
      virtual osg::Quat GetKeyframeQuat(const dtAnim::BoneInterface& bone, dtAnim::Keyframe keyframe) const = 0;

   protected:
      virtual ~AnimationInterface() {}
   };

   typedef std::vector<dtCore::RefPtr<dtAnim::AnimationInterface> > AnimationArray;
}

#endif

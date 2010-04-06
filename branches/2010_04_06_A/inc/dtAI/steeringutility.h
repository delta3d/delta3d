/* -*-c++-*-
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
 * Taken from the source code for CTFGame off of the AI Game Programming Wisdom 4 chapter
 *    "Particle Filter Methods for More Realistic Hiding and Seeking" (Christian Darken, Brad Anderegg)
 *
 * Copyright (C) 2007, Bradley Anderegg
 */

#ifndef DELTA_STEERINGUTILITY
#define DELTA_STEERINGUTILITY

#include <osg/Referenced>
#include <osg/Vec3>

namespace dtAI
{

   struct SteeringOutput
   {
      osg::Vec3 mLinearVelocity;
      osg::Vec3 mAngularVelocity;
   };

   struct Kinematic
   {
      osg::Vec3 mTranslation;
      osg::Vec3 mRotation;
      osg::Vec3 mLinearVelocity;
      osg::Vec3 mAngularVelocity;
   };

   class KinematicGoal
   {
   public:
      KinematicGoal()
         : mGoal()
         , mHasPosition(false)
         , mHasRotation(false)
         , mHasVelocity(false)
         , mHasAngularVel(false)
      {

      }

      const Kinematic& GetGoal() const
      {
         return mGoal;
      }

      bool HasPosition() const
      {
         return mHasPosition;
      }

      void SetPosition(const osg::Vec3& pos)
      {
         mHasPosition = true;
         mGoal.mTranslation = pos;
      }

      const osg::Vec3& GetPosition() const
      {
         return mGoal.mTranslation;
      }

      void RemovePosition()
      {
         mHasPosition = false;
      }

      bool HasRotation() const
      {
         return mHasRotation;
      }

      void SetRotation(const osg::Vec3& rot)
      {
         mHasRotation = true;
         mGoal.mRotation = rot;
      }

      const osg::Vec3& GetRotation() const
      {
         return mGoal.mRotation;
      }

      void RemoveRotation()
      {
         mHasRotation = false;
      }

      bool HasLinearVelocity() const
      {
         return mHasVelocity;
      }

      void SetLinearVelocity(const osg::Vec3& vel)
      {
         mHasVelocity = true;
         mGoal.mLinearVelocity = vel;
      }

      const osg::Vec3& GetLinearVelocity() const
      {
         return mGoal.mLinearVelocity;
      }

      void RemoveLinearVelocity()
      {
         mHasVelocity = false;
      }

      bool HasAngularVelocity() const
      {
         return mHasAngularVel;
      }

      void SetAngularVelocity(const osg::Vec3& vel)
      {
         mHasAngularVel = true;
         mGoal.mAngularVelocity = vel;
      }

      const osg::Vec3& GetAngularVelocity() const
      {
         return mGoal.mAngularVelocity;
      }

      void RemoveAngularVelocity()
      {
         mHasAngularVel = false;
      }

   private:
      Kinematic mGoal;

      bool mHasPosition;
      bool mHasRotation;
      bool mHasVelocity;
      bool mHasAngularVel;
   };

} // namespace dtAI

#endif //DELTA_STEERINGUTILITY

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
 */

#ifndef DELTA_TRIGGER
#define DELTA_TRIGGER

#include <dtCore/deltadrawable.h>
#include <dtCore/refptr.h>
#include <dtABC/action.h>
#include <dtABC/export.h>

#include <osg/Node>

namespace dtABC
{
   class DT_ABC_EXPORT Trigger : public dtCore::DeltaDrawable
   {
      DECLARE_MANAGEMENT_LAYER(Trigger)

   public:
      Trigger(const std::string& name = "Trigger");
   protected:
      virtual ~Trigger();
   public:

      virtual void OnSystem(const dtUtil::RefString& str, double, double)
;

      void SetEnabled(bool enabled) { mEnabled = enabled; }
      bool GetEnabled() const { return mEnabled; }

      void SetTimeDelay(double timeDelay) { mTimeDelay = timeDelay; mTimeLeft = mTimeDelay; }
      double GetTimeDelay() const { return mTimeDelay; }

      double GetTimeLeft() const { return mTimeLeft; }

      void Fire();

      void SetAction(Action* action) { mActionToFire = action; }

      Action* GetAction() { return mActionToFire.get(); }
      const Action* GetAction() const { return mActionToFire.get(); }

      /**
       * Gets the number of times this trigger will activate before
       * becoming disabled. Negative value indicates an infinite
       * number of activations.
       *
       * @return The number of times this trigger will activate before
       * disabling.
       */
      int GetTimesActive() const { return mTimesActive; }

      /**
       * Sets the number of times this trigger will activate before
       * becoming disabled.
       *
       * @param pTimesActive The number of times this trigger will activate
       * before becoming disabled. For an infinite number of times, set
       * the value to be negative.
       */
      void SetTimesActive(int pTimesActive) { mTimesActive = pTimesActive; }


      //this is required by deltadrawable
      osg::Node* GetOSGNode() { return mNode.get(); }
      const osg::Node* GetOSGNode() const { return mNode.get(); }


   private:

      void Update(double time);

   private:

      bool                      mEnabled;
      double                    mTimeDelay;
      double                    mTimeLeft;
      int                       mTimesActive, mTimesTriggered;
      dtCore::RefPtr<Action>    mActionToFire;
      dtCore::RefPtr<osg::Node> mNode;
   };
} // namespace dtABC

#endif // DELTA_TRIGGER

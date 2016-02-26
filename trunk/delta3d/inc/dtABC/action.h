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
 * Bradley Anderegg
 */

#ifndef __ACTION_H__
#define __ACTION_H__

////////////////////////////////////////////////////////////////////////////////

#include <dtCore/refptr.h>
#include <dtCore/deltadrawable.h>
#include <osg/Node>

#include "export.h"

////////////////////////////////////////////////////////////////////////////////

namespace dtABC
{
   /**
    * An Action is something which happens over time
    */
class DT_ABC_EXPORT Action : public dtCore::DeltaDrawable
{
public:
   Action();

   /**
    *  Filters out preframe events
    */
   void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

   void Update(double dt);

   virtual bool CanStart() const { return true; }

   bool Start();
   void Pause();
   void UnPause();

   bool GetIsRunning() const {return mIsRunning;}

   void SetTimeStep(float dt) { mTimeStep = dt; }
   float GetTimeStep() { return mTimeStep; }

   /**
    *  Determines if OnNextStep should be called only once per frame or
    *  possibly called multiple times with a time step over accumulated time.
    */
   void SetTickOncePerFrame(bool tickOncePerFrame) { mTickOncePerFrame = tickOncePerFrame; }
   bool GetTickOncePerFrame() const { return mTickOncePerFrame; }

   osg::Node* GetOSGNode(){return mNode.get();}
   const osg::Node* GetOSGNode()const{return mNode.get();}

protected:
   /*virtual*/ ~Action();

   /**
    * @brief overload this function to make changes based
    *  on the time step
    * @return bool indicating whether or not to continue
    *  returning false will stop the update function calling this
    */
   virtual bool OnNextStep() = 0;

   /**
    * @brief called on Start()
    */
   virtual void OnStart() = 0;

   /**
    * @brief called on Pause()
    */
   virtual void OnPause() = 0;

   /**
    * @brief called on UnPause()
    */
   virtual void OnUnPause() = 0;

protected:
   // variables
   float mTimeStep;
   float mTotalTime;
   float mAccumTime;
   bool mIsRunning;
   bool mTickOncePerFrame;

   dtCore::RefPtr<osg::Node> mNode;
};

} // namespace dtABC

////////////////////////////////////////////////////////////////////////////////

#endif // __ACTION_H__

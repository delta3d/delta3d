/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011 MOVES Institute
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

#ifndef axisobserver_h__
#define axisobserver_h__

// axisobserver.h: Declaration of the AxisObserver class.
//
//////////////////////////////////////////////////////////////////////

namespace dtCore
{
   class Axis;

   /// An interface for objects interested in observing changes to axes.
   class DT_CORE_EXPORT AxisObserver
   {
   public:

      virtual ~AxisObserver() {}

      /**
       * Called when an axis' state has changed.
       *
       * @param axis the changed axis
       * @param oldState the old state of the axis
       * @param newState the new state of the axis
       * @param delta a delta value indicating stateless motion
       */
      virtual void OnAxisStateChanged(const Axis* axis, double oldState, double newState, double delta)=0;
   };
}

#endif // axisobserver_h__

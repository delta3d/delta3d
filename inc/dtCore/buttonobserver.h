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

#ifndef buttonobserver_h__
#define buttonobserver_h__

// buttonobserver.h: Declaration of the ButtonObserver class.
//
//////////////////////////////////////////////////////////////////////

namespace dtCore
{
   class Button;

   /// An interface for objects interested in observing button state changes.
   class DT_CORE_EXPORT ButtonObserver
   {
   public:

      virtual ~ButtonObserver() {}

      /// Called when a button's state has changed.
      /// @param button the origin of the event
      /// @param oldState the old state of the button
      /// @param newState the new state of the button
      virtual void OnButtonStateChanged(const Button* button, bool oldState, bool newState)=0;
   };
}

#endif // buttonobserver_h__

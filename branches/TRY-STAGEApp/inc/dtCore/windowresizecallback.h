/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2009 MOVES Institute 
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

#ifndef windowresizecallback_h__
#define windowresizecallback_h__

#include <osg/Referenced>

namespace dtCore
{
   /** Interface class used as a callback to be notified when the DeltaWin gets resized
    *  @see DeltaWin::AddResizeCallback()
    */
   class WindowResizeCallback : public osg::Referenced
   {
   public:
      WindowResizeCallback() {};

      virtual void operator () (const dtCore::DeltaWin& win, int x, int y, int width, int height) = 0;

   protected:
      virtual ~WindowResizeCallback() {};
   };
}
#endif // windowresizecallback_h__

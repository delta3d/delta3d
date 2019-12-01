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

#ifndef __DELTA_BREAK_OVVERIDE__
#define __DELTA_BREAK_OVVERIDE__

namespace dtUtil
{
   /**
    * This macro should be used when deprecating (or removing) virtual functions.
    * The effect is to force a compile error on client code that may be overriding
    * the virtual function. Here is an example.
      @code
         virtual void GetTransform(Transform& xform, CoordSysEnum cs = ABS_CS) const;
         //virtual void GetTransform(Transform* xform, CoordSysEnum cs = ABS_CS) const;
         private:
         BREAK_OVERRIDE(GetTransform(const Transform*, CoordSysEnum))
      @endcode
    */
   struct BreakOverride
   {
   };
}

#define BREAK_OVERRIDE(f)\
virtual dtUtil::BreakOverride f\
{\
   return dtUtil::BreakOverride();\
}

#endif // __DELTA_BREAK_OVVERIDE__

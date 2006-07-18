/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2006 MOVES Institute
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
* @author Erik Johnson 06/06/2006
*
* Bits is a derivation from the Bits.h file of the CADKit project, Usul:
* Perry L. Miller IV
* http://cadkit.sourceforge.net/
*/

#ifndef __DELTA_BITS_H__
#define __DELTA_BITS_H__


namespace dtUtil
{

   /**
    * @namespace dtUtil::Bits
    *
    * Contains bit-wise operation functionality which makes using bits
    * a little bit easier.
    */
   namespace Bits
   {

      //
      //  See if the "bits" are in "number".
      //
      template < class N, class B > inline bool Has ( N number, B bits )
      {
         return ( ( number & bits ) == ( static_cast<N>(bits) ) );
      }

      //
      //  Add the "bits" to "number".
      //

      template < class N, class B > inline N Add ( N number, B bits )
      {
         return ( number | bits );
      }

      //
      //  Remove the "bits" from "number".
      //

      template < class N, class B > inline N Remove ( N number, B bits )
      {
         return ( ( number & bits ) ? ( number ^ bits ) : number );
      }

      //
      //  Toggle the "bits" in "number".
      //

      template < class N, class B > inline N Toggle ( N number, B bits )
      {
         return ( number ^ bits );
      }
   } //namespace Bits
}//namespace dtUtil

#endif // __DELTA_BITS_H__

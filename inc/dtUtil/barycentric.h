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
* John Grant 04/01/2007
*/

#ifndef _pd_barycentric_h_
#define _pd_barycentric_h_

namespace dtUtil
{
   /////////////////////////////////////////////////////////////////////////////
   template<class V>
   V CalculateBarycentricCenter(const V& a,
                                const V& b,
                                const V& c)
   {
      return (a+b+c) / 3.;
   }


   /////////////////////////////////////////////////////////////////////////////
   /// Transforms Cartesian data points into Barycentric coordinate systems.
   /// http://en.wikipedia.org/wiki/Barycentric_coordinates_%28mathematics%29
   template<class VecT>            // the 3D return type after transforming a data point into barycentric coords.
            //typename RealT=float>  // an internal type used for precision
   class BarycentricSpace
   {
   public:
      BarycentricSpace(const VecT& a, const VecT& b, const VecT& c);

      // So this can be stored in a vector.  The default copy constructor should do fine.
      BarycentricSpace();

      /// transform the data point into the barycentric system.
      /// @param p the data point to be transformed into the Barycentric coordinate system.
      /// @return the Barycentric coordinate.
      VecT Transform(const VecT& p) const;

   private:
      ///\todo make RealT a template parameter
      typedef float RealT;
      RealT A,B,D,E,G,H;
      VecT P3;
   };

   // --- template implementation below --- //

   /////////////////////////////////////////////////////////////////////////////
   template<class V>
   BarycentricSpace<V>::BarycentricSpace(const V& p1, const V& p2, const V& p3)
      : A(p1[0] - p3[0])
      , B(p2[0] - p3[0])
      , D(p1[1] - p3[1])
      , E(p2[1] - p3[1])
      , G(p1[2] - p3[2])
      , H(p2[2] - p3[2])
      , P3(p3)
   {
      ///\todo correct for when the X difference is zero.
   }

   template<class V>
   BarycentricSpace<V>::BarycentricSpace()
      : A()
      , B()
      , D()
      , E()
      , G()
      , H()
      , P3()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   template<class V>
   V BarycentricSpace<V>::Transform(const V& p) const
   {
      RealT C = P3[0] - p[0];
      RealT F = P3[1] - p[1];
      RealT I = P3[2] - p[2];

      RealT num1 = B*(F+I) - C*(E+H);
      RealT den1 = A*(E+H) - B*(D+G);
      RealT Lambda1 = num1 / den1;

      RealT num2 = A*(F+I) - C*(D+G);
      RealT den2 = B*(D+G) - A*(E+H);
      RealT Lambda2 = num2 / den2;

      RealT Lambda3 = 1 - Lambda1 - Lambda2;

      V coord(Lambda1, Lambda2, Lambda3);
      return coord;
   }

}

#endif // _pd_barycentric_h_

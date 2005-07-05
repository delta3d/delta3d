/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_ISECTOR
#define DELTA_ISECTOR

// isector.h: interface for the Isector class.
//
//////////////////////////////////////////////////////////////////////



#include "dtCore/transformable.h"
#include "dtCore/deltadrawable.h"
#include <osgUtil/IntersectVisitor>

namespace dtCore
{
/*!
    This class is used to check for intersections of a line segment with 
    geometry.  Supply the starting position, the direction vector, the length
    of the Isector, and the geometry to intersect with.

    If no geometry is specified with SetGeometry(), the whole Scene is searched.

    After calling Update(), the results of the intersection may be queried with
    GetHitPoint().

\note Although derived from Transformable, this class does not use any 
    inherited functionality from Transformable at this point.


    To search the whole scene for the height of terrain at a given xyz:
\code
    Isector *isect = new Isector();
    sgVec3 queryPoint = {500.f, 500.f, 1000.f};
    isect->SetStartPosition( queryPoint );
    sgVec3 direction = {0.f, 0.f, -1.f};
    isect->SetDirection( direction );

    isect->Update();

    sgVec3 hitPt;
    isect->GetHitPoint( hitPt );
\endcode
*/

   class DT_EXPORT Isector : public Transformable  
   {
   public:
      DECLARE_MANAGEMENT_LAYER(Isector)

      ///Default constructor
      Isector(sgVec3 xyz=NULL, sgVec3 dir=NULL);
      virtual ~Isector();

      ///Set the length of the isector
	   void SetLength( const float distance);
      
      ///Get the intersected point
	   void GetHitPoint( sgVec3 xyz, const int pointNum=0 );

      ///Get the number of intersected items
      int GetNumberOfHits(void);

      ///Set the direction vector
	   void SetDirection( sgVec3 dir );

      ///Set the starting position
	   void SetStartPosition( sgVec3 xyz );

      void SetEndPosition( sgVec3 endXYZ );

      ///Check for intersections
	   bool Update(void);

      ///Supply a particular geometry to intersect
	   void SetGeometry( DeltaDrawable *object);

      /// Get the Hitlist member
      const osgUtil::IntersectVisitor::HitList& GetHitList() const { return mHitList; }
      osgUtil::IntersectVisitor::HitList& GetHitList()             { return mHitList; }

   private:
	   DeltaDrawable* mGeometry;
	   sgVec3 mStartXYZ; ///<The starting xyz
      sgVec3 mEndXYZ;
      sgVec3 mDirVec;   ///<The direction vector
      float mDistance;  ///<The maximum distance for the intersector
      osgUtil::IntersectVisitor::HitList mHitList; ///<The last list of hits
      bool mDirVecSet;
   };
}


#endif // DELTA_ISECTOR

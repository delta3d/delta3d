// isector.h: interface for the Isector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISECTOR_H__EAA7B048_67D4_4869_843A_D758E2D9450D__INCLUDED_)
#define AFX_ISECTOR_H__EAA7B048_67D4_4869_843A_D758E2D9450D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "transformable.h"
#include "drawable.h"

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
      
      ///Get the first intersected point
	   void GetHitPoint( sgVec3 xyz );

      ///Set the direction vector
	   void SetDirection( sgVec3 dir );

      ///Set the starting position
	   void SetStartPosition( sgVec3 xyz );

      ///Check for intersections
	   bool Update(void);

      ///Supply a particular geometry to intersect
	   void SetGeometry( Drawable *object);
      
   private:
	   sgVec3 mHitPoint;
	   Drawable* mGeometry;
	   sgVec3 mStartXYZ;
      sgVec3 mDirVec;
      float mDistance;
   };
}

#endif // !defined(AFX_ISECTOR_H__EAA7B048_67D4_4869_843A_D758E2D9450D__INCLUDED_)

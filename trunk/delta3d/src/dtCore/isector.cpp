// isector.cpp: implementation of the Isector class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/isector.h"
#include "dtCore/scene.h"
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Isector)


/*!
 * Default constructor.  Setup default values for internal members. The length
 * of Isector is set to 10000.0. Use the starting xyz and direction if 
 * supplied.   
 *
 * @param xyz : The starting position of the Isector
 * @param dir : The direction vector of the Isector
 */
 Isector::Isector(const osg::Vec3& xyz, const osg::Vec3& dir):
mGeometry(0),
mDistance(10000.f),
mDirVecSet(false)
{
   RegisterInstance(this);
   mStartXYZ = xyz;
   mDirVec = dir;
}

Isector::~Isector()
{

}



/*!
 * Tell this Isector to intersect with only the supplied DeltaDrawable.  By default,
 * Isector will search the entire Scene.
 *
 * @param *object : The geometry to intersect with
 */
void Isector::SetGeometry(DeltaDrawable *object)
{
   mGeometry = object;
}


/*!
 * Check for intersections using the supplied attributes.  After calling this,
 * the results may be queried using GetHitPoint().
 * @return bool  : True if a valid intersection took place, false otherwise
 */
bool Isector::Update()
{
   bool retVal = false;
   osgUtil::IntersectVisitor iv;
   RefPtr<osg::LineSegment> seg = new osg::LineSegment;
   
   osg::Vec3 endPt;

   if (mDirVecSet)
   {
      //make an end point from the start xyz, direction, and distance
      endPt = mDirVec;
      endPt *= mDistance;
      endPt += mStartXYZ;
   }
   else
   {
      endPt = mEndXYZ;
   }

   seg->set(mStartXYZ, endPt);
   
   iv.addLineSegment(seg.get());
   
   //if we have specifid geometry, traverse it.  Otherwise just use the
   //first Scene defined.
   osg::Node *node = 0;
   if (mGeometry)
   {
      node = mGeometry->GetOSGNode();
   }
   else
   {
      node = Scene::GetInstance(0)->GetSceneNode();
   }

   node->accept(iv);
   
   if (iv.hits())
   {
      mHitList = iv.getHitList(seg.get());
      retVal = true;
   }

   return retVal;
}




/*!
 * Set the starting location for the Isector.
 *
 * @param xyz : XYZ in meters
 */
void Isector::SetStartPosition( const osg::Vec3& xyz )
{
   mStartXYZ = xyz;
}

/*!
* Set the starting location for the Isector.
*
* @param xyz : XYZ in meters
*/
void Isector::SetEndPosition( const osg::Vec3& endXYZ )
{
   mEndXYZ = endXYZ;
   mDirVecSet = false;
}


/*!
 * Set the direction vector to point the Isector.  Vector will be normalized 
 * internally.
 *
 * @param dir : The direction vector in world coordinates
 */
void Isector::SetDirection( const osg::Vec3& dir )
{
   mDirVec = dir;
   mDirVec.normalize();
   
   mDirVecSet = true;
}


/*!
 * Get the intersected point since the last call to Update().
 *
 * @param xyz : The xyz position to be filled out [in/out]
 * @param pointNum:  Which intersection point to return [0..GetNumberOfHits()]
 */
void Isector::GetHitPoint( osg::Vec3& xyz, int pointNum/* =0  */) const
{
   if (pointNum >= GetNumberOfHits()) return;

   osg::Vec3 ip = mHitList[pointNum].getWorldIntersectPoint();
   xyz = ip;

   //osg::Geode *g = mHitList[pointNum]._geode.get();
}


/*!
 * Set the length of the Isector.  By default, it is set to 10000.  Use this
 * to shorten or lengthen the intersection search.
 *
 * @param distance : The length of the Isector in meters
 */
void Isector::SetLength(float distance)
{
   mDistance = distance;
}

/*! 
 * Get the number of items that were intersected by this Isector.  Note: 
 * Isector::Update() must be called prior to calling this method.
 * 
 * @return The number of intersected items
 */
int Isector::GetNumberOfHits() const
{
   return( mHitList.size() );
}

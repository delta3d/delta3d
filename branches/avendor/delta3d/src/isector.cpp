// isector.cpp: implementation of the Isector class.
//
//////////////////////////////////////////////////////////////////////

#include "isector.h"
#include <osgUtil/IntersectVisitor>
#include "scene.h"
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
Isector::Isector(sgVec3 xyz, sgVec3 dir):
mGeometry(NULL),
mDistance(10000.f)
{
   RegisterInstance(this);
   sgZeroVec3(mHitPoint);
   sgZeroVec3(mStartXYZ);
   sgZeroVec3(mDirVec);

   if (xyz) sgCopyVec3(mStartXYZ, xyz);
   if (dir) sgCopyVec3(mDirVec, dir);
}

Isector::~Isector()
{

}



/*!
 * Tell this Isector to intersect with only the supplied Drawable.  By default,
 * Isector will search the entire Scene.
 *
 * @param *object : The geometry to intersect with
 */
void Isector::SetGeometry(Drawable *object)
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
   osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment;
   
   //make an end point from the start xyz, direction, and distance
   sgVec3 endPt;
   sgCopyVec3(endPt, mDirVec);
   sgScaleVec3(endPt, mDistance);
   sgAddVec3(endPt, mStartXYZ );
   
   seg->set(osg::Vec3(mStartXYZ[0], mStartXYZ[1], mStartXYZ[2]),
            osg::Vec3(endPt[0], endPt[1], endPt[2]) );
   
   iv.addLineSegment(seg.get());
   
   //if we have specifid geometry, traverse it.  Otherwise just use the
   //first Scene defined.
   osg::Node *node = NULL;
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
      osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
      if (!hitList.empty())
      {
         osg::Vec3 ip = hitList.front().getWorldIntersectPoint();
         sgSetVec3(mHitPoint, ip[0], ip[1], ip[2] );
         //osg::Vec3 np = hitList.front().getWorldIntersectNormal();
      }
      retVal = true;
   }

   return retVal;
}


/*!
 * Set the starting location for the Isector.
 *
 * @param xyz : XYZ in meters
 */
void Isector::SetStartPosition(sgVec3 xyz)
{
   sgCopyVec3(mStartXYZ, xyz);
}


/*!
 * Set the direction vector to point the Isector.  Vector will be normalized 
 * internally.
 *
 * @param dir : The direction vector in world coordinates
 */
void Isector::SetDirection(sgVec3 dir)
{
   sgCopyVec3(mDirVec, dir);
   sgNormaliseVec3(mDirVec);
}


/*!
 * Get the first intersected point since the last call to Update().
 *
 * @param xyz : The xyz position to be filled out [in/out]
 */
void Isector::GetHitPoint(sgVec3 xyz)
{
   sgCopyVec3(xyz, mHitPoint);
}


/*!
 * Set the length of the Isector.  By default, it is set to 10000.  Use this
 * to shorten or lengthen the intersection search.
 *
 * @param distance : The length of the Isector in meters
 */
void Isector::SetLength(const float distance)
{
   mDistance = distance;
}

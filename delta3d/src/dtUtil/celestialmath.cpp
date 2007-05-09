#include <prefix/dtutilprefix-src.h>
#include <dtUtil/celestialmath.h>
#include <dtUtil/celestialmesh.h>
#include <dtUtil/mathdefines.h>
#include <osg/Quat>

#define TRIANGLE_NOT_FOUND -1

void dtUtil::GetCelestialCoordinates(const osg::Vec3 &origin,
                                     const osg::Vec3 &target_point,
                                     const osg::Vec3 &lookForward,
                                     const osg::Vec3 &lookUp,
                                     float &azimuth, float &elevation )
{  
   osg::Vec3f target = (target_point - origin);
   target.normalize();

   // Project our target vector onto the xy plane 
   // in order to calculate azimuth
   osg::Vec3f targetRight   = target ^ lookUp;
   targetRight.normalize();

   osg::Vec3f targetForward = lookUp ^ targetRight;
   targetForward.normalize();

   osg::Vec3f lookRight = lookForward ^ lookUp;
   lookRight.normalize();

   // Use the projected vector to calculate the azimuth
   // between the projection and the character forward
   float lookDotTargetForward   = targetForward * lookForward;
   float targetDotTargetForward = targetForward * target;

   // acos needs it's parameter between -1 and 1
   lookDotTargetForward = dtUtil::Min(lookDotTargetForward, 1.0f);
   lookDotTargetForward = dtUtil::Max(lookDotTargetForward, -1.0f);

   targetDotTargetForward = dtUtil::Min(targetDotTargetForward, 1.0f);
   targetDotTargetForward = dtUtil::Max(targetDotTargetForward, -1.0f);   
  
   // We need to manually determine the sign
   float azimuthSign   = ((target * lookRight) > 0.0f) ? -1.0f: 1.0f;    
   float elevationSign = ((target * lookUp) > 0.0f) ? 1.0f: -1.0f;

   // We can use the angle between the projection
   // and the original target to determine elevation
   elevation  = acos(targetDotTargetForward) * elevationSign;
   azimuth    = acos(lookDotTargetForward) * azimuthSign;
}


void dtUtil::GetCelestialDirection(const float azimuth,
                                   const float elevation,
                                   const osg::Vec3& forwardDirection,
                                   osg::Vec3 &outDirection)
{
   // Downward elevation is negative so
   // negate the right vector
   osg::Vec3 up(0, 0, 1);
   osg::Vec3 right(1, 0, 0);

   osg::Quat rotateZ, rotateX;
   rotateX.makeRotate(elevation, right);
   rotateZ.makeRotate(azimuth, up); 

   // Rotations must be applied in the following order
   osg::Quat celestialRotation =  rotateX * rotateZ;   
   outDirection = celestialRotation * forwardDirection;   
}


/// http://www.blackpawn.com/texts/pointinpoly/default.html
int dtUtil::FindCelestialTriangleID( const CelestialMesh &mesh, float azimuth, float elevation ) 
{
   unsigned int numTris = mesh.mTriangles.size();

   osg::Vec3f point(azimuth, elevation, 0.0f);

   int animationIndex = TRIANGLE_NOT_FOUND;

   for (unsigned int triIndex = 0; triIndex < numTris; ++triIndex)
   {
      const osg::Vec3 &A = mesh.mTriangles[triIndex].mVertices[0]->mMaxAzMaxEl;
      const osg::Vec3 &B = mesh.mTriangles[triIndex].mVertices[1]->mMaxAzMaxEl;
      const osg::Vec3 &C = mesh.mTriangles[triIndex].mVertices[2]->mMaxAzMaxEl;

      if (!IsPointBetweenVectors(point, A, B, C)) { continue; }
      if (!IsPointBetweenVectors(point, B, A, C)) { continue; }
      if (!IsPointBetweenVectors(point, C, A, C)) { continue; }

      animationIndex = triIndex;
      break;
   }

   return animationIndex;
}

void dtUtil::GetClosestPointOnSegment(const osg::Vec3 &startPoint,
                                      const osg::Vec3 &endPoint,
                                      const osg::Vec3 &refPoint,
                                      osg::Vec3 &closestPoint)
{
   osg::Vec3 lineDirection = endPoint - startPoint;

   float numerator   = (refPoint * lineDirection) - (startPoint * lineDirection);
   float denominator = lineDirection * lineDirection;

   float scale = numerator / denominator;

   if (scale <= 0.0f)
   {
      closestPoint = startPoint;
   }
   else if (scale >= 1.0f)
   {
      closestPoint = endPoint;
   }
   else
   {
      closestPoint = startPoint + lineDirection * scale;
   }
}   

bool dtUtil::IsPointBetweenVectors(const osg::Vec3f &point,
                                   const osg::Vec3f &origin,
                                   const osg::Vec3f &A,
                                   const osg::Vec3f &B) 
{
   osg::Vec3f vector_A = A - origin;
   osg::Vec3f vector_B = B - origin;
   osg::Vec3f vector_P = point - origin;

   osg::Vec3f CrossA   = vector_P ^ vector_A;
   osg::Vec3f CrossB   = vector_B ^ vector_P;
   osg::Vec3f refCross = vector_B ^ vector_A;

   return ((CrossA * refCross >= 0) &&
           (CrossB * refCross >= 0));
}

void dtUtil::MapCelestialToScreen(float azimuth,
                                  float elevation,
                                  float maxDistance,                             
                                  float windowWidth,
                                  float windowHeight,
                                  const osg::Vec2 &screenOrigin,
                                  osg::Vec2 &outScreenPos)
{
   // Transform az/el values to values ranging from 0 to 1
   float normalizedAzimuth   = azimuth / maxDistance;
   float normalizedElevation = elevation / maxDistance;

   // Calculate the final screen position
   outScreenPos[0] = screenOrigin.x() + (windowWidth * normalizedAzimuth);
   outScreenPos[1] = screenOrigin.y() + (windowHeight * normalizedElevation);   
}

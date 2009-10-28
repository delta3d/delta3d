#include <dtAnim/posemesh.h>
#include <dtAnim/posemath.h>

#include <dtUtil/mathdefines.h>
#include <osg/Quat>

////////////////////////////////////////////////////////////////////////////////
void dtAnim::GetCelestialCoordinates(osg::Vec3 target,
                                     const osg::Vec3 &lookForward,                                    
                                     float &azimuth, 
                                     float &elevation)
{  
   target.normalize();

   // Derive the reference frame for the "look" pose
   osg::Vec3 frameRight = lookForward ^ osg::Z_AXIS;
   osg::Vec3 frameUp    = frameRight ^ lookForward;

   osg::Matrix frameMatrix(frameRight.x(),  frameRight.y(),  frameRight.z(),  0.0f,                            
                           lookForward.x(), lookForward.y(), lookForward.z(), 0.0f,
                           frameUp.x(),     frameUp.y(),     frameUp.z(),     0.0f,
                           0.0f,            0.0f,            0.0f,            1.0f);  

   // Transform the target into the proper coordinate frame
   target = frameMatrix * target;

   // Project our target vector onto the xy plane 
   // in order to calculate azimuth
   osg::Vec3f targetRight = target ^ osg::Z_AXIS;
   targetRight.normalize();

   osg::Vec3f targetForward = osg::Z_AXIS ^ targetRight;
   targetForward.normalize();    

   // Use the projected vector to calculate the azimuth
   // between the projection and the character forward
   float lookDotTargetForward   = targetForward * osg::Y_AXIS;
   float targetDotTargetForward = targetForward * target;

   // acos needs it's parameter between -1 and 1
   dtUtil::Clamp(lookDotTargetForward, -1.0f, 1.0f);
   dtUtil::Clamp(targetDotTargetForward, -1.0f, 1.0f);

   // We need to manually determine the sign
   float azimuthSign   = ((target * osg::X_AXIS) > 0.0f) ? -1.0f: 1.0f;    
   float elevationSign = ((target * osg::Z_AXIS) > 0.0f) ? 1.0f: -1.0f;

   // We can use the angle between the projection
   // and the original target to determine elevation
   elevation  = acos(targetDotTargetForward) * elevationSign;
   azimuth    = acos(lookDotTargetForward) * azimuthSign;
}

////////////////////////////////////////////////////////////////////////////////
void dtAnim::GetCelestialDirection(const float azimuth,
                                   const float elevation,
                                   const osg::Vec3& forwardDirection,
                                   const osg::Vec3 &upDirection,
                                   osg::Vec3 &outDirection)
{
   // Downward elevation is negative so
   // negate the right vector  
   osg::Vec3 right = forwardDirection ^ upDirection;

   right.normalize();

   osg::Quat rotateZ, rotateX;
   rotateX.makeRotate(elevation, right);
   rotateZ.makeRotate(azimuth, upDirection); 

   // Rotations must be applied in the following order
   osg::Quat celestialRotation =  rotateX * rotateZ;   
   outDirection = celestialRotation * forwardDirection;   
}

////////////////////////////////////////////////////////////////////////////////
void dtAnim::GetClosestPointOnSegment(const osg::Vec3 &startPoint,
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

////////////////////////////////////////////////////////////////////////////////
bool dtAnim::IsPointBetweenVectors(const osg::Vec3f &point,
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

////////////////////////////////////////////////////////////////////////////////
void dtAnim::MapCelestialToScreen(float azimuth,
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

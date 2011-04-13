/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */

#ifndef DELTA_FIRE_FIGHTER_UTILITY_FUNCTIONS
#define DELTA_FIRE_FIGHTER_UTILITY_FUNCTIONS

#include <dtCore/transform.h>
#include <osg/Math>

const float GRAVITY = 9.8f;                  //meters per second^2
const float yardsPerNM = 2000.0f;
const float secsPerHour = 3600.0f;
const float metersPerKnotPerSecond = 0.51283f;

enum Task { NONE, ATTACK_TARGET };

struct UFWaypoint
{
   float x;
   float y;
   float altitude;
   float speed;
   float desiredArrivalTime;
   Task task;

   UFWaypoint()
   {
      x = 0.0f;
      y = 0.0f;
      altitude = 0.0f;
      speed = 0.0f;
      desiredArrivalTime = 0.0f;
      task = NONE;
   }

   UFWaypoint(float tX, float tY, float tAltitude, float tSpeed,
            float tDesiredArrivalTime = 0.0f, Task tTask = NONE)
   {
      x = tX;
      y = tY;
      altitude = tAltitude;
      speed = tSpeed;
      desiredArrivalTime = tDesiredArrivalTime;
      task = tTask;
   }
};

inline void AdjustX(dtCore::Transform* startPos, float newX, bool relative = false)
{
   float x1, y1, z1, h1, p1, r1;

   startPos->Get(x1, y1, z1, h1, p1, r1);

   if (relative)
   {
      newX += x1;
   }

   startPos->Set(newX, y1, z1, h1, p1, r1);
}

inline void AdjustY(dtCore::Transform* startPos, float newY, bool relative = false)
{
   float x1, y1, z1, h1, p1, r1;

   startPos->Get(x1, y1, z1, h1, p1, r1);

   if (relative)
   {
      newY += y1;
   }

   startPos->Set(x1, newY, z1, h1, p1, r1);
}

inline void AdjustZ(dtCore::Transform* startPos, float newZ, bool relative = false)
{
   float x1, y1, z1, h1, p1, r1;

   startPos->Get(x1, y1, z1, h1, p1, r1);

   if (relative)
   {
      newZ += z1;
   }

   startPos->Set(x1, y1, newZ, h1, p1, r1);
}

inline void AdjustH(dtCore::Transform* startPos, float newH, bool relative = false)
{
   float x1, y1, z1, h1, p1, r1;

   startPos->Get(x1, y1, z1, h1, p1, r1);

   if (relative)
   {
      newH += h1;
   }

   startPos->Set(x1, y1, z1, newH, p1, r1);
}

inline void AdjustP(dtCore::Transform* startPos, float newP, bool relative = false)
{
   float x1, y1, z1, h1, p1, r1;

   startPos->Get(x1, y1, z1, h1, p1, r1);

   if (relative)
   {
      newP += p1;
   }

   startPos->Set(x1, y1, z1, h1, newP, r1);
}

inline void AdjustR(dtCore::Transform* startPos, float newR, bool relative = false)
{
   float x1, y1, z1, h1, p1, r1;

   startPos->Get(x1, y1, z1, h1, p1, r1);

   if (relative)
   {
      newR += r1;
   }

   startPos->Set(x1, y1, z1, h1, p1, newR);
}

inline float ComputeDistance(float x1, float y1, float x2, float y2)
{
   return sqrtf(osg::square(x2 - x1) + osg::square(y2 - y1));
}

/*
 *  Bearing from x1,y1 to x2,y2...
 */
inline float ComputeBearingTo(float x1, float y1, float x2, float y2)
{
   return 180.0f - osg::RadiansToDegrees(atan2f(x2 - x1, y2 - y1));
}

inline void ComputeNewCoordinates(float x1, float y1, float distance, float angle, float &x2, float &y2)
{
   x2 = x1 + distance * sinf(osg::DegreesToRadians(angle));
   y2 = y1 - distance * cosf(osg::DegreesToRadians(angle));
}

inline float ConvertTodtCoreBearing(float bearing)
{
   return 360.0f - bearing;
}

inline float ConvertFromdtCoreBearing(float bearing)
{
   return 360.0f - bearing;
}

inline void VerifyBearingWithinBounds(float& bearing)
{
   while (bearing >= 360.0f)
   {
      bearing -= 360.0f;
   }

   while (bearing < .0f)
   {
      bearing += 360.0f;
   }
}

inline dtCore::Transform Offset2DPosition(dtCore::Transform* startPos, dtCore::Transform* offsetPos)
{
   dtCore::Transform tempPos;
   float x1, y1, z1, h1, p1, r1;
   float x2, y2, z2, h2, p2, r2;

   startPos->Get(x1, y1, z1, h1, p1, r1);
   offsetPos->Get(x2, y2, z2, h2, p2, r2);

   float newX2 = x2 * cosf(osg::DegreesToRadians(h1)) - y2 * sinf(osg::DegreesToRadians(h1));
   float newY2 = x2 * sinf(osg::DegreesToRadians(h1)) + y2 * cosf(osg::DegreesToRadians(h1));

   tempPos.Set(x1 + newX2, y1 + newY2, z2, h1 + h2, p2, r2);

   return tempPos;
}

inline dtCore::Transform Difference2DPosition(dtCore::Transform* startPos, dtCore::Transform* offsetPos)
{
   dtCore::Transform tempPos;
   float x1, y1, z1, h1, p1, r1;
   float x2, y2, z2, h2, p2, r2;

   startPos->Get(x1, y1, z1, h1, p1, r1);
   offsetPos->Get(x2, y2, z2, h2, p2, r2);

   float bearing = 0.0f - h1;

   float newX2 = (x2 - x1) * cosf(osg::DegreesToRadians(bearing)) - (y2 - y1) * sinf(osg::DegreesToRadians(bearing));
   float newY2 = (x2 - x1) * sinf(osg::DegreesToRadians(bearing)) + (y2 - y1) * cosf(osg::DegreesToRadians(bearing));

   tempPos.Set(newX2, newY2, z2, h2 - h1, p2, r2);
   return tempPos;
}

#endif

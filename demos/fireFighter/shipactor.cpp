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

#include <fireFighter/shipactor.h>
#include <fireFighter/utilityfunctions.h>
#include <dtAudio/sound.h>
#include <dtCore/particlesystem.h>
#include <osg/Math>
#include <cmath>

/////////////////////////////////////////////////
ShipActorProxy::ShipActorProxy()
{

}

ShipActorProxy::~ShipActorProxy()
{

}

void ShipActorProxy::BuildPropertyMap()
{
   VehicleActorProxy::BuildPropertyMap();
}

void ShipActorProxy::BuildInvokables()
{
   VehicleActorProxy::BuildInvokables();
}

/////////////////////////////////////////////////
IMPLEMENT_ENUM(ShipActor::ThrottlePosition);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::BACK_EMERGENCY("BACK_EMERGENCY", 0);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::BACK_FULL("BACK_FULL", 1);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::BACK_TWO_THIRDS("BACK_TWO_THIRDS", 2);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::BACK_ONE_THIRD("BACK_ONE_THIRD", 3);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::STOP("STOP", 4);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::AHEAD_ONE_THIRD("AHEAD_ONE_THIRD", 5);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::AHEAD_TWO_THIRDS("AHEAD_TWO_THIRDS", 6);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::AHEAD_STANDARD("AHEAD_STANDARD", 7);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::AHEAD_FULL("AHEAD_FULL",  8);
ShipActor::ThrottlePosition ShipActor::ThrottlePosition::AHEAD_FLANK("AHEAD_FLANK", 9);

ShipActor::ShipActor(dtGame::GameActorProxy& parent)
   : VehicleActor(parent)
   , shaftEngaged(true)
   , position(0.0f, 0.0f, 100.0f)
   , course(0.0f)
   , heading(0.0f)
   , maxAheadSpeed(30.0f)
   , maxAsternSpeed(-20.0f)
   , speed(0.0f)
   , desiredThrottle(0.0f)
   , throttle(0.0f)
   , throttleRate(3.0f)
   , maxRudderAngle(37.0f)
   , effRudderAngle(.0f)
   , rudderAngle(0.0f)
   , desiredRudderAngle(0.0f)
   , rudderSwingRate(6.0f)
   , tacticalDiameter(1000.0f)
   , heel(0.0f)
   , displacement(8300.0f)
   , shaftHP(100000.0f)
   , heelFactor(0.5f)
{

}

ShipActor::~ShipActor()
{

}

void ShipActor::TimeUpdate(double deltaTime)
{
   if (mEngineRunning)
   {
      dtCore::Transform tempPos;
      GetTransform(tempPos);
      SetPosition(tempPos);
      Update(deltaTime);
      SetModelPosition();
   }
}

void ShipActor::SetModelPosition()
{
   dtCore::Transform tempPos;
   dtCore::Transform newPos = GetPosition();

   float speedOffset = 3.5f * GetSpeed() / GetMaxAheadSpeed();

   if (CheckWake(portWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &portWakePosition);
      portWake->SetTransform(tempPos);
   }

   if (CheckWake(stbdWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &stbdWakePosition);
      stbdWake->SetTransform(tempPos);
   }

   if (CheckWake(portBowWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &portBowWakePosition);
      AdjustZ(&tempPos, speedOffset, true);
      portBowWake->SetTransform(tempPos);
   }

   if (CheckWake(stbdBowWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &stbdBowWakePosition);
      AdjustZ(&tempPos, speedOffset, true);
      stbdBowWake->SetTransform(tempPos);
   }

   if (CheckWake(portRooster.get()))
   {
      tempPos = Offset2DPosition(&newPos, &portRoosterPosition);
      AdjustZ(&tempPos, 1.2f * speedOffset, true);
      AdjustX(&tempPos, GetHeel() / -9.0f, true);
      portRooster->SetTransform(tempPos);
   }

   if (CheckWake(stbdRooster.get()))
   {
      tempPos = Offset2DPosition(&newPos, &stbdRoosterPosition);
      AdjustZ(&tempPos, 1.2f * speedOffset, true);
      AdjustX(&tempPos, GetHeel() / -9.0f, true);
      stbdRooster->SetTransform(tempPos);
   }

   newPos = GetPosition();
   SetTransform(newPos, *mCoordSys == ShipActor::CoordSys::SYS_ABS ? ABS_CS : REL_CS);
}

bool ShipActor::CheckWake(dtCore::ParticleSystem* wake)
{
   if (wake != NULL)
   {
      if (wake->IsEnabled())
      {
         if (!mEngineRunning || !shaftEngaged)
         {
            wake->SetEnabled(false);
         }
      }
      else
      {
         if (mEngineRunning && shaftEngaged)
         {
            wake->SetEnabled(true);
         }
      }

      return wake->IsEnabled();
   }
   else
   {
      return false;
   }
}

void ShipActor::SetPortWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition)
{
   portWake = wake;
   portWakePosition = wakePosition;
   portWake->SetTransform(portWakePosition);
}

void ShipActor::SetStbdWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition)
{
   stbdWake = wake;
   stbdWakePosition = wakePosition;
   stbdWake->SetTransform(stbdWakePosition);
}

void ShipActor::SetPortBowWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition)
{
   portBowWake = wake;
   portBowWakePosition = wakePosition;
   portBowWake->SetTransform(portBowWakePosition);
}
void ShipActor::SetStbdBowWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition)
{
   stbdBowWake = wake;
   stbdBowWakePosition = wakePosition;
   stbdBowWake->SetTransform(stbdBowWakePosition);
}

void ShipActor::SetPortRooster(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition)
{
   portRooster = wake;
   portRoosterPosition = wakePosition;
   portRooster->SetTransform(portRoosterPosition);
}

void ShipActor::SetStbdRooster(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition)
{
   stbdRooster = wake;
   stbdRoosterPosition = wakePosition;
   stbdRooster->SetTransform(stbdRoosterPosition);
}

void ShipActor::EngageShaft()
{
   shaftEngaged = true;
}

void ShipActor::DisengageShaft()
{
   shaftEngaged = false;
}

void ShipActor::SetSternWakeSound(dtAudio::Sound* tWakeSound, dtCore::Transform tWakeSoundPosition)
{
   wakeSound = tWakeSound;
   wakeSoundPosition = tWakeSoundPosition;
   wakeSound->SetTransform(wakeSoundPosition, dtCore::Transformable::REL_CS);
   wakeSound->SetLooping(true);
}

void ShipActor::PlayWakeSound()
{
   wakeSound->Play();
}

void ShipActor::StopWakeSound()
{
   wakeSound->Stop();
}

void ShipActor::GetCoordinates(float& x, float& y)
{
   float z;
   position.GetTranslation(x, y, z);
}

float ShipActor::GetHeading()
{
   return heading;
}

float ShipActor::GetCourse()
{
   return course;
}

void ShipActor::SetCourse(float crs)
{
   VerifyBearingWithinBounds(crs);

   desiredRudderAngle = .0f;
   rudderAngle = .0f;
   course = crs;
}

float ShipActor::GetSpeed()
{
   return speed;
}

void ShipActor::SetSpeed(float spd)
{
   if (spd < maxAheadSpeed)
   {
      if (spd > maxAsternSpeed)
      {
         speed = spd;
      }
      else
      {
         speed = maxAsternSpeed;
      }
   }
   else
   {
      speed = maxAheadSpeed;
   }

   desiredThrottle = speed;
   throttle = speed;
}

float ShipActor::GetMaxAheadSpeed()
{
   return maxAheadSpeed;
}

void ShipActor::SetMaxAheadSpeed(float maxAhdSpd)
{
   maxAheadSpeed = maxAhdSpd;
}

float ShipActor::GetMaxAsternSpeed()
{
   return maxAsternSpeed;
}

void ShipActor::SetMaxAsternSpeed(float maxAstnSpd)
{
   maxAsternSpeed = maxAstnSpd;
}

void ShipActor::SetDesiredThrottlePosition(float desiredSpeed)
{
   if (desiredSpeed < maxAheadSpeed)
   {
      if (desiredSpeed > maxAsternSpeed)
      {
         desiredThrottle = desiredSpeed;
      }
      else
      {
         desiredThrottle = maxAsternSpeed;
      }
   }
   else
   {
      desiredThrottle = maxAheadSpeed;
   }
}

void ShipActor::SetDesiredThrottlePosition(ShipActor::ThrottlePosition &bell)
{
   float desiredSpeed = 0.0f;

   if (bell == ShipActor::ThrottlePosition::BACK_EMERGENCY)
   {
      desiredSpeed = maxAsternSpeed;
   }
   else if (bell == ShipActor::ThrottlePosition::AHEAD_FLANK)
   {
      desiredSpeed = maxAheadSpeed;
   }
   else
   {
      // The old version of this code uses a C style enum in which using the expression
      // bell - 1 would give you the item before you in the enum. To work around this,
      // I passed the C value into the constructor of the ThrottlePosition enumeration so that it
      // can be used here, as you cannot subtract 1 from a dtUtil::Enumeration

      // /*Old version*/ desiredSpeed = ((bell - 1) * 5.0f) - 15.0f; //convert the bell into knots
      desiredSpeed = ((bell.GetValue() - 1) * 5.0f) - 15.0f; //convert the bell into knots
   }

   SetDesiredThrottlePosition(desiredSpeed);  //ensures within max ahead/astern
}

float ShipActor::GetThrottlePosition()
{
   return throttle;
}

void ShipActor::SetThrottleRate(float engineRate)
{
   throttleRate = engineRate;
}

dtCore::Transform ShipActor::GetPosition()
{
   return position;
}

dtCore::Transform* ShipActor::GetUpdatingPosition()
{
   return &position;
}

void ShipActor::SetPosition(dtCore::Transform newPosition)
{
   position = newPosition;
}

void ShipActor::SetDesiredRudderAngle(float rudder)
{
   if (rudder > maxRudderAngle)
   {
      desiredRudderAngle = maxRudderAngle;
   }
   else if (rudder < (-1 * maxRudderAngle))
   {
      desiredRudderAngle = -1 * maxRudderAngle;
   }
   else
   {
      desiredRudderAngle = rudder;
   }
}

void ShipActor::SetMaxRudderAngle(float maxRudder)
{
   maxRudderAngle = maxRudder;
}

float ShipActor::GetMaxRudderAngle()
{
   return maxRudderAngle;
}

float ShipActor::GetRudderSwingRate()
{
   return maxRudderAngle;
}

void ShipActor::SetRudderSwingRate(float swingRate)
{
   rudderSwingRate = swingRate;
}

float ShipActor::GetRudderAngle()
{
   return rudderAngle;
}

float ShipActor::GetEffRudderAngle()
{
   return effRudderAngle;
}

void ShipActor::Update(float elapsedTime)
{
   UpdateRudder(elapsedTime);
   UpdateThrottle(elapsedTime);
   UpdateSpeed(elapsedTime);
   UpdateEffRudderAndHeel(elapsedTime);
   UpdateCourse(elapsedTime);
   UpdatePosition(elapsedTime);
}

void ShipActor::UpdateRudder(float elapsedTime)
{
   //update actual rudder angle
   if (rudderAngle != desiredRudderAngle)
   {
      //handle right rudder order
      if (desiredRudderAngle > 0.0f)
      {

         //check actual and desired rudder are in same direction
         if (rudderAngle > 0.0f)
         {

            //rudder needs to go farther right
            if (desiredRudderAngle - rudderAngle > 0.0f)
            {
               rudderAngle += rudderSwingRate * elapsedTime;
               if (rudderAngle > desiredRudderAngle)
               {
                  rudderAngle = desiredRudderAngle;
               }
            }

            //rudder too far right
            else
            {
               rudderAngle -= rudderSwingRate * elapsedTime;
               if (rudderAngle < desiredRudderAngle)
               {
                  rudderAngle = desiredRudderAngle;
               }
            }
         }

         //handle actual and desired rudder in opposite directions
         else
         {
            rudderAngle += rudderSwingRate * elapsedTime;
            if (rudderAngle > desiredRudderAngle)
            {
               rudderAngle = desiredRudderAngle;
            }
         }
      }

      //handle left rudder order
      else if (desiredRudderAngle < 0.0f)
      {

         //check actual and desired rudder are in same direction
         if (rudderAngle < 0.0f)
         {

            //rudder needs to go farther left
            if (desiredRudderAngle - rudderAngle < 0.0f)
            {
               rudderAngle -= rudderSwingRate * elapsedTime;
               if (rudderAngle < desiredRudderAngle)
               {
                  rudderAngle = desiredRudderAngle;
               }
            }

            //rudder too far left
            else
            {
               rudderAngle += rudderSwingRate * elapsedTime;
               if (rudderAngle > desiredRudderAngle)
               {
                  rudderAngle = desiredRudderAngle;
               }
            }
         }

         //handle actual and desired rudder in opposite directions
         else
         {
            rudderAngle -= rudderSwingRate * elapsedTime;
            if (rudderAngle < desiredRudderAngle)
            {
               rudderAngle = desiredRudderAngle;
            }
         }
      }

      //handle rudder amidship order
      else
      {
         if (rudderAngle > 0.0f)
         {
            rudderAngle -= rudderSwingRate * elapsedTime;
            if (rudderAngle < desiredRudderAngle)
            {
               rudderAngle = desiredRudderAngle;
            }
         }
         else
         {
            rudderAngle += rudderSwingRate * elapsedTime;
            if (rudderAngle > desiredRudderAngle)
            {
               rudderAngle = desiredRudderAngle;
            }
         }
      }
   }
}

void ShipActor::UpdateThrottle(float elapsedTime)
{
   if (desiredThrottle != throttle)
   {
      if (desiredThrottle > throttle)
      {
         throttle += throttleRate * elapsedTime;
         if (throttle > desiredThrottle)
         {
            throttle = desiredThrottle;
         }
      }
      else
      {
         throttle -= throttleRate * elapsedTime;
         if (throttle < desiredThrottle)
         {
            throttle = desiredThrottle;
         }
      }
   }
}

void ShipActor::UpdateSpeed(float elapsedTime)
{
   speed += (throttle - speed - (speed * std::abs(rudderAngle) * 0.005f)) * 0.09f * elapsedTime;
}

void ShipActor::UpdateEffRudderAndHeel(float elapsedTime)
{
   //update effective rudder angle
   if (rudderAngle > effRudderAngle)
   {
      effRudderAngle += (std::abs(rudderAngle - effRudderAngle))
         / log(displacement) * elapsedTime;
   }
   else if (rudderAngle < effRudderAngle)
   {
      effRudderAngle -= (std::abs(rudderAngle - effRudderAngle))
         / log(displacement) * elapsedTime;
   }

   //update heel
   if (speed > 0.0f)
   {
      heel = std::abs(effRudderAngle) * (speed / maxAheadSpeed) * heelFactor;

      //heel to side opposite the turn
      if (effRudderAngle < 0.0f)
      {
         heel *= -1.0f;
      }
   }
   else
   {
      heel = 0.0f;
   }
}

void ShipActor::UpdateCourse(float elapsedTime)
{
   if (effRudderAngle != 0.0f)
   {
      float deltaCourse = (360.0f * (speed * yardsPerNM / secsPerHour))
         / (GetAdjustedTacticalDiameter() * osg::PI) * elapsedTime;

      if (effRudderAngle > 0.0f)
      {
         course += deltaCourse;
      }
      else
      {
         course -= deltaCourse;
      }
   }

   //keep in range: 0 <= course < 360.0f
   while (course >= 360.0f)
   {
      course -= 360.0f;
   }

   while (course < .0f)
   {
      course += 360.0f;
   }
}

void ShipActor::UpdatePosition(float elapsedTime)
{
   float x,y,z,h,p,r;
   float p51Bearing = ConvertTodtCoreBearing(course);
   float distance = speed * metersPerKnotPerSecond * elapsedTime;

   position.Get(x, y, z, h, p, r);

   position.Set(
      x += distance * sinf(osg::DegreesToRadians(p51Bearing)),
      y -= distance * cosf(osg::DegreesToRadians(p51Bearing)),
      z,
      p51Bearing,
      p,
      heel);
}

void ShipActor::SetTacticalDiameter(float diameter)
{
   tacticalDiameter = diameter;
}

float ShipActor::GetAdjustedTacticalDiameter()
{
   float adjTacDia = .0f;
   float absSpeed = std::abs(speed);

   //check for sufficient water flow over rudder
   if (absSpeed < 3.0f)
   {
      return 30000.0f;
   }

   float factor = (15.0f / absSpeed) * std::abs(effRudderAngle);

   adjTacDia = (tacticalDiameter * 15.0f * factor) / (factor * factor);

   if (adjTacDia > 30000.0f)
   {
      adjTacDia = 30000.0f;
   }

   return adjTacDia;
}

void ShipActor::SetDisplacement(float tonnage)
{
   displacement = tonnage;
}

void ShipActor::SetShaftHP(float shp)
{
   shaftHP = shp;
}

float ShipActor::GetHeel()
{
   return heel;
}

void ShipActor::SetHeelFactor(float tHeelFactor)
{
   heelFactor = tHeelFactor;
}

float ShipActor::GetHeelFactor()
{
   return heelFactor;
}

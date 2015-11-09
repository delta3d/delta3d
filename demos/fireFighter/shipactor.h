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

#ifndef DELTA_FIRE_FIGHTER_SHIP_ACTOR
#define DELTA_FIRE_FIGHTER_SHIP_ACTOR

#include <fireFighter/vehicleactor.h>
#include <dtCore/transform.h>
#include <fireFighter/export.h>

namespace dtCore
{
   class ParticleSystem;
}

namespace dtAudio
{
   class Sound;
}

class FIRE_FIGHTER_EXPORT ShipActor : public VehicleActor
{
   public:

      class FIRE_FIGHTER_EXPORT ThrottlePosition : public dtUtil::Enumeration
      {
         DECLARE_ENUM(ThrottlePosition);

         public:

            static ThrottlePosition BACK_EMERGENCY;   //max astern speed
            static ThrottlePosition BACK_FULL;        //~-15 knots
            static ThrottlePosition BACK_TWO_THIRDS;  //~-10 knots
            static ThrottlePosition BACK_ONE_THIRD;   //~-5 knots ordered
            static ThrottlePosition STOP;             //zero knots ordered
            static ThrottlePosition AHEAD_ONE_THIRD;  //~5 knots ordered
            static ThrottlePosition AHEAD_TWO_THIRDS; //~10 knots ordered
            static ThrottlePosition AHEAD_STANDARD;   //~15 knots ordered
            static ThrottlePosition AHEAD_FULL;       //~20 knots ordered
            static ThrottlePosition AHEAD_FLANK;      //max ahead speed

            int GetValue() { return mCValue; }

         private:

            ThrottlePosition(const std::string &name,
                             const int cValue) :
               dtUtil::Enumeration(name),
               mCValue(cValue)
            {
               AddInstance(this);
            }

            const int mCValue;
      };

      /// Constructor
      ShipActor(dtGame::GameActorProxy& parent);

      virtual void TimeUpdate(double deltaTime);

      virtual void SetModelPosition();

      void SetPortWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition);

      void SetStbdWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition);

      void SetPortBowWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition);

      void SetStbdBowWake(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition);

      void SetPortRooster(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition);

      void SetStbdRooster(dtCore::ParticleSystem* wake, dtCore::Transform wakePosition);

      void EngageShaft();

      void DisengageShaft();

      void SetSternWakeSound(dtAudio::Sound* wakeSound, dtCore::Transform wakeSoundPosition);

      void PlayWakeSound();

      void StopWakeSound();

      void GetCoordinates(float& x, float& y);

      float GetHeading();

      float GetCourse();

      /*
       * Setting course will change the course immediately.  If a realistic
       * course adjustment is desired, set the desired rudder angle and
       * monitor the course until the proper course is achieved.
       */
      void SetCourse(float crs);

      float GetSpeed();

      /*
       * Setting speed will change the speed immediately.  If a realistic
       * speed adjustment is desired, set the desired throttle position and
       * the speed will eventually change to the proper position.
       */
      void SetSpeed(float spd);

      float GetMaxAheadSpeed();

      float GetMaxAsternSpeed();

      void SetMaxAheadSpeed(float maxAhdSpd);

      void SetMaxAsternSpeed(float maxAstnSpd);

      /*
       * Desired speed in knots.  Ensures the desiredSpeed is within max
       * ahead and max astern values.
       */
      void SetDesiredThrottlePosition(float desiredSpeed);

      void SetDesiredThrottlePosition(ThrottlePosition& bell);

      float GetThrottlePosition();

      void SetThrottleRate(float engineRate);

      dtCore::Transform GetPosition();

      dtCore::Transform* GetUpdatingPosition();

      void SetPosition(dtCore::Transform newPosition);

      void SetDesiredRudderAngle(float rudder);

      void SetMaxRudderAngle(float maxRudder);

      float GetMaxRudderAngle();

      float GetRudderSwingRate();

      void SetRudderSwingRate(float tRudderSwingRate);

      float GetRudderAngle();

      float GetEffRudderAngle();

      void Update(float elapsedTime);

      /*
       * The Tactical Diameter is the diameter a ship can turn within (once
       * heeled) at 15 knots speed with 15 degrees rudder ordered.  All other
       * speed/rudder combinations will be derived from this number.  1000
       * yards is a reasonable default as it is used for Naval multi-ship
       * maneuvers.
       */
      void SetTacticalDiameter(float diameter = 1000.0f);

      /*
       * Adjusted Tactical Diameter is a function of speed and ruddeer angle.
       * The adjustment is based on the graph of the function f(x) = x/(x^2).
       * It is a loose approximation of the effects of speed and rudder angle
       * on the tactical diameter.
       */
      float GetAdjustedTacticalDiameter();

      void SetDisplacement(float tonnage);

      void SetShaftHP(float shp);

      /**
       * The list of the ship during a turn.
       */
      float GetHeel();

      void SetHeelFactor(float tHeelFactor);

      float GetHeelFactor();

   protected:

      bool CheckWake(dtCore::ParticleSystem* wake);

      void UpdateRudder(float elapsedTime);

      void UpdateThrottle(float elapsedTime);

      void UpdateSpeed(float elapsedTime);

      void UpdateEffRudderAndHeel(float elapsedTime);

      void UpdateCourse(float elapsedTime);

      void UpdatePosition(float elapsedTime);

      dtCore::RefPtr<dtCore::ParticleSystem> portWake;
      dtCore::RefPtr<dtCore::ParticleSystem> stbdWake;
      dtCore::RefPtr<dtCore::ParticleSystem> portBowWake;
      dtCore::RefPtr<dtCore::ParticleSystem> stbdBowWake;
      dtCore::RefPtr<dtCore::ParticleSystem> portRooster;
      dtCore::RefPtr<dtCore::ParticleSystem> stbdRooster;
      dtCore::Transform portWakePosition;
      dtCore::Transform stbdWakePosition;
      dtCore::Transform portBowWakePosition;
      dtCore::Transform stbdBowWakePosition;
      dtCore::Transform portRoosterPosition;
      dtCore::Transform stbdRoosterPosition;
      bool engineRunning;
      bool shaftEngaged;
      dtCore::RefPtr<dtAudio::Sound> wakeSound;
      dtCore::Transform wakeSoundPosition;
      dtCore::Transform position;
      float course;             //0 <= degrees < 360, absolute course
      float heading;            //0 <= degrees < 360, absolute ship's head
      float maxAheadSpeed;      //knots
      float maxAsternSpeed;     //knots
      float speed;              //knots, actual speed
      float desiredThrottle;    //knots, desired throttle
      float throttle;           //knots, answered throttle
      float throttleRate;       //knots/second, (engine spool)
      float maxRudderAngle;     //degrees (+/-)
      float effRudderAngle;     //degrees (+/-), considers momentum of ship
      float rudderAngle;        //degrees, actual rudder angle
      float desiredRudderAngle; //degrees
      float rudderSwingRate;    //degrees/second, movement of rudder
      float tacticalDiameter;   //yards
      float heel;               //degrees
      float displacement;       //tons
      float shaftHP;            //shaft horsepower
      float heelFactor;         //heeling tendency, 0..1

   protected:

      /// Destructor
      virtual ~ShipActor();

   private:
};

class FIRE_FIGHTER_EXPORT ShipActorProxy : public VehicleActorProxy
{
   public:

      /// Constructor
      ShipActorProxy();

      /// Builds the properties of this actor
      virtual void BuildPropertyMap();

      /// Builds the invokables of this actor
      virtual void BuildInvokables();

      /// Instantiates the actor itself
      virtual void CreateDrawable() { SetDrawable(*new ShipActor(*this)); }

   protected:

      /// Destructor
      virtual ~ShipActorProxy();

   private:
};

#endif

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

#ifndef DELTA_FIRE_FIGHTER_FIRE_ACTOR
#define DELTA_FIRE_FIGHTER_FIRE_ACTOR

#include <fireFighter/gameitemactor.h>
#include <fireFighter/export.h>
#include <osg/Vec4>
#include <vector>

namespace dtCore
{
   class ActorProxyIcon;
}

namespace dtCore
{
   class ParticleSystem;
   class PositionalLight;
}

class FIRE_FIGHTER_EXPORT FireActor : public GameItemActor
{
   public:

      /// Constructor
      FireActor();

      /// Builds the properties of this actor
      void BuildPropertyMap() override;

      /// Builds the invokables of this actor
      void BuildInvokables() override;

      // Used in STAGE
      dtCore::ActorProxyIcon* GetBillBoardIcon() override;

      const dtCore::BaseActorObject::RenderMode& GetRenderMode() override
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
      }

      /**
       * Sets the filename of the flame particle system.
       * @param filename the new filename
       */
      void SetFlameFilename(const std::string& filename);

      /**
       * Sets the filename of the spark particle system.
       * @param filename the new filename
       */
      void SetSparkFilename(const std::string& filename);

      /**
       * Sets the filename of the smoke particle system.
       * @param filename the new filename
       */
      void SetSmokeFilename(const std::string& filename);

      /**
       * Sets the filename of the smoke ceiling particle system.
       * @param filename the new filename
       */
      void SetSmokeCeilingFilename(const std::string& filename);

      /**
       * Sets the radius of the fire.
       * @param radius the new radius
       */
      void SetRadius(float radius);

      /**
       * Returns the radius of the fire.
       * @return the current radius
       */
      float GetRadius() const;

      /**
       * Sets the intensity of the fire.
       * @param intensity the new intensity
       */
      void SetIntensity(float intensity);

      /**
       * Decreases the intensity of the fire
       * @param intensity The intensity to subtract
       */
      void DecreaseIntensity(float intensity);

      /**
       * Returns the intensity of the fire.
       * @return the current intensity
       */
      float GetIntensity() const;

      /**
       * Adds a boundary plane to contain the particle systems
       * associated with this fire.
       * @param plane the boundary plane to add
       */
      void AddBoundaryPlane(const osg::Vec4& plane);

      /**
       * Removes the boundary plane at the specified index.
       * @param index the index of the boundary plane to remove
       */
      void RemoveBoundaryPlane(int index);

      /**
       * Gets the number of active boundary planes.
       * @return the number of active boundary planes
       */
      int GetBoundaryPlaneCount() const;

      /**
       * Gets the boundary plane at the specified index.
       * @param dest the destination vector to hold the plane parameters
       * @param index the index of the boundary plane to retrieve
       */
      void GetBoundaryPlane(osg::Vec4& dest, int index) const;

      /**
       * Message handler.
       * @param data the received message
       */
      void OnTickLocal(const dtGame::TickMessage& msg);

      /// Invokable that plays the fire sound when the hatch door is opened or closed
      void PlayFireSound(const dtGame::Message& msg);

      /// Invokable to stop all sounds associated with this actor
      void StopSounds(const dtGame::Message& msg);

      /**
       * Sets the light rotation
       * @param rotation The rotation to set
       */
      void SetLightRotation(const osg::Vec3& rotation);

      /**
       * Gets the light rotation
       * @return The rotation
       */
      osg::Vec3 GetLightRotation() const;

      /**
       * Sets the light rotation
       * @param rotation The rotation to set
       */
      void SetLightTranslation(const osg::Vec3& xyz);

      /**
       * Gets the light rotation
       * @return The rotation
       */
      osg::Vec3 GetLightTranslation() const;


   protected:

      /// Called when the actor is added to the game manager
      void OnEnteredWorld() override;

      /// Destructor
      virtual ~FireActor();

   private:
      dtCore::RefPtr<dtCore::ParticleSystem> mFlameSystem, mSparkSystem, mSmokeSystem, mCeilingSystem;

      dtCore::RefPtr<dtCore::PositionalLight> mLight;

      /// The radius of the fire.
      float mRadius;

      /// The intensity of the fire.
      float mIntensity;

      /// The boundary planes that limit the particle systems.
      std::vector<osg::Vec4> mBoundaryPlanes;
};

#endif

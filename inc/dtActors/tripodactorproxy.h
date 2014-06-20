/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2005, MOVES Institute
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
* Chris Osborn
*/

#ifndef DELTA_TRIPOD_ACTOR_PROXY
#define DELTA_TRIPOD_ACTOR_PROXY

#include <osg/Vec3>
#include <dtCore/actorproxy.h>
#include <dtCore/plugin_export.h>
#include <dtUtil/deprecationmgr.h>

namespace dtActors
{
   /**
   * @class TripodActorProxy
   * @brief This proxy wraps the dtCore::Tripod Delta3D object.
   */
   class DT_PLUGIN_EXPORT TripodActorProxy : public dtCore::BaseActorObject
   {

   public:

      class DT_PLUGIN_EXPORT TetherModeEnum : public dtUtil::Enumeration
      {
            DECLARE_ENUM(TetherModeEnum)

         public:

            static TetherModeEnum TETHER_PARENT_REL;
            static TetherModeEnum TETHER_WORLD_REL;

         private:

            TetherModeEnum( const std::string &name ) : dtUtil::Enumeration(name)
            {
               AddInstance(this);
            }
      };

      TripodActorProxy() { SetClassName("dtCore::Tripod"); }

      /**
      * Adds the properties that are common to all Delta3D camera objects.
      */
      virtual void BuildPropertyMap();

      /**
      * Tripods cannot have a position in the scene. Instead they
      * provides offsets to things that are in the scene. The indicates 
      * that the actor is a "global" actor.
      *
      * @return false
      * @see BaseActorObject::IsPlaceable()
      */
      virtual bool IsPlaceable() const { return false; }


      /**
      * Sets the transformable which this tripod will move
      * @param transformableProxy the transformable to move
      */
      void SetChild(dtCore::BaseActorObject* cameraProxy);

      /**
      * Gets the transformable which this tripod is moving
      * @return the transformable which the tripod is moving
      */
      dtCore::DeltaDrawable* GetChild();

      /**
      * Sets the transformable (i.e. parent) which this tripod will
      * move the camera in relation to
      * @param transformableProxy the parent transformable
      */
      void SetAttachToTransformable( dtCore::BaseActorObject* transformableProxy );

      /**
      * Gets the transformable (i.e. parent) which this tripod is
      * moving the camera in relation to
      * @return the parent transformable
      */
      dtCore::DeltaDrawable* GetAttachedTransformable();

      /**
      * Sets the transformable you wish to target the parent at
      * @param targetProxy the desired target
      */
      void SetLookAtTarget( dtCore::BaseActorObject* targetProxy );

      /**
      * Gets the transformable which the parent is targeted at
      * @return the target
      */
      dtCore::DeltaDrawable* GetLookAtTarget();

      /**
      * Sets the rotation offset from the parent
      * @param rotation the desired rotation offset
      */
      void SetRotationOffset( const osg::Vec3& rotation );

      /**
      * Gets the rotation offset from the parent
      * @return The current rotation offset vector
      */
      osg::Vec3 GetRotationOffset();

      /**
      * Sets the translation offset from the parent
      * @param translation the desired translation offset
      */
      void SetTranslationOffset( const osg::Vec3& translation );

      /**
      * Gets the translation offset from the parent
      * @return The current translation offset
      */
      osg::Vec3 GetTranslationOffset();

      /**
      * Sets the scale on the translation offset from the parent
      * @param scale the desired scale on the translation offset
      */
      void SetTranslationScaleOffset( const osg::Vec3& scale );

      /**
      * Gets the scale on the translation offset from the parent
      * @return The current scale vector on the translation offset
      */
      osg::Vec3 GetTranslationScaleOffset();

      /**
      * Sets the scale on the rotation offset from the parent
      * @param scale the desired scale on the rotation offset
      */
      void SetRotationScaleOffset( const osg::Vec3& scale );

      /**
      * Gets the scale on the rotation offset from the parent
      * @return The current scale vector on the rotation offset
      */
      osg::Vec3 GetRotationScaleOffset();

      /**
      * Sets the tether mode for this tripod actor
      * @param mode The new tether mode for the actor
      */
      void SetTetherMode( TetherModeEnum& mode );

      /**
      * Gets the tether mode for this tripod actor.
      * @return The current tether mode for the actor
      */
      TetherModeEnum& GetTetherMode() const;

      /// Supports the following deprecated properties: 'Camera' 
      virtual dtCore::RefPtr<dtCore::ActorProperty> GetDeprecatedProperty(const std::string& name);


      /** Deprecated 5/23/2011. Call SetChild() instead. */
      DEPRECATE_FUNC void SetCamera(dtCore::BaseActorObject* cameraProxy);

      /** Deprecated 5/23/2011. Call GetChild() instead. */
      DEPRECATE_FUNC dtCore::DeltaDrawable* GetCamera();

   protected:

      /**
      * Initializes the actor
      */
      virtual void CreateDrawable();

      /**
      * Destructor
      */
      virtual ~TripodActorProxy() {}
   };
}

#endif //DELTA_TRIPOD_ACTOR_PROXY

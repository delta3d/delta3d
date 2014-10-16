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
 * David Guthrie
 * Allen Danklefsen
 * Brad Anderegg
 */
#ifndef _MATERIAL_ACTOR_H_
#define _MATERIAL_ACTOR_H_

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/actorproxy.h>
#include <dtCore/propertymacros.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtUtil/refstring.h>
#include <osg/Node>

namespace dtPhysics
{
   /**
   * This is a simple data actor.  It holds the physics properties for materials.
   * You should create a few of these in your map and then link to them with your physics objects.
   * It holds the restitution, static friction, and dynamic friction coefficients (see
   * the properties for more information).
   */
   class DT_PHYSICS_EXPORT  MaterialActor : public dtCore::DeltaDrawable
   {
   public:

      // Constructor
      MaterialActor();

      /// @return the material def object with all the data.
      const MaterialDef& GetMaterialDef() const;

      /// @return the material def object with all the data.
      MaterialDef& GetMaterialDef();

      ///required by DeltaDrawable
      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

   protected:

      // Destructor
      virtual ~MaterialActor();

   private:

      MaterialDef mMaterial;
      dtCore::RefPtr<osg::Node> mNode;
   };

   /**
   * Proxy for the NxAgeiaMaterialActor. It holds the physics properties for Ageia materials.
   * You should create a few of these in your map and then link to them with your Ageia Actor.
   * It holds the restitution, static friction, and dynamic friction coefficients (see
   * the properties for more information).
   */
   class DT_PHYSICS_EXPORT  MaterialActorProxy : public dtCore::ActorProxy
   {
   public:
      static const dtUtil::RefString PROPERTY_KINETIC_FRICTION;
      static const dtUtil::RefString PROPERTY_STATIC_FRICTION;
      static const dtUtil::RefString PROPERTY_RESTITUTION;
      static const dtUtil::RefString PROPERTY_KINETIC_ANISOTROPIC_FRICTION;
      static const dtUtil::RefString PROPERTY_STATIC_ANISOTROPIC_FRICTION;
      static const dtUtil::RefString PROPERTY_DIRECTION_OF_ANISOTROPY;
      static const dtUtil::RefString PROPERTY_ENABLE_ANISOTROPIC_FRICTION;
      static const dtUtil::RefString PROPERTY_DISABLE_STRONG_FRICTION;
      static const dtUtil::RefString PROPERTY_MATERIAL_INDEX;

      MaterialActorProxy();

      // Adds the properties associated with this actor
      virtual void BuildPropertyMap();

      /**
      * The Material actor is global.
      */
      virtual bool IsPlaceable() const { return false; }

      // Creates the actor
      void CreateActor();

   protected:

      virtual ~MaterialActorProxy();
   };
}

#endif

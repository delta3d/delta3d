/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
 * Copyright (C) 2014, Caper Holdings, LLC
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
#include <dtGame/gameactorproxy.h>
#include <dtCore/propertymacros.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtUtil/refstring.h>
#include <osg/Node>

namespace dtPhysics
{
   /**
   * It holds the physics properties for materials.
   * You should create a few of these in your map and then link to them with your physics object.
   * It holds the restitution, static friction, and dynamic/rolling friction coefficients (see
   * the properties for more information).
   */
   class DT_PHYSICS_EXPORT  MaterialActor : public dtGame::GameActorProxy
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

      MaterialActor();

      // Adds the properties associated with this actor
      /*override*/  void BuildPropertyMap();

      /**
      * The Material actor is global.
      */
      /*override*/  bool IsPlaceable() const { return false; }

      // Creates the actor
      /*override*/ void CreateDrawable();

      /// Registers the material
      /*override*/ void OnEnteredWorld();

      /// Don't want any.
      /*override*/ void BuildActorComponents() {}

      /// @return the material object this actor registered.
      Material* GetMaterial() const;

      /// @return the material def object with all the data.
      const MaterialDef& GetMateralDef() const;

      /// @return the material def object with all the data.
      MaterialDef& GetMateralDef();

   protected:
      MaterialDef mMaterial;
      virtual ~MaterialActor();
   };

   /**
   * This is a simple stub drawable that does nothing.
   */
   class DT_PHYSICS_EXPORT MaterialDrawable : public dtCore::DeltaDrawable
   {
   public:

      // Constructor
      MaterialDrawable();

      ///required by DeltaDrawable
      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

   protected:

      // Destructor
      virtual ~MaterialDrawable();

   private:

      dtCore::RefPtr<osg::Node> mNode;
   };

}

#endif

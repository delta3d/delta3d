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
#include <dtUtil/getsetmacros.h>
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
      static const dtUtil::RefString PROPERTY_ALIASES;
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
      void BuildPropertyMap() override;

      /**
      * The Material actor is global.
      */
      bool IsPlaceable() const override { return false; }

      // Creates the actor
      void CreateDrawable() override;

      /// Registers the material
      void OnEnteredWorld() override;

      void OnRemovedFromWorld() override;

      /// @return the material object this actor registered.
      Material* GetMaterial() const;

      /// @return the material def object with all the data.
      const MaterialDef& GetMaterialDef() const;

      /// @return the material def object with all the data.
      MaterialDef& GetMaterialDef();

      DT_DECLARE_ARRAY_ACCESSOR(std::string, Alias, Aliases);
   protected:
      MaterialDef mMaterial;
      virtual ~MaterialActor();
   };

   typedef dtCore::RefPtr<MaterialActor> MaterialActorPtr;
}

#endif

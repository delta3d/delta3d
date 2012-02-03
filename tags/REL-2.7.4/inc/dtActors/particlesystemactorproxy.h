/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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

#ifndef DELTA_PARTICLE_SYSTEM_ACTOR_PROXY
#define DELTA_PARTICLE_SYSTEM_ACTOR_PROXY

#include <dtCore/plugin_export.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/particlesystem.h>
#include <dtCore/exceptionenum.h>

namespace dtActors
{
   /**
    * @class ParticleSystemActorProxy
    * @brief This proxy wraps the ParticleSystem Delta3D object.
    */
   class DT_PLUGIN_EXPORT ParticleSystemActorProxy : public dtCore::TransformableActorProxy
   {
   public:

      /**
       * Constructor
       */
      ParticleSystemActorProxy() { SetClassName("dtCore::ParticleSystem"); }

      /**
       * Adds the properties that are common to all Delta3D particle system objects.
       */
      virtual void BuildPropertyMap();

      /**
       * Loads in a particle system file
       * @param fileName The particle system file to load
       */
      void LoadFile(const std::string &fileName)
      {
         dtCore::ParticleSystem* ps = dynamic_cast<dtCore::ParticleSystem*>(GetActor());
         if (!ps)
         {
            throw dtCore::InvalidActorException(
            "Actor should be type dtCore::ParticleSystem", __FILE__, __LINE__);
         }

         if (!ps->LoadFile(fileName))
         {
            LOG_ERROR("Could not load the file" + fileName);
         }
      }

      /**
       * Gets the method by which a particle system is rendered.
       * @return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON.
       */
      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
      }

      /**
       * Gets the billboard used to represent particle systems.
       * @return
       */
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

   protected:

      /**
       * Creates a particle system actor.
       */
      virtual void CreateActor();

      /**
       * Destructor
       */
      virtual ~ParticleSystemActorProxy() { }
   };
}

#endif

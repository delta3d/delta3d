/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, MOVES Institute
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
 * Erik Johnson
 */

#ifndef shaderactorcomponent_h__
#define shaderactorcomponent_h__

#include <dtGame/export.h>
#include <dtGame/actorcomponentbase.h>
#include <dtDAL/resourcedescriptor.h>

namespace dtGame
{

   /** 
    * ActorComponent used to apply a shader to a GameActor.
    * Adds the "CurrentShader" ResourceActorProperty to the parent GameActor which
    * is tied to SetCurrentShader()/GetCurrentShader().
    */
   class DT_GAME_EXPORT ShaderActorComponent : public dtGame::ActorComponent
   {
   public:
      static const ActorComponent::ACType SHADER_ACTOR_COMPONENT_TYPE;

   	ShaderActorComponent();

      virtual void OnAddedToActor(dtGame::GameActor& actor);
      virtual void OnRemovedFromActor(dtGame::GameActor& actor);
      virtual void BuildPropertyMap();

      /** 
       * Apply the supplied pixel shader resource to the parent GameActor.
       * @param shaderResource The pixel shader resource to apply.  Uses dtDAL::Project
       * to resolve the actual filename.
       */
      virtual void SetCurrentShader(const dtDAL::ResourceDescriptor& shaderResource);

      /** 
       * Get the currently applied shader as the ResourceDescriptor.
       * @return The currently applied shader ResourceDescriptor.  Could be empty
       * if none has been applied.
       */
      const dtDAL::ResourceDescriptor GetCurrentShader() const;

   protected:
      virtual ~ShaderActorComponent();
   	
   private:
      dtDAL::ResourceDescriptor mCurrentShaderResource;
   };

}
#endif // shaderactorcomponent_h__

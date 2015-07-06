/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "testapputils.h"
#include "testappactorregistry.h"
#include <dtActors/engineactorregistry.h>
#include <dtActors/gamemeshactor.h>
#include <dtAnim/animactorregistry.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/object.h>
#include <dtCore/resourcedescriptor.h>



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   bool TestAppUtils::GenerateTangentsForObject(dtCore::BaseActorObject& actor) const
   {
      dtCore::DeltaDrawable* drawable = actor.GetDrawable();

      if (drawable == NULL)
      {
         LOG_WARNING("Could not access drawable to create tangents for actor: "
            + actor.GetName());
         return false;
      }
      return drawable->GenerateTangents();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool TestAppUtils::IsAttachableActor(dtCore::BaseActorObject& actor) const
   {
      bool result = false;

      const dtCore::ActorType* actorType = &actor.GetActorType();

      if(actorType == dtAnim::AnimActorRegistry::ANIMATION_ACTOR_TYPE.get())
      {
         result = true;
      }
      else if (actorType == dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE.get())
      {
         // This would be the perfect case to just be able to add a property to an actor.
         dtActors::GameMeshActor& meshActor = static_cast<dtActors::GameMeshActor&>(actor);

         dtCore::ResourceDescriptor res = meshActor.GetDrawable<dtCore::Object>()->GetMeshResource();
         const std::string& resName = res.GetDisplayName();

         // Search the resource descriptor string for a hint about the model.
         if (resName.find("vehicle") != std::string::npos)
         {
            result = true;
         }
      }
      else if(actorType == dtActors::EngineActorRegistry::BEZIER_CONTROLLER_ACTOR_TYPE.get())
      {
         const std::string& resName = actor.GetName();

         if (resName.find("Camera") != std::string::npos)
         {
            result = true;
         } 
      }
      
      return result;
   }

}

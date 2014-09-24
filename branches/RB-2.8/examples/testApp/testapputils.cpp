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
#include "meshobjectactor.h"
#include <dtActors/engineactorregistry.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/resourcedescriptor.h>
#include <dtUtil/geometrycollector.h>
#include <dtUtil/log.h>
#include <osg/Geometry>
#include <osgUtil/TangentSpaceGenerator>



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

      return GenerateTangentsForObject(*drawable);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool TestAppUtils::GenerateTangentsForObject(dtCore::DeltaDrawable& drawable) const
   {
      bool success = false;

      // Get all geometry in the graph to apply the shader to
      osg::ref_ptr<dtUtil::GeometryCollector> geomCollector = new dtUtil::GeometryCollector;
      drawable.GetOSGNode()->accept(*geomCollector);

      // Calculate tangent vectors for all faces and store them as vertex attributes
      for (size_t geomIndex = 0; geomIndex < geomCollector->mGeomList.size(); ++geomIndex)
      {
         osg::Geometry* geom = geomCollector->mGeomList[geomIndex];

         // Force display lists to OFF and VBO's to ON so that vertex
         // attributes can be set without disturbing the graphics driver
         geom->setSupportsDisplayList(false);
         geom->setUseDisplayList(false);
         geom->setUseVertexBufferObjects(true);

         osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
         tsg->generate(geom, 0);
         osg::Array* tangentArray = tsg->getTangentArray();

         if ( ! geom->getTexCoordArray(1))
         {
            if (tangentArray != NULL)
            {
               geom->setTexCoordArray(1, tangentArray);

               success = true;
            }
            else
            {
               LOG_WARNING("Could not generate tangent space for object: " + drawable.GetName()
                  + " - Geometry: " + geom->getName());
            }
         }
         else
         {
            LOG_INFO("Tangent space data may already exist for object: " + drawable.GetName()
               + " - Geometry: " + geom->getName());
         }
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool TestAppUtils::IsAttachableActor(dtCore::BaseActorObject& actor) const
   {
      bool result = false;

      const dtCore::ActorType* actorType = &actor.GetActorType();

      if(actorType == TestAppActorRegistry::CIVILIAN_ACTOR_TYPE.get()
         || actorType == TestAppActorRegistry::VESSEL_ACTOR_TYPE.get())
      {
         result = true;
      }
      else if (actorType == TestAppActorRegistry::MESH_OBJECT_ACTOR_TYPE.get())
      {
         MeshObjectActor& meshActor = static_cast<MeshObjectActor&>(actor);

         dtCore::ResourceDescriptor res = meshActor.GetMeshResource();
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

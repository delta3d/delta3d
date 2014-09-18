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

///////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
///////////////////////////////////////////////////////////////////////
#include "testapputils.h"
#include <dtCore/deltadrawable.h>
#include <dtUtil/geometrycollector.h>
#include <dtUtil/log.h>
#include <osg/Geometry>
#include <osgUtil/TangentSpaceGenerator>



namespace dtExample
{
   ////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////
   bool TestAppUtils::GenerateTangentsForObject(dtCore::BaseActorObject& actor)
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

   bool TestAppUtils::GenerateTangentsForObject(dtCore::DeltaDrawable& drawable)
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

         if ( ! geom->getVertexAttribArray(6))
         {
            if (tangentArray != NULL)
            {
               geom->setVertexAttribArray(6, tangentArray);

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

}

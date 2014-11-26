/* -*-c++-*-
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

#ifndef DELTA_TRANSFORMABLE_ACTOR_PROXY
#define DELTA_TRANSFORMABLE_ACTOR_PROXY

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Node>

#include <dtCore/actorproxy.h>
#include <dtCore/export.h>
#include <dtCore/resourcedescriptor.h>

#include <dtUtil/getsetmacros.h>

namespace dtCore
{
   /**
    * This proxy wraps the Transformable Delta3D object.
    */
   class DT_CORE_EXPORT TransformableActorProxy : public dtCore::BaseActorObject
   {
      public:

         static const dtUtil::RefString PROPERTY_ROTATION;
         static const dtUtil::RefString PROPERTY_TRANSLATION;
         static const dtUtil::RefString PROPERTY_NORMAL_RESCALING;
         static const dtUtil::RefString PROPERTY_RENDER_PROXY_NODE;

         /**
          * Constructor
          */
         TransformableActorProxy();

         /**
          * Adds the properties that are common to all Delta3D transformable objects.
          */
         virtual void BuildPropertyMap();

         /**
          * Transformables can be placed in a scene
          */
         virtual bool IsPlaceable() const { return true; }

         /**
         * Determines if the translation should be shown in the actor properties.
         */
         virtual bool IsTranslationPropertyShown() const { return true; }

         /**
         * Determines if the rotation should be shown in the actor properties.
         */
         virtual bool IsRotationPropertyShown() const { return true; }

         /**
          * Sets the rotation of an object.
          * @param rotation the desired rotation
          */
         void SetRotation(const osg::Vec3 &rotation);

         void SetRotationFromMatrix(const osg::Matrix &rotation);

         /**
          * Gets the rotation of an object
          * @return The current rotation vector
          */
         osg::Vec3 GetRotation() const;

         /**
          * Sets the translation of an object
          * @param translation the desired translation
          */
         void SetTranslation(const osg::Vec3 &translation);

         /**
          * Gets the translation of an object
          * @return The current translation
          */
         osg::Vec3 GetTranslation() const;

         /**
          * Called when the SetRotation function is called.  The rotation will
          * be passed in using x,y,z order (p,r,h)
          * @param oldValue The previous value
          * @param newValue The new value
          */
         virtual void OnRotation(const osg::Vec3& /*oldValue*/, const osg::Vec3& /*newValue*/) { }

         /**
          * Called when the SetTranslation function is called
          * @param oldValue The previous value
          * @param newValue The new value
          */
         virtual void OnTranslation(const osg::Vec3& /*oldValue*/, const osg::Vec3& /*newValue*/) { }




      protected:

         /**
          * Destructor
          */
         virtual ~TransformableActorProxy() {}

      private:
   };
}

#endif

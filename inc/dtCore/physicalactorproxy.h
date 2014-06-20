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
#ifndef _PHYSICAL_ACTOR_PROXY_H_
#define _PHYSICAL_ACTOR_PROXY_H_

#include <dtCore/export.h>
#include <dtCore/transformableactorproxy.h>

namespace dtCore
{
   /**
    * This proxy wraps the Physical Delta3D object.
    */
   class DT_CORE_EXPORT PhysicalActorProxy : public dtCore::TransformableActorProxy
   {
      public:

         static const dtUtil::RefString PROPERTY_ENABLE_DYNAMICS;
         static const dtUtil::RefString PROPERTY_MASS;
         
         /**
          * Constructor
          */
         PhysicalActorProxy()
         {
            SetClassName("dtCore::Physical");
         }

         /**
          * Adds the properties that are common to all Delta3D physical objects.
          */
         virtual void BuildPropertyMap();

         /**
          * Sets the mass of an object
          * @param mass the desired mass
          */
         void SetMass(float mass);

         /**
          * Gets the mass of an object
          * @return The current mass
          */
         float GetMass() const;

         /**
          * Sets the center of gravity of an object
          * @param g the desired center of gravity
          */
         void SetCenterOfGravity(const osg::Vec3 &g);

         /**
          * Gets the center of gravity of an object
          * @return The current center of gravity
          */
         osg::Vec3 GetCenterOfGravity() const;

      protected:

         /**
          * Destructor
          */
         virtual ~PhysicalActorProxy() {}

   };
}

#endif

/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 */
#ifndef DELTA_DAL_ENVIRONMENT_ACTOR
#define DELTA_DAL_ENVIRONMENT_ACTOR

#include <dtCore/baseactorobject.h>
#include <dtCore/export.h>
#include <dtUtil/referencedinterface.h>
#include <dtCore/deltadrawable.h>

namespace dtCore
{
   /** Interface class to an "environment actor".
   */
   class DT_CORE_EXPORT IEnvironmentActor : public dtUtil::ReferencedInterface
   {
      public:

         /// Constructor
         IEnvironmentActor();

         /// Destructor
         virtual ~IEnvironmentActor();

         /**
          * Adds a DeltaDrawable to the internal hierarchy of the environment
          * @param dd The DeltaDrawable to add as a child
          */
         virtual void AddActor(dtCore::DeltaDrawable& dd) = 0;

         /**
          * Removes a DeltaDrawable from the internal hierarchy
          * @param dd The DeltaDrawable to remove
          */
         virtual void RemoveActor(dtCore::DeltaDrawable& dd) = 0;

         /**
          * Removes all actors associated with this environment
          */
         virtual void RemoveAllActors() = 0;

         /**
          * Called to see if this environment has the specified proxy
          * @param proxy The proxy to look for
          * @return True if it contains it, false if not
          */
         virtual bool ContainsActor(dtCore::DeltaDrawable& dd) const = 0;

         /**
          * Gets all the actors associated with this environment
          * @param vec The vector to fill
          */
         virtual void GetAllActors(dtCore::DeltaDrawable::DrawableList& vec) = 0;

   
         /**
          * Gets the number of children of this environment
          */
         virtual unsigned int GetNumEnvironmentChildren() const = 0;
   };
}

#endif

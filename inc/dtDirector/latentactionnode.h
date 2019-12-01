/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTOR_LATENT_ACTION_NODE
#define DIRECTOR_LATENT_ACTION_NODE

#include <dtDirector/actionnode.h>
#include <dtDirector/export.h>


namespace dtDirector
{
   /**
    * This is the base class for all latent action nodes.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT LatentActionNode : public ActionNode
   {
   public:

      /**
       * Constructs the Node.
       */
      LatentActionNode();

      /**
       * Retrieves the action node, if valid.
       */
      virtual LatentActionNode* AsLatentNode();

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~LatentActionNode();

   private:

   };
}

#endif

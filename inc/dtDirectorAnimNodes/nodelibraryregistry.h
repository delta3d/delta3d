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

#ifndef ANIM_NODE_LIBRARY_REGISTRY
#define ANIM_NODE_LIBRARY_REGISTRY

#include <dtDirector/nodepluginregistry.h>
#include <dtDirectorAnimNodes/nodelibraryexport.h>

////////////////////////////////////////////////////////////////////////////////
namespace dtDirector
{
   /**
    * Class that exports the applicable nodes to a library
    */
   class ANIM_NODE_LIBRARY_EXPORT NodeLibraryRegistry : public dtDirector::NodePluginRegistry
   {
   public:

      /// Events

      /// Actions
      static dtCore::RefPtr<dtDirector::NodeType> ANIMATE_ACTOR_ACTION_NODE_TYPE;

      /// Values

      /**
       *	Constructor.
       */
      NodeLibraryRegistry();

      /**
       *	Registers all of the nodes to be exported.
       */
      void RegisterNodeTypes();

      /**
       * Retrieves the type of node library this is.
       * @note: This should be overloaded if the nodes that belong
       * to the library are of a specific type.
       */
      virtual std::string GetNodeLibraryType() const
      {
         return "Core";
      }

   private:
   };
}

#endif // ANIM_NODE_LIBRARY_REGISTRY

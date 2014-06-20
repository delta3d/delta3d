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
#ifndef DELTA_OBJECT_ACTOR_PROXY
#define DELTA_OBJECT_ACTOR_PROXY

#include <dtCore/plugin_export.h>
#include <dtCore/physicalactorproxy.h>

namespace dtActors 
{
   /**
   * @class DeltaObjectActorProxy
   * @brief This proxy wraps the Delta3D Object.
   */
   class DT_PLUGIN_EXPORT DeltaObjectActorProxy : public dtCore::PhysicalActorProxy 
   {
      public:

         /**
         * Constructor
         */
         DeltaObjectActorProxy() { SetClassName("dtCore::Object"); }

         /**
         * Adds the properties that are common to all Delta3D physical objects.
         */
         virtual void BuildPropertyMap();

//         /**
//         * This method is responsible for loading a mesh file and any other
//         * associated data.  This method is abstract to ensure that more specific
//         * classes are created from this one.  A Delta3D object is very generic
//         * therefore, even if the subclass does something as simple as loading a mesh,
//         * at least it can be identified in a more concrete manor.
//         * @param fileName The path to the file to load.
//         * @note
//         *  This method could be as simple as merely calling the dtCore::Object::LoadFile,
//         *  or could be complicated.  For an example of a more detailed LoadFile
//         *  implementation see the StaticMeshActorProxy.
//         * @see StaticMeshActorProxy
//         */
//         virtual void LoadFile(const std::string& fileName) = 0;

      protected:
        
         /**
         * Creates a Delta3D object actor.
         */
         virtual void CreateDrawable();

         /**
         * Destructor
         */
         virtual ~DeltaObjectActorProxy() {}
   };
}

#endif

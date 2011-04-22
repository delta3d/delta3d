/* 
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

#ifndef DELTA_OBJECT
#define DELTA_OBJECT

// object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/loadable.h>
#include <dtCore/physical.h>

namespace dtCore
{
   /** 
    * A visual Object with physical properties. The Object represents a 
    * virtual object which is renderable, movable, and has physical properties.
    */
   class DT_CORE_EXPORT Object : public Loadable, public Physical
   {
      DECLARE_MANAGEMENT_LAYER(Object)
         
      public:
      
         Object(const std::string& name = "Object");

         /** Overloaded constructor will use the supplied node instead of
           * creating one internally.
           * @note: Calling LoadFile will overwrite the supplied node with whatever is loaded.
           * @param node : A node this class should use internally
           * @param name : The name of this instance
           */
         Object( TransformableNode &node, const std::string &name = "Object" );

      protected:

         virtual ~Object();

      public:
                  
         ///Load a file from disk
         virtual osg::Node* LoadFile(const std::string& filename, bool useCache = true);

         ///recenters the object geometry on LoadFile
         void RecenterGeometryUponLoad( const bool enable = true ) { mRecenterGeometry = enable; }

      private:

         void Ctor();

         bool mRecenterGeometry;///<if we want to recenter the geometry of the object about the origin upon load
   };   
}

#endif // DELTA_OBJECT
/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_DELTA_DRAWABLE
#define DELTA_DELTA_DRAWABLE

// deltadrawable.h: Declaration of the DeltaDrawable class.
//
//////////////////////////////////////////////////////////////////////


#include "dtCore/export.h"
#include "osg/Node"

namespace dtCore
{
   class Scene;
   
   /**
    * A drawable object.
    */
   class DT_EXPORT DeltaDrawable
   {
      public:

         /**
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode() = 0;
         
         /**
          * Notifies this drawable object that it has been added to
          * a scene.
          *
          * @param scene the scene to which this drawable object has
          * been added
          */
         virtual void AddedToScene( Scene* scene ) {}
         
   };

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   typedef DeltaDrawable Drawable;
#endif
};


#endif // DELTA_DELTA_DRAWABLE

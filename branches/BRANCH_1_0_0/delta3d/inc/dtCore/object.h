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

#ifndef DELTA_OBJECT
#define DELTA_OBJECT

// object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////



#include "dtCore/base.h"
#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include "dtCore/notify.h"
#include "dtCore/transformable.h"
#include "dtCore/deltadrawable.h"
#include "dtCore/physical.h"

namespace dtCore
{
   
   ///A visual Object

 /** The Object represents a visual object in the Scene.  Once instantiated, 
   * a pre-built geometry file can be loaded using LoadFile().  The file specified
   * is searched for using the paths supplied to SetDataFilePathList().
   * Since Object is
   * derived from Transform, it may be positioned and queried using any of 
   * Transform's methods.
   *
   * The Object must be added to a Scene to be viewed using Scene::AddObject().
   */
   class DT_EXPORT Object : public Transformable, public DeltaDrawable, public Physical
   {
      DECLARE_MANAGEMENT_LAYER(Object)
         
      public:
         Object(std::string name = "Object");
         virtual ~Object();
         
         ///Get a handle to the OSG Node
         virtual osg::Node * GetOSGNode(void) {return mNode.get();}
         
         ///Load a file from disk
         virtual  bool LoadFile( std::string filename, bool useCache = true);

         ///Get the filename of the last loaded file
         virtual  std::string GetFilename(void) const {return mFilename;}
         
      protected:
         osg::ref_ptr<osg::MatrixTransform> mNode; ///<Contains the actual model
         std::string mFilename; ///<The filename of the last file loaded
   };
   
};


#endif // DELTA_OBJECT

/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2007 MOVES Institute
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
* Michael Guerrero 
*/

#ifndef __POSE_MESH_DATABASE_H__
#define __POSE_MESH_DATABASE_H__

#include "export.h"

#include <string>
#include <vector>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/Referenced>
#include <osg/ref_ptr>

namespace dtAnim 
{
   class Cal3DModelWrapper; 
   class PoseMesh;

   /// manager of the HotSpotData resources
   class DT_ANIM_EXPORT PoseMeshDatabase: public osg::Referenced
   {
   public:
      typedef std::vector<PoseMesh*> PoseMeshList;     

      PoseMeshDatabase( dtAnim::Cal3DModelWrapper *model );
      ~PoseMeshDatabase();

      /*
      * GetPoseMeshByName Get a pointer to a loaded pose mesh
      * @param name the name of the pose mesh whose pointer is wanted
      * @return a pointer to pose mesh with a matching name
      */
      PoseMesh* GetPoseMeshByName( const std::string& name );
      
      PoseMeshList& GetMeshes() { return mMeshes; }

      bool LoadFromFile( const std::string& file );      

   private:
      PoseMeshList mMeshes;
      osg::ref_ptr<dtAnim::Cal3DModelWrapper> mModel;    
   };
}

#endif // __POSE_MESH_DATABASE_H__

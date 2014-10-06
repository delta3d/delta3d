/* -*-c++-*-
* dtPhysics
* Copyright 2007-2010, Alion Science and Technology
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Bradley Anderegg
*/


#ifndef DELTA_PHYSICSREADERWRITER_H
#define DELTA_PHYSICSREADERWRITER_H

#include <dtPhysics/physicsexport.h> 
#include <osg/Array>
#include <string>
#include <vector>

#include <dtCore/refptr.h>

namespace dtPhysics
{
   class DT_PHYSICS_EXPORT PhysicsReaderWriter
   {
      public:
         struct PhysicsTriangleData
         {
            dtCore::RefPtr<osg::Vec3Array> mVertices;
            dtCore::RefPtr<osg::UIntArray> mFaces;
            dtCore::RefPtr<osg::UIntArray> mMaterialFlags;
         };


      public:

         static bool LoadTriangleDataFile(PhysicsTriangleData& triangleData, const std::string& filename);
         static bool SaveTriangleDataFile(const PhysicsTriangleData& triangleData, const std::string& filename);

      private: 

   };


} // namespace dtPhysics

#endif // DELTA_PHYSICSREADERWRITER_H

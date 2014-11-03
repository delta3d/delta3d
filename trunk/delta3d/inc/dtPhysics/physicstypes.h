/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * Bradley Anderegg
 */
#ifndef DELTA_PHYSICS_TYPES
#define DELTA_PHYSICS_TYPES

#include <dtPhysics/physicsexport.h>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>
//#include <osg/Array>
#include <dtCore/transform.h>
#include <string>

// TODO there must be a better way to get the materials...
//#include <dtUtil/mswin.h>
//#include <pal/pal.h>

class palMaterialInteractionCollisionCallback;
class palMaterial;
class palAction;
class palDebugDraw;

namespace dtPhysics
{
   typedef osg::Vec3 VectorType;
   typedef osg::Vec4 Vector4Type;
   typedef osg::Matrix MatrixType;
   typedef dtCore::Transform TransformType;
   //typedef DT_PHYSICS_EXPORT osg::Vec3Array VectorArrayType;

   typedef float Real;

   typedef int CollisionGroup;
   typedef unsigned long CollisionGroupFilter;

   typedef palMaterial Material;
   typedef unsigned MaterialIndex;
   typedef DT_PHYSICS_EXPORT palMaterialInteractionCollisionCallback MaterialInteractionCollisionCallback;

   typedef palDebugDraw DebugDraw;

}

#endif //DELTA_PHYSICS_INTERFACE

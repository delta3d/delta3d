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
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/model.h>
#include <dtCore/transform.h>

#include <osg/Vec3>

namespace dtCore
{

   /////////////////////////////////////////////////////////////////////////////
   Model::Model():
      mModelTransform(new osg::MatrixTransform),
      mScale(1.0f, 1.0f, 1.0f)
   {
      SetDirty();
   }

   /////////////////////////////////////////////////////////////////////////////
   Model::~Model()
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::SetDirty()
   {
      dtCore::Transform xform;
      GetTransform(xform);
      xform.Rescale(mScale);
      UpdateMatrixTransform(xform);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::SetScale(const osg::Vec3& modelScale)
   {
      mScale = modelScale;
      SetDirty();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::GetScale(osg::Vec3& modelScale) const
   {
      modelScale = mScale;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::SetTransform(const dtCore::Transform& xform)
   {
      UpdateMatrixTransform(xform);
      //dirty so we can re-apply the scale.
      SetDirty();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::GetTransform(dtCore::Transform& xform) const
   {
      xform.Set(mModelTransform->getMatrix());
   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::UpdateMatrixTransform(const dtCore::Transform &xform)
   {
      //two copies?  Not good.
      osg::Matrix m;
      xform.Get(m);
      mModelTransform->setMatrix(m);
   }
}

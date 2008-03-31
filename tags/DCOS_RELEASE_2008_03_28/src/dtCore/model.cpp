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
   class ModelMatrixUpdateCallback : public osg::NodeCallback
   {
      public:

         ModelMatrixUpdateCallback(osg::Vec3& scale):
            mScale(scale)
         {

         }

         /** Callback method called by the NodeVisitor when visiting a node.*/
         virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
         {
            osg::MatrixTransform* modelTransform = static_cast<osg::MatrixTransform*>(node);
            dtCore::Transform xform;
            osg::Matrix m;
            xform.Set(modelTransform->getMatrix());
            xform.Rescale(mScale);
            xform.Get(m);
            modelTransform->setMatrix(m);
            traverse(node,nv);

            node->setUpdateCallback(NULL);
         }

      private:

         osg::Vec3& mScale;
   };

   /////////////////////////////////////////////////////////
   Model::Model():
      mModelTransform(new osg::MatrixTransform),
      mScale(1.0f, 1.0f, 1.0f),
      mUpdateCallback(new ModelMatrixUpdateCallback(mScale))
   {
      SetDirty();
   }

   /////////////////////////////////////////////////////////
   Model::~Model()
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::SetDirty()
   {
      mModelTransform->setUpdateCallback(mUpdateCallback.get());
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
      //two copies?  Not good.
      osg::Matrix m;
      xform.Get(m);
      mModelTransform->setMatrix(m);
      //dirty so we can re-apply the scale.
      SetDirty();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Model::GetTransform(dtCore::Transform& xform) const
   {
      xform.Set(mModelTransform->getMatrix());
   }
}

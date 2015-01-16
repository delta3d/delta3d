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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/basemodelwrapper.h>
#include <dtUtil/mathdefines.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   BaseModelWrapper::BaseModelWrapper(dtAnim::BaseModelData& modelData)
      : mModelData(&modelData)
   {}

   BaseModelWrapper::~BaseModelWrapper()
   {}

   dtAnim::BaseModelData* BaseModelWrapper::GetModelData() const
   {
      return mModelData.get();
   }

   void BaseModelWrapper::HandleModelUpdated()
   {}

   void BaseModelWrapper::UpdateScale()
   {
      osg::Node* node = GetDrawableNode();

      float scale = GetScale();

      if (!dtUtil::Equivalent(scale, 1.0f, 0.001f) && node != NULL && ! mScaleTransform.valid())
      {
         mScaleTransform = new osg::MatrixTransform;
         mScaleTransform->setName("Scale Transform");
         mScaleTransform->addChild(node);
      }

      if (mScaleTransform.valid())
      {
         osg::Matrix mtx(osg::Matrix::identity());
         mtx.makeScale(scale, scale, scale);
         mScaleTransform->setMatrix(mtx);
      }
   }

   osg::MatrixTransform* BaseModelWrapper::GetScaleTransform() const
   {
      return mScaleTransform.get();
   }
}

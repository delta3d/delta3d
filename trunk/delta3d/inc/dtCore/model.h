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
#ifndef DELTA_MODEL
#define DELTA_MODEL

#include <dtCore/export.h>
#include <dtCore/refptr.h>

#include <osg/MatrixTransform>
#include <osg/Referenced>

namespace dtCore
{
   class Transform;

   class DT_CORE_EXPORT Model : public osg::Referenced
   {
      public:

         /**
          * Constructor
          */
         Model();

         /**
          * Destructor
          */
         virtual ~Model();

         /**
          * Sets the transform of the model
          * @param xform The transform
          */
         void SetTransform(const dtCore::Transform& xform);


         /**
          * Returns the transform of the model
          * @param xform The Transform to fill in
          */
         void GetTransform(dtCore::Transform& xform) const;

         /**
          * Sets the scale of the model
          * @param modelScale The scale
          */ 
         void SetScale(const osg::Vec3& modelScale);

         /**
          * Returns the model transform
          * @return mModelTransform
          */
         osg::MatrixTransform& GetMatrixTransform() { return *mModelTransform; }

         /**
          * Returns the model transform
          * @return mModelTransform
          */
         const osg::MatrixTransform& GetMatrixTransform() const { return *mModelTransform; }

         /**
          * Returns the scale of the model
          * @param modelScale The Vec3 to fill in
          */
         void GetScale(osg::Vec3& modelScale) const;

         /**
          * Tells this object that the scale vector has been changed, so that it
          * can be reapplied. This operation can take a couple hundredths of a millisecond
          * and is best not called every frame.
          * This is called automatically by SetModelScale and SetModelTransform, but 
          * must be called explicitly if one modifies the model transform directly by calling
          * GetModelTransform().setMatrix();
          */
         void SetDirty();

      private:
         void UpdateMatrixTransform(const dtCore::Transform &xform);

         dtCore::RefPtr<osg::MatrixTransform> mModelTransform;
         osg::Vec3 mScale;
   };
}

#endif

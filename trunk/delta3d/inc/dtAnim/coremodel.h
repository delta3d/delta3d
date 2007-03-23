/*  -*- c++ -*-
Copyright (C) 2003 <ryu@gpul.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

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

#ifndef __dtAnim__CORE_MODEL_H__
#define __dtAnim__CORE_MODEL_H__

#include <vector>
#include <osg/Texture2D>
#include <osg/Referenced>
#include <dtAnim/export.h>
#include <cal3d/cal3d.h>

namespace dtAnim {

   ///Adapter that enables reference-counting of core models

   /** 
   * A core model is a reference (a template) for creating real models. This
   * allows sharing lot of data, like the base geometry (the geometry before
   * deforming any vertex)
   */
   class DT_ANIM_EXPORT CoreModel: public osg::Object {
   public:
      CoreModel();

      CoreModel(const std::string& name);

      virtual osg::Object* cloneType() const { return new CoreModel("dummy"); }

      /** Clone the an object, with Object* return type.
      Must be defined by derived classes.*/
      virtual osg::Object* clone(const osg::CopyOp&) const;

      virtual bool isSameKindAs(const osg::Object *obj) const { return dynamic_cast<const CoreModel*>(obj) != 0; }

      /** return the name of the object's library. Must be defined
      by derived classes. The OpenSceneGraph convention the is
      that the namspace of a library is the same as the library name.*/
      virtual const char* libraryName() const { return "dtAnim"; }

      /** return the name of the object's class type. Must be defined
      by derived classes.*/
      virtual const char* className() const { return "CoreModel"; }

      /**
      * Loads the skeleton of the model
      */
      void LoadSkeleton(std::string skeleton_file);

      /**
      * Loads one animation. You can load several.
      */
      void LoadAnimation(std::string animation_file);

      /**
      * Loads one animation and associate a string with this anim. You can load several.
      */
      void LoadAnimation(std::string animation_file, std::string animation_name);

      /**
      * Retrieves the ID of the animation referenced by a string.
      */
      int GetAnimationId(std::string animation_name) const;

      /**
      * Loads one mesh. You can load several.
      */
      void LoadMesh(std::string mesh_file);

      /**
      * Loads one material. You can load several, in fact you can load
      * several for each mesh, as each mesh can have submeshes.
      */
      void LoadMaterial(std::string material_file);

      /** Loads all the textures referenced by some material. You can
      * specify some directory, if not, current will be used */
      void LoadAllTextures(std::string directory=".");

      /** Loads all the textures referenced by some material. You must
      * specify the directory of the textures and the materialId */
      void LoadMaterialTextures(std::string directory, int materialId);

      inline ~CoreModel() { /*coreModel.destroy();*/ }

      /* Get a pointer to the real core model, *DONT* keep pointers to
      * it. Instead, keep references to this class (dtAnim::CoreModel). */
      CalCoreModel *get() { return mCalCoreModel; }

      std::string GetSkeletonFileName() const { return mSkeleton; }

      const std::vector<std::pair<std::string, std::string> > GetAnimationFileNames() const { return mAnimations; }

      const std::vector<std::string> GetMeshFileNames() const { return mMeshes; }

      const std::vector<std::string> GetMaterialFileNames() const { return mMaterials; }

   private:
      CalCoreModel *mCalCoreModel;
      /** List of textures loaded by this class, to destroy them when this class
      * is destroyed. */
      std::vector< osg::ref_ptr<osg::Texture2D> > mTextures;

      std::string mSkeleton;
      std::vector<std::pair<std::string, std::string> > mAnimations;
      std::vector< std::string > mMeshes;
      std::vector< std::string > mMaterials;
   };

}; // namespace dtAnim

#endif

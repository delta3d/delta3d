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
 * David Guthrie
 */

#ifndef DELTA_SUBMESH_H__
#define DELTA_SUBMESH_H__


#include <dtAnim/export.h>
#include <dtAnim/cal3dmodeldata.h>

#include <dtCore/refptr.h>

#include <osg/Geometry>

namespace dtAnim
{

   class Cal3DModelWrapper;

   ///Adapter that converts cal3d submeshes into osg::Drawables

   /**
    * The easy way would be to draw all the character (CalModel) in a single
    * Drawable, but this approach lacks from state sorting. Each submesh of
    * each mesh of a model can have different state attributes. With the current
    * approach, if you have 1000 soldiers with two different textures each one,
    * there will be only two state changes per frame, and not 2000.
    *
    * Users of the osgCal library doesn't need to know about this class, it is
    * internal.
    */
   class DT_ANIM_EXPORT SubmeshDrawable: public osg::Drawable {
   public:
      static const unsigned LOD_COUNT = 4;

      /**
       * Creates a submesh for one model given the mesh and submesh of this mesh
       */
      SubmeshDrawable(Cal3DModelWrapper *wrapper, unsigned mesh, unsigned submesh);

      /**
       * Draws the geometry.
       */
      virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

      /**
       * Accept PrimitiveVisitor, in this case a TriangleVisitor
       */

      /** Return true, SubMesh does support accept(PrimitiveFunctor&). */
      virtual bool supports(osg::PrimitiveFunctor&) const { return true; }

      /** Accept a PrimitiveFunctor and call its methods to tell it
       * about the interal primitives that this Drawable has. 
       */

      virtual void accept(osg::PrimitiveFunctor& pf) const;

      virtual osg::Object* cloneType() const;
      virtual osg::Object* clone(const osg::CopyOp&) const;

      unsigned int GetMeshID() const { return mMeshID; }
      unsigned int GetSubmeshID() const { return mSubmeshID; }

      Cal3DModelWrapper& GetModelWrapper() { return *mWrapper; }
      const Cal3DModelWrapper& GetModelWrapper() const { return *mWrapper; }

      LODOptions& GetLODOptions() { return mModelData->GetLODOptions(); }
      const LODOptions& GetLODOptions() const { return mModelData->GetLODOptions(); }

      float GetCurrentLOD() const { return mCurrentLOD; }
      void SetCurrentLOD(float lod) { mCurrentLOD = lod; };

      void SetBoundingBox(const osg::BoundingBox& boundingBox) { mBoundingBox = boundingBox; }

   protected:
      ~SubmeshDrawable();

      void InitVertexBuffers(osg::State& state) const;

   private:
      SubmeshDrawable();   ///< not implemented by design
      bool VBOAvailable(const osg::RenderInfo& renderInfo) const;
      void DrawUsingVBO(osg::RenderInfo &renderInfo) const;
      void DrawUsingPrimitives(osg::RenderInfo &renderInfo) const;
      void ClearTheState(osg::State& state) const;

      unsigned mMeshID;
      unsigned mSubmeshID;

      mutable osg::ref_ptr<osg::VertexBufferObject>  mMeshVBO;
      mutable osg::ref_ptr<osg::ElementBufferObject> mMeshEBO;
      mutable unsigned mFaceCount[LOD_COUNT];
      mutable unsigned mFaceOffsets[LOD_COUNT];
      mutable unsigned mVertexCount[LOD_COUNT];
      mutable unsigned mVertexOffsets[LOD_COUNT];
      mutable dtCore::RefPtr<Cal3DModelData> mModelData;

      mutable float mCurrentLOD;
      mutable bool  mInitalized;

      dtCore::RefPtr<Cal3DModelWrapper> mWrapper;

      static const unsigned STRIDE = 10;
      static const unsigned STRIDE_BYTES = STRIDE * sizeof(float);

      osg::BoundingBox mBoundingBox;

      void SetUpMaterial();

      // Old, compatible rendering path
      mutable float* mMeshVertices;
      mutable float* mMeshNormals;
      mutable float* mMeshTextureCoordinates;
      mutable int* mMeshFaces;
   };

   class SubmeshDirtyCallback: public osg::Drawable::UpdateCallback 
   {
   public:
      virtual void update (osg::NodeVisitor*, osg::Drawable* d);
   };


   class SubmeshUserData: public osg::Object
   {
   public:
      typedef osg::Object BaseClass;

      float    mLOD;

      /** Clone the type of an object, with Object* return type.
      Must be defined by derived classes.*/
      virtual osg::Object* cloneType() const;

      /** Clone an object, with Object* return type.
      Must be defined by derived classes.*/
      virtual osg::Object* clone(const osg::CopyOp&) const;

      virtual bool isSameKindAs(const osg::Object*) const;

      /** return the name of the object's library. Must be defined
      by derived classes. The OpenSceneGraph convention is that the
      namespace of a library is the same as the library name.*/
      virtual const char* libraryName() const;

      /** return the name of the object's class type. Must be defined
      by derived classes.*/
      virtual const char* className() const;
   };

} //namespace dtAnim

#endif

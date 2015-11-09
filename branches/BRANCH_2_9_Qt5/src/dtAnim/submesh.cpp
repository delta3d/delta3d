/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 */

#include <dtAnim/submesh.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/lodcullcallback.h>
#include <dtAnim/modeldatabase.h>

#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <osg/Material>
#include <osg/Texture2D>
#include <osg/PolygonMode>
#include <osg/Geometry>
#include <osg/CullFace>
#include <osg/Math>
#include <osg/BlendFunc>
#include <osg/Version>
#include <osg/GLExtensions>

#include <cassert>

namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const unsigned int VBO_OFFSET_POSITION = 0;
   static const unsigned int VBO_OFFSET_NORMAL = 3;
   static const unsigned int VBO_OFFSET_TEXCOORD0 = 6;
   static const unsigned int VBO_OFFSET_TEXCOORD1 = 8;
   static const unsigned int VBO_STRIDE = 10;
   static const unsigned int VBO_STRIDE_BYTES = VBO_STRIDE * sizeof(float);



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class SubmeshComputeBound : public osg::Drawable::ComputeBoundingBoxCallback
   {
   public:
      SubmeshComputeBound(const osg::BoundingBox& boundingBox)
         : mBoundingBox(boundingBox)
      {
      }

      /*virtual*/ osg::BoundingBox computeBound(const osg::Drawable&) const
      {
         // temp until a better solution is implemented
         return mBoundingBox;
      }

      const osg::BoundingBox& mBoundingBox;
   };


   ////////////////////////////////////////////////////////////////////////////////////////
   osg::Object* SubmeshUserData::cloneType() const
   {
      return new SubmeshUserData;
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   osg::Object* SubmeshUserData::clone(const osg::CopyOp& op) const
   {
      SubmeshUserData* theClone = static_cast<SubmeshUserData*>(cloneType());
      theClone->mLOD = mLOD;
      return theClone;
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   bool SubmeshUserData::isSameKindAs(const Object* o) const
   {
      return dynamic_cast<const SubmeshUserData*>(o) != NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   const char* SubmeshUserData::libraryName() const { return "dtAnim"; }

   ////////////////////////////////////////////////////////////////////////////////////////
   const char* SubmeshUserData::className() const { return "SubmeshUserData";}

   ////////////////////////////////////////////////////////////////////////////////////////
   void SubmeshDirtyCallback::update (osg::NodeVisitor*, osg::Drawable* d)
   {
      d->dirtyBound();
   }


   ////////////////////////////////////////////////////////////////////////////////////////
   SubmeshDrawable::SubmeshDrawable(Cal3DModelWrapper* wrapper, unsigned mesh, unsigned Submesh)
      : mMeshID(mesh)
      , mSubmeshID(Submesh)
      , mCurrentLOD(1.0f)
      , mInitalized(false)
      , mWrapper(wrapper)
      , mMeshVertices(NULL)
      , mMeshNormals(NULL)
      , mMeshTextureCoordinates(NULL)
      , mMeshFaces(NULL)
   {
      setUseDisplayList(false);
      setUseVertexBufferObjects(true);
      SetUpMaterial();

      osg::StateSet* ss = getOrCreateStateSet();
      ss->setAttributeAndModes(new osg::CullFace);

      mModelData = mWrapper->GetCalModelData();

      if (!mModelData.valid())
      {
         LOG_ERROR("Model does not have model data.  Unable to cache vertex buffers.");
      }

      //setUpdateCallback(new SubmeshDirtyCallback());
      setCullCallback(new LODCullCallback(*mWrapper, mMeshID));
      setComputeBoundingBoxCallback(new SubmeshComputeBound(mBoundingBox));
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   SubmeshDrawable::~SubmeshDrawable(void)
   {
      delete [] mMeshVertices;
      delete [] mMeshNormals;
      delete [] mMeshFaces;
      delete [] mMeshTextureCoordinates;
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   void SubmeshDrawable::SetUpMaterial()
   {
      osg::StateSet* set = this->getOrCreateStateSet();

      if (mWrapper->BeginRenderingQuery())
      {
         // select mesh and Submesh for further data access
         if (mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID))
         {
            Cal3dMaterial* calMaterial = mWrapper->GetSelectedSubmeshMaterial();

            osg::Material* material = new osg::Material();
            set->setAttributeAndModes(material, osg::StateAttribute::ON);

            osg::Vec4 materialColor;

            // set the material diffuse color
            materialColor = calMaterial->GetDiffuseColor();

            bool materialTranslucent = materialColor[3] < 1.0f;
            osg::Material::Face materialFace = materialTranslucent ? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;

            material->setDiffuse(materialFace, materialColor);

            // set the material ambient color
            material->setAmbient(materialFace, calMaterial->GetAmbientColor());

            // set the material specular color
            material->setSpecular(materialFace, calMaterial->GetSpecularColor());

            // set the material shininess factor
            float shininess = calMaterial->GetShininess();
            material->setShininess(materialFace, shininess);

            if (calMaterial->GetTextureCount() > 0)
            {
               unsigned i = 0;
               osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(calMaterial->GetTexture(i));

               while (texture != NULL)
               {
                  // TODO:
                  // OSG seems to assume all PNG files have alpha. For now let the code be simple
                  // and use material alpha to flag a material as translucent.

                  // Mark the mesh as a transparency if the image is found to have alpha values.
                  osg::Image* image = texture->getImage();
                  /*if(image != NULL && image->isImageTranslucent())
                  {
                     materialTranslucent = true;
                  }*/

                  set->setTextureAttributeAndModes(i, texture, osg::StateAttribute::ON);
                  texture = dynamic_cast<osg::Texture2D*>(calMaterial->GetTexture(++i));
               }
            }

            if(materialTranslucent)
            {
               osg::BlendFunc* bf = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
               set->setMode(GL_BLEND, osg::StateAttribute::ON);
               set->setAttributeAndModes(bf, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
               set->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            }
         }
         mWrapper->EndRenderingQuery();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   void SubmeshDrawable::InitVertexBuffers(osg::State& state) const
   {
      int vertexCountTotal = 0;
      int faceCountTotal = 0;

      for (unsigned i = 0; i < LOD_COUNT; ++i)
      {
         float lodToQuery = float(i + 1) / float(LOD_COUNT);
         mWrapper->SetLODLevel(lodToQuery);

         if (mWrapper->BeginRenderingQuery())
         {
            // select mesh and Submesh for further data access
            if (mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID))
            {
               dtCore::RefPtr<dtAnim::SubmeshInterface> submesh = mWrapper->GetSelectedSubmesh();
               // begin the rendering loop t get the faces.

               if (submesh.valid())
               {
                  mVertexCount[i] = submesh->GetVertexCount();
                  mFaceCount[i] = submesh->GetFaceCount();

                  vertexCountTotal += mVertexCount[i];
                  faceCountTotal += mFaceCount[i];
               }
            }
            mWrapper->EndRenderingQuery();
         }
      }

      //save the offsets for easy lookup.
      mVertexOffsets[0] = 0;
      mFaceOffsets[0] = 0;

      for (unsigned i = 1; i < LOD_COUNT; ++i)
      {
         mVertexOffsets[i] = mVertexOffsets[i - 1] + mVertexCount[i - 1];
         mFaceOffsets[i] = mFaceOffsets[i - 1 ] + mFaceCount[i - 1];
      }

      // Allocate data arrays to populate
      CalIndex* indexArrayStart = new CalIndex[faceCountTotal * 3];
      float* vertexArrayStart = new float[STRIDE * vertexCountTotal];

      CalIndex* indexArray = indexArrayStart;
      float* vertexArray = vertexArrayStart;

      ///Fill the index and vertex VBOs once for each level of detail
      for (unsigned i = 0; i < LOD_COUNT; ++i)
      {
         float lodToQuery = float(i + 1) / float(LOD_COUNT);
         mWrapper->SetLODLevel(lodToQuery);

         // begin the rendering loop t get the faces.
         if (mWrapper->BeginRenderingQuery())
         {
            // select mesh and Submesh for further data access
            if (mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID))
            {
               dtCore::RefPtr<dtAnim::Cal3dSubmesh> submesh = mWrapper->GetSelectedSubmesh();

               int vertexCount = submesh->GetVertices(vertexArray, STRIDE_BYTES);

               // Position and normal will be copied per frame, no need to do it here
               // Only copy over the texture coordinates.

               submesh->GetTextureCoords(0, vertexArray + 6, STRIDE_BYTES);
               submesh->GetTextureCoords(1, vertexArray + 8, STRIDE_BYTES);

               //invert texture coordinates.
               for (unsigned i = 0; i < vertexCount * STRIDE; i += STRIDE)
               {
                  vertexArray[i + 7] = 1.0f - vertexArray[i + 7]; //the odd texture coordinates in cal3d are flipped, not sure why
                  vertexArray[i + 9] = 1.0f - vertexArray[i + 9]; //the odd texture coordinates in cal3d are flipped, not sure why
               }

               int indexCount = submesh->GetFaces(indexArray);

               ///offset into the vbo to fill the correct lod.
               vertexArray += vertexCount * STRIDE;
               indexArray += indexCount * 3;
            }
         }

         mWrapper->EndRenderingQuery();
      }

      mMeshVBO = new osg::VertexBufferObject;
      mMeshEBO = new osg::ElementBufferObject;

      // Create osg arrays that can be passed to create buffer objects
      osg::FloatArray* osgVertexArray = new osg::FloatArray(STRIDE * vertexCountTotal, vertexArrayStart);

      osg::DrawElements* drawElements = NULL;
      
      // Allocate the draw elements for the element size that CalIndex defines 
      if (sizeof(CalIndex) < 4)
      {
         drawElements = new osg::DrawElementsUShort(GL_TRIANGLES, faceCountTotal * 3, (GLushort*)indexArrayStart);
      }
      else
      {
         drawElements = new osg::DrawElementsUInt(GL_TRIANGLES, faceCountTotal * 3, (GLuint*)indexArrayStart);
      }

      mMeshVBO->addArray(osgVertexArray);
      mMeshEBO->addDrawElements(drawElements);

      // Store the buffers with the model data for possible re-use later
      mModelData->SetVertexBufferObject(mMeshVBO);
      mModelData->SetElementBufferObject(mMeshEBO);

      // The osg arrays copy these values, so we don't need them anymore
      delete[] vertexArrayStart;
      delete[] indexArrayStart;
   }


#define BUFFER_OFFSET(x)((GLvoid*) (0 + ((x) * sizeof(float))))
#define INDEX_OFFSET(x)((GLvoid*) (0 + ((x) * sizeof(CalIndex))))

   ////////////////////////////////////////////////////////////////////////////////////////
   void SubmeshDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
   {
      if (VBOAvailable(renderInfo)) 
      {
         DrawUsingVBO(renderInfo);
      }
      else 
      {
         DrawUsingPrimitives(renderInfo);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   void SubmeshDrawable::accept(osg::PrimitiveFunctor& functor) const
   {
      if (mMeshVBO == 0)
      {
         return;
      }

      /// this processes the lowest LOD at the moment,
      /// because that's what's loaded at the front of the VBO.

      //osg::Vec3f* vertexArray =
      osg::BufferData* vertexArray = mMeshVBO->getBufferData(0);
      if (vertexArray->asArray()->getType() != osg::Array::Vec3ArrayType)
         return;

      functor.setVertexArray(mVertexCount[0], reinterpret_cast<const osg::Vec3*>(vertexArray->getDataPointer()));
     
      const GLvoid* indexArray = mMeshEBO->getBufferData(0)->getDataPointer();
      if (indexArray != NULL)
      {
         if (sizeof(CalIndex) == sizeof(short))
         {
            osg::ref_ptr<osg::DrawElementsUShort> pset = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES,
                                                                   mFaceCount[0] * 3, (GLushort*) indexArray);
            pset->accept(functor);
         }
         else
         {
            osg::ref_ptr<osg::DrawElementsUInt> pset = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,
                                                                 mFaceCount[0] * 3, (GLuint*) indexArray);
            pset->accept(functor);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   osg::Object* SubmeshDrawable::clone(const osg::CopyOp&) const
   {
      return new SubmeshDrawable(mWrapper.get(), mMeshID, mSubmeshID);
   }

   ////////////////////////////////////////////////////////////////////////////////////////
   osg::Object* SubmeshDrawable::cloneType() const
   {
      return new SubmeshDrawable(mWrapper.get(), mMeshID, mSubmeshID);
   }

   //////////////////////////////////////////////////////////////////////////
   bool SubmeshDrawable::VBOAvailable(const osg::RenderInfo& renderInfo) const
   {
      const osg::State *state = renderInfo.getState();
      if (state != NULL)
      {
         return getUseVertexBufferObjects() && state->isVertexBufferObjectSupported();
      }
      else
      {
         return false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SubmeshDrawable::ClearTheState(osg::State& state) const
   {
      state.unbindVertexBufferObject();
      state.unbindElementBufferObject();

      // This data could potential cause problems
      // so we clear it out here (i.e CEGUI incompatible)
      state.setVertexPointer(NULL);

      state.setNormalPointer(NULL);
      state.setTexCoordPointer(0, NULL);
      state.setTexCoordPointer(1, NULL);
   }

   //////////////////////////////////////////////////////////////////////////
   void SubmeshDrawable::DrawUsingVBO(osg::RenderInfo &renderInfo) const
   {
      osg::State& state = *renderInfo.getState();

      //bind the VBO's
      state.disableAllVertexArrays();

      bool initializedThisDraw = false;
      if (!mInitalized)
      {
         InitVertexBuffers(state);
         mInitalized = true;
         initializedThisDraw = true;
      }

      // begin the rendering loop
      if(mWrapper->BeginRenderingQuery())
      {
         // select mesh and Submesh for further data access
         if(mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID))
         {
            dtCore::RefPtr<dtAnim::Cal3dSubmesh> submesh = mWrapper->GetSelectedSubmesh();

            SubmeshUserData* userData = 
               dynamic_cast<SubmeshUserData*>(renderInfo.getUserData());

            float finalLOD = 0.0;
            if (userData != NULL)
            {
               finalLOD = userData->mLOD;
            }
            else
            {
               finalLOD = GetCurrentLOD();
            }

            /// the inverse of ((index + 1) / LOD_COUNT) = lodfloat as seen in InitVertexBuffers.

            unsigned lodIndex = unsigned((float(LOD_COUNT) * finalLOD) - 1.0f);
            dtUtil::Clamp(lodIndex, 0U, LOD_COUNT - 1U);

            ///quantify the lod floating point number.
            finalLOD = (float(lodIndex + 1) / float(LOD_COUNT));

            mWrapper->SetLODLevel(finalLOD);

            const osg::GLExtensions* glExt = osg::GLExtensions::Get(state.getContextID(), true);

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 3
            state.bindVertexBufferObject(mMeshVBO->getOrCreateGLBufferObject(renderInfo.getContextID()));
#else
            state.bindVertexBufferObject(mMeshVBO);
#endif

            // Get the transformed vertices for this frame
            if (!initializedThisDraw)
            {
               float* vertexArray = reinterpret_cast<float*>(glExt->glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB));
               if (vertexArray == NULL)
               {
                  ClearTheState(state);
                  mWrapper->EndRenderingQuery();
                  return;
               }

               ///offset into the vbo to fill the correct lod.
               vertexArray += mVertexOffsets[lodIndex] * STRIDE;

               submesh->GetVertices(vertexArray, STRIDE_BYTES);

               // get the transformed normals of the Submesh
               submesh->GetNormals(vertexArray + 3, STRIDE_BYTES);
               glExt->glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
            }

            unsigned offset = mVertexOffsets[lodIndex] * VBO_STRIDE;

            state.setVertexPointer(3, GL_FLOAT, VBO_STRIDE_BYTES, BUFFER_OFFSET(offset + VBO_OFFSET_POSITION));
            state.setNormalPointer(GL_FLOAT, VBO_STRIDE_BYTES, BUFFER_OFFSET(offset + VBO_OFFSET_NORMAL));
            state.setTexCoordPointer(0, 2, GL_FLOAT, VBO_STRIDE_BYTES, BUFFER_OFFSET(offset + VBO_OFFSET_TEXCOORD0));
            state.setTexCoordPointer(1, 2, GL_FLOAT, VBO_STRIDE_BYTES, BUFFER_OFFSET(offset + VBO_OFFSET_TEXCOORD1));
            state.disableColorPointer();

            state.setNormalPointer(GL_FLOAT, STRIDE_BYTES, BUFFER_OFFSET(3 + offset));
            state.setTexCoordPointer(0, 2, GL_FLOAT, STRIDE_BYTES, BUFFER_OFFSET(6 + offset));
            state.setTexCoordPointer(1, 2, GL_FLOAT, STRIDE_BYTES, BUFFER_OFFSET(8 + offset));

            //make the call to render
#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 3
            state.bindElementBufferObject(mMeshEBO->getOrCreateGLBufferObject(renderInfo.getContextID()));
#else
            state.bindElementBufferObject(mMeshEBO);
#endif
            glDrawElements(GL_TRIANGLES, mFaceCount[lodIndex] * 3U, (sizeof(CalIndex) < 4) ?
                           GL_UNSIGNED_SHORT: GL_UNSIGNED_INT, INDEX_OFFSET(3U * mFaceOffsets[lodIndex]));

            ClearTheState(state);
         }
      }

      // end the rendering
      mWrapper->EndRenderingQuery();
   }

   //////////////////////////////////////////////////////////////////////////
   void SubmeshDrawable::DrawUsingPrimitives(osg::RenderInfo &renderInfo) const
   {
      osg::State& state = *renderInfo.getState();

      // VBO's arn't available, so use conventional rendering path.
      // begin the rendering loop
      if(mWrapper->BeginRenderingQuery())
      {
         // select mesh and submesh for further data access
         if(mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID))
         {
            dtCore::RefPtr<Cal3dSubmesh> submesh = mWrapper->GetSelectedSubmesh();
            dtAnim::MaterialInterface* material = mWrapper->GetSelectedSubmeshMaterial();

            if (submesh.valid())
            {
               int vertexCount = submesh->GetVertexCount();
               int faceCount = submesh->GetFaceCount();
               if (!mMeshVertices) 
               {
                  mMeshVertices           = new float[vertexCount*3];
                  mMeshNormals            = new float[vertexCount*3];
                  mMeshTextureCoordinates = new float[vertexCount*2];
                  mMeshFaces              = new int[faceCount*3];
                  submesh->GetFaces(mMeshFaces);
               }

               // get the transformed vertices of the submesh
               vertexCount = submesh->GetVertices(mMeshVertices);

               // get the transformed normals of the submesh
               submesh->GetNormals(mMeshNormals);

               // get the texture coordinates of the submesh
               // this is still buggy, it renders only the first texture.
               // it should be a loop rendering each texture on its own texture unit
               unsigned tcount = submesh->GetTextureCoords(0, mMeshTextureCoordinates);

               // flip vertical coordinates
               for (int i = 1; i < vertexCount * 2; i += 2)
               {
                  mMeshTextureCoordinates[i] = 1.0f - mMeshTextureCoordinates[i];
               }

               // set the vertex and normal buffers
               state.setVertexPointer(3, GL_FLOAT, 0, mMeshVertices);
               state.setNormalPointer(GL_FLOAT, 0, mMeshNormals);

               // set the texture coordinate buffer and state if necessary
               if(material != NULL && (material->GetTextureCount() > 0) && (tcount > 0))
               {
                  // set the texture coordinate buffer
                  state.setTexCoordPointer(0, 2, GL_FLOAT, 0, mMeshTextureCoordinates);
               }

               // White 
               glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


               // draw the submesh
               if(sizeof(CalIndex) == sizeof(short))
               {
                  glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_SHORT, mMeshFaces);
               }
               else
               {
                  glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, mMeshFaces);
               }

               // get the faces of the submesh for the next frame
               faceCount = submesh->GetFaces(mMeshFaces);
            }
         }

         // end the rendering
         mWrapper->EndRenderingQuery();
      }
   }
} // namespace dtAnim

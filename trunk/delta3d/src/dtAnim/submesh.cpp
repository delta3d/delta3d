#include <dtAnim/submesh.h>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/PolygonMode>
#include <dtAnim/cal3dmodelwrapper.h>
#include <osg/Geometry>

using namespace dtAnim;

class SubMeshDirtyCallback: public osg::Geometry::UpdateCallback 
{
public:
   virtual void update (osg::NodeVisitor *, osg::Drawable *d) 
    {
      d->dirtyBound();
   }
};

SubMeshDrawable::SubMeshDrawable(Cal3DModelWrapper *wrapper, unsigned mesh, unsigned submesh) 
{
   this->mWrapper   = wrapper;
   this->mMeshID    = mesh;
   this->mSubmeshID = submesh;

   setUseDisplayList(false);
   setUpMaterial();

   // initialize arrays, just in case
   mMeshVertices           = 0;
   mMeshNormals            = 0;
   mMeshTextureCoordinates = 0;
   mMeshFaces              = 0;

   // set the model to LOD 1, so that we have the maximal number of
   // vertices and faces in order to be able to pre-allocate the arrays
   mWrapper->SetLODLevel(1);
   
   if(mWrapper->BeginRenderingQuery())
   {
      if(mWrapper->SelectMeshSubmesh(mesh, submesh))
      {
         vertexCount             = mWrapper->GetVertexCount();
         faceCount               = mWrapper->GetFaceCount();
         mMeshVertices           = new float[vertexCount*3];
         mMeshNormals            = new float[vertexCount*3];
         mMeshTextureCoordinates = new float[vertexCount*2];
         mMeshFaces              = new int[faceCount*3];

         mWrapper->GetFaces(mMeshFaces);
      }

      mWrapper->EndRenderingQuery();
   }
   setUpdateCallback(new SubMeshDirtyCallback());
}

SubMeshDrawable::~SubMeshDrawable(void)
{
   delete [] mMeshVertices;
   delete [] mMeshNormals;
   delete [] mMeshFaces;
   delete [] mMeshTextureCoordinates;
}

void SubMeshDrawable::setUpMaterial() 
{
   osg::StateSet *set = this->getOrCreateStateSet();   
   
   // select mesh and submesh for further data access
   if(mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID)) 
   {
     osg::Material *material = new osg::Material();
     //material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
     set->setAttributeAndModes(material, osg::StateAttribute::ON);
     unsigned char meshColor[4];
     osg::Vec4 materialColor;

     // set the material ambient color
     mWrapper->GetAmbientColor(&meshColor[0]);
     materialColor[0] = meshColor[0] / 255.0f;
     materialColor[1] = meshColor[1] / 255.0f;
     materialColor[2] = meshColor[2] / 255.0f;
     materialColor[3] = meshColor[3] / 255.0f;
     //if (materialColor[3] == 0) materialColor[3]=1.0f;
     material->setAmbient(osg::Material::FRONT_AND_BACK, materialColor);

     // set the material diffuse color
     mWrapper->GetDiffuseColor( &meshColor[0] );
     materialColor[0] = meshColor[0] / 255.0f;
     materialColor[1] = meshColor[1] / 255.0f;
     materialColor[2] = meshColor[2] / 255.0f;
     materialColor[3] = meshColor[3] / 255.0f;
     //if (materialColor[3] == 0) materialColor[3]=1.0f;
     material->setDiffuse(osg::Material::FRONT_AND_BACK, materialColor);

     // set the material specular color
     mWrapper->GetSpecularColor(&meshColor[0]);
     materialColor[0] = meshColor[0] / 255.0f;
     materialColor[1] = meshColor[1] / 255.0f;
     materialColor[2] = meshColor[2] / 255.0f;
     materialColor[3] = meshColor[3] / 255.0f;
     //if (materialColor[3] == 0) materialColor[3]=1.0f;
     material->setSpecular(osg::Material::FRONT_AND_BACK, materialColor);

     // set the material shininess factor
     float shininess;
     shininess = mWrapper->GetShininess();
     material->setShininess(osg::Material::FRONT_AND_BACK, shininess);

     if (mWrapper->GetMapCount() > 0)
     {
        osg::Texture2D *texture = (osg::Texture2D*)mWrapper->GetMapUserData(0);
        if (texture != 0) 
        {
           set->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
        }
     }      
   }
}

void SubMeshDrawable::drawImplementation(osg::RenderInfo & renderInfo) const 
{
   // begin the rendering loop
   if(mWrapper->BeginRenderingQuery())
   {
      // select mesh and submesh for further data access
      if(mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID))
      {
         osg::State *state = renderInfo.getState();
         state->disableAllVertexArrays();

         // get the transformed vertices of the submesh
         vertexCount = mWrapper->GetVertexCount();
         vertexCount = mWrapper->GetVertices(mMeshVertices);

         // get the transformed normals of the submesh
         mWrapper->GetNormals(mMeshNormals);
         
         // get the texture coordinates of the submesh
         // this is still buggy, it renders only the first texture.
         // it should be a loop rendering each texture on its own texture unit
         unsigned tcount = mWrapper->GetTextureCoords(0, mMeshTextureCoordinates);

         // flip vertical coordinates
         for (unsigned int i = 1; i < vertexCount * 2; i += 2)
         {
            mMeshTextureCoordinates[i] = 1.0f - mMeshTextureCoordinates[i];
         }


         // set the vertex and normal buffers
         state->setVertexPointer(3, GL_FLOAT, 0, mMeshVertices);
         state->setNormalPointer(GL_FLOAT, 0, mMeshNormals);

         // set the texture coordinate buffer and state if necessary
         if((mWrapper->GetMapCount() > 0) && (tcount > 0))
         {
            // set the texture coordinate buffer
            state->setTexCoordPointer(0, 2, GL_FLOAT, 0, mMeshTextureCoordinates);
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
         faceCount = mWrapper->GetFaces(mMeshFaces);
      }

      // end the rendering
      mWrapper->EndRenderingQuery();
   }
}


void SubMeshDrawable::accept(osg::PrimitiveFunctor& functor) const
{
   if(mWrapper != NULL && mMeshVertices != NULL)
   {
      if(mWrapper->BeginRenderingQuery())
      {
         // select mesh and submesh for further data access
         if(mWrapper->SelectMeshSubmesh(mMeshID, mSubmeshID))
         {
            //since the vertices aren't set until we first render, we must get them here to ensure they are valid
            mWrapper->GetVertices(mMeshVertices);

            functor.setVertexArray(vertexCount, (osg::Vec3f *)(mMeshVertices));

            if(sizeof(CalIndex) == sizeof(short))
            {
               osg::ref_ptr<osg::DrawElementsUShort> pset = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES,
                                                                      faceCount*3, (GLushort *) mMeshFaces);
               pset->accept(functor);
            } 
            else
            {
              osg::ref_ptr<osg::DrawElementsUInt> pset = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,
                                                                   faceCount*3, (GLuint *) mMeshFaces);
              pset->accept(functor);
           }
         }
      }

   }

   return;
}

osg::Object* SubMeshDrawable::clone(const osg::CopyOp&) const 
{
   return new SubMeshDrawable(mWrapper.get(), mMeshID, mSubmeshID);
}

osg::Object* SubMeshDrawable::cloneType() const
{
   return new SubMeshDrawable(mWrapper.get(), mMeshID, mSubmeshID);
}

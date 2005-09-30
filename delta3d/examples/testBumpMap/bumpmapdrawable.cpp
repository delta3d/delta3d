#include "bumpmapdrawable.h"
#include "cubevertices.h"

#include <osg/PolygonMode>
#include <dtCore/globals.h>

BumpMapDrawable::BumpMapDrawable()
{

   mNode = new osg::Group;

}

BumpMapDrawable::~BumpMapDrawable()
{


}

void BumpMapDrawable::SetUniforms(const osg::Vec3& pLightPos, const osg::Vec3& pEyePos)
{
   osg::StateSet* ss = mGeode->getOrCreateStateSet();
   ss->getUniform( "lightPos" )->set( osg::Vec4(pLightPos[0], pLightPos[1], pLightPos[2], 0.0f));
   ss->getUniform( "eyePosition" )->set(osg::Vec4(pEyePos[0], pEyePos[2], pEyePos[2], 0.0f));
}

void BumpMapDrawable::SetWireframe(bool pWireframe)
{
   osg::StateSet* ss = mGeode->getOrCreateStateSet();

   if(pWireframe)
   {
      osg::PolygonMode *polymode = new osg::PolygonMode;
      polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
      ss->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
   }
   else
   {
      osg::PolygonMode *polymode = new osg::PolygonMode;
      polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      ss->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
   }

}

void BumpMapDrawable::Initialize()
{

   CreateGeometry();
   EnableShaders();

   osg::Group* grp = dynamic_cast<osg::Group*>(mNode.get());
   grp->addChild(mGeode.get());
}

void BumpMapDrawable::CreateGeometry()
{
   CubeVertices mData;

   mGeode = new osg::Geode;
   mGeometry = new osg::Geometry;

   //set our geometry variables

   //the actual verts
   mGeometry->setVertexArray(mData.mVerts.get());
   mGeometry->setNormalArray(mData.mNormals.get());

   //verts 2 and 3
   mGeometry->setTexCoordArray(0, mData.mVerts2.get());
   mGeometry->setTexCoordArray(1, mData.mVerts3.get());

   //tex coords 1-3

   mGeometry->setTexCoordArray(2, mData.mTexCoords.get());
   mGeometry->setTexCoordArray(3, mData.mTexCoords2.get());
   mGeometry->setTexCoordArray(4, mData.mTexCoords3.get());

   //create indices
   RefPtr<osg::IntArray> indices = 
      new osg::IntArray(36);

   for(int i = 0; i < 36; ++i)
   {
      (*indices)[i] = i;
   }

   mGeometry->setVertexIndices(indices.get());

   mGeometry->setNormalIndices(indices.get());

   mGeometry->setTexCoordIndices(0, indices.get());
   mGeometry->setTexCoordIndices(1, indices.get());
   mGeometry->setTexCoordIndices(2, indices.get());
   mGeometry->setTexCoordIndices(3, indices.get());
   mGeometry->setTexCoordIndices(4, indices.get());

   mGeometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 36) );

   mGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

   mGeode->addDrawable(mGeometry.get());
   mGeode->setName("BumpMapCube");

}

void BumpMapDrawable::EnableShaders()
{
   //create state set
   osg::StateSet* ss = mGeode->getOrCreateStateSet();
   //ss->setMode(osg::StateAttribute::CULLFACE, GL_FRONT);

   //set up textures
   RefPtr<osg::Image> img1 = osgDB::readImageFile( std::string(GetDeltaRootPath()+ "/data/textures/sheetmetal.tga"));
   RefPtr<osg::Image> img2 = osgDB::readImageFile( std::string(GetDeltaRootPath()+ "/data/textures/delta3d_logo_normal_map.tga"));


   RefPtr<osg::Texture2D> tex1 = new osg::Texture2D(img1.get());
   RefPtr<osg::Texture2D> tex2 = new osg::Texture2D(img2.get());

   tex1->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   tex1->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

   tex2->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   tex2->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

   ss->setTextureAttributeAndModes(0,tex1.get(),osg::StateAttribute::ON);
   ss->setTextureAttributeAndModes(1,tex2.get(),osg::StateAttribute::ON);

   ss->setTextureMode(0, GL_TEXTURE_2D, GL_TRUE);
   ss->setTextureMode(1, GL_TEXTURE_2D, GL_TRUE);


   //load the shader file
   mProg = new osg::Program;

   RefPtr<osg::Shader> bumpMapVert = new osg::Shader( osg::Shader::VERTEX );
   RefPtr<osg::Shader> bumpMapFrag = new osg::Shader( osg::Shader::FRAGMENT );

   mProg->addShader( bumpMapVert.get() );
   mProg->addShader( bumpMapFrag.get() );

   bumpMapVert->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/bumpmap.vert");
   bumpMapFrag->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/bumpmap.frag");

   mLightPos = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "lightPos");
   mLightPos->set(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
   ss->addUniform(mLightPos.get());

   mEyePos = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "eyePosition");
   mEyePos->set(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
   ss->addUniform(mEyePos.get());


   RefPtr<osg::Uniform> unTex1 = new osg::Uniform(osg::Uniform::SAMPLER_2D, "texMap");
   unTex1->set(0);
   ss->addUniform(unTex1.get());


   RefPtr<osg::Uniform> unTex2 = new osg::Uniform(osg::Uniform::SAMPLER_2D, "normalMap");
   unTex2->set(1);
   ss->addUniform(unTex2.get());

   ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::ON );
}

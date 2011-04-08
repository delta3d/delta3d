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
 * Bradley Anderegg
 */
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/skybox.h>

#include <dtCore/moveearthtransform.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtUtil/log.h>

#include <osg/Depth>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Image>
#include <osg/MatrixTransform>
#include <osg/PolygonMode> ///for wireframe rendering
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Vec3>
#include <osg/GLExtensions>
#include <osg/Texture2D>
#include <osg/TextureCubeMap>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(SkyBox)

SkyBox::SkyBox(const std::string& name, RenderProfileEnum pRenderProfile):
EnvEffect(name),
mRenderProfilePreference(pRenderProfile),
mRenderProfile(0),
mInitializedTextures(false)
{
   RegisterInstance(this);

   SetOSGNode( new osg::Group() );

   GetOSGNode()->setCullCallback(new SkyBox::ConfigCallback(this));

   memset(mTexPreSetList, 0, sizeof(bool) * 6);
}

SkyBox::~SkyBox()
{
   DeregisterInstance(this);
}


void SkyBox::Config()
{
   GetOSGNode()->setCullCallback(0);

   SetRenderProfile(mRenderProfilePreference);

   if(mInitializedTextures)
   {
      for(int i = 0; i < 6; ++i)
      {
         if(mTexPreSetList[i])
         {
            SetTexture(SkyBoxSideEnum(i), mTexList[i]);
         }
      }
   }

   mRenderProfile->Config(GetOSGNode()->asGroup());
}


void SkyBox::SetRenderProfile(RenderProfileEnum pRenderProfile)
{
   CheckHardware();

   switch(pRenderProfile)
   {
      case RP_CUBE_MAP:
         {
            if(mSupportedProfiles[RP_CUBE_MAP])
            {
               mRenderProfile = new SkyBox::CubeMapProfile();
               return;
            }
            else
            {
               Log::GetInstance().LogMessage(Log::LOG_ERROR,__FUNCTION__,
                  "The SkyBox RenderProfile selected is not available for your hardware" );
            }
         }

      case RP_ANGULAR_MAP:
         {
            if(mSupportedProfiles[RP_ANGULAR_MAP])
            {
               mRenderProfile = new SkyBox::AngularMapProfile();
               return;
            }
            else
            {
               Log::GetInstance().LogMessage(Log::LOG_ERROR,__FUNCTION__,
                  "The SkyBox RenderProfile selected is not available for your hardware" );
            }
         }

      case RP_DEFAULT:
         {
            if(mSupportedProfiles[RP_CUBE_MAP])
            {
               mRenderProfile = new SkyBox::CubeMapProfile();
               return;
            }
            else
            {
               mRenderProfile = new SkyBox::FixedFunctionProfile();
            }

         }

      default:
         {
            mRenderProfile = new SkyBox::FixedFunctionProfile();
            return;
         }
   }

}


void SkyBox::CheckHardware()
{

   //this should always be supported
   mSupportedProfiles[RP_FIXED_FUNCTION] = true;

   if(osg::isGLExtensionSupported(0, "GL_ARB_texture_cube_map") && osg::isGLExtensionSupported(0, "GL_ARB_fragment_shader") && osg::isGLExtensionSupported(0, "GL_ARB_vertex_shader"))
   {
      mSupportedProfiles[RP_CUBE_MAP] = true;
   }
   else
   {
      mSupportedProfiles[RP_CUBE_MAP] = false;
   }

   if (osg::isGLExtensionSupported(0, "GL_ARB_fragment_shader") && osg::isGLExtensionSupported(0, "GL_ARB_vertex_shader"))
   {
        mSupportedProfiles[RP_ANGULAR_MAP] = true;
   }
   else
   {
      mSupportedProfiles[RP_ANGULAR_MAP] = false;
   }

}


void SkyBox::SetTexture(SkyBoxSideEnum side, const std::string& filename)
{
   if(mRenderProfile.valid())
   {
      mRenderProfile->SetTexture(side, filename);
   }
   else
   {
      mTexList[side] = filename;
      mTexPreSetList[side] = true;
      mInitializedTextures = true;
   }
}

void SkyBox::Repaint(   const osg::Vec3& skyColor,
                        const osg::Vec3& fogColor,
                        double sunAngle,
                        double sunAzimuth,
                        double visibility )
{
   //need to recolor anything?
}

//////////////////////////////////////////////////////////////////////////
//Now for the implementation of the different render profiles         //
//////////////////////////////////////////////////////////////////////////

SkyBox::AngularMapProfile::AngularMapProfile()
{
   mGeode = new osg::Geode();
   mAngularMap = new osg::Texture2D();
   mGeode->addDrawable(new SkyBox::SkyBoxDrawable());
}


void SkyBox::AngularMapProfile::Config(osg::Group* pGroup)
{
   osg::StateSet* ss = mGeode->getOrCreateStateSet();

   osg::Depth* depth = new osg::Depth;
   depth->setFunction(osg::Depth::ALWAYS);
   depth->setRange(1.0,1.0);
   ss->setAttributeAndModes(depth, osg::StateAttribute::ON );


   ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   ss->setMode(GL_FOG,osg::StateAttribute::OFF);
   ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF );

   ss->setRenderBinDetails(-2,"RenderBin");


   osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
   modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   modelview_abs->setMatrix(osg::Matrix::identity());
   modelview_abs->addChild(mGeode.get());

   osg::Projection* projection = new osg::Projection;
   projection->setMatrix(osg::Matrix::ortho2D(0,1,0,1));
   projection->addChild(modelview_abs);


   pGroup->addChild(projection);



   //load texture
   mAngularMap->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   mAngularMap->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
   mAngularMap->setUnRefImageDataAfterApply(true);
   ss->setTextureAttributeAndModes(0,mAngularMap.get(),osg::StateAttribute::ON);
   ss->setTextureMode(0, GL_TEXTURE_2D, GL_TRUE);


   //setup shaders

   mProgram = new osg::Program;
   dtCore::RefPtr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
   dtCore::RefPtr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);

   std::string vertFile = dtCore::FindFileInPathList("/shaders/AngularMapSkyBox.vert");
   std::string fragFile = dtCore::FindFileInPathList("/shaders/AngularMapSkyBox.frag");

   if (vertFile.empty())
   {
      LOG_ERROR("Can not find the SkyBox shader file: AngularMapSkyBox.vert");
   }

   if (fragFile.empty())
   {
      LOG_ERROR("Can not find the SkyBox shader file: AngularMapSkyBox.frag");
   }

   vertShader->loadShaderSourceFromFile(vertFile);
   fragShader->loadShaderSourceFromFile(fragFile);

   vertShader->setName("SkyBox angular map vertex shader");
   fragShader->setName("SkyBox angular map fragment shader");

   mProgram->addShader(vertShader.get());
   mProgram->addShader(fragShader.get());

   dtCore::RefPtr<osg::Uniform> tex = new osg::Uniform(osg::Uniform::SAMPLER_2D, "angularMap");
   tex->set(0);
   ss->addUniform(tex.get());

   mInverseModelViewProjMatrix = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "inverseModelViewProjMatrix");
   ss->addUniform(mInverseModelViewProjMatrix.get());

   ss->setAttributeAndModes(mProgram.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

   mGeode->setCullCallback(new AngularMapProfile::UpdateViewCallback(this));

}

//note that the side doesnt matter because an angular map is just one texture
void SkyBox::AngularMapProfile::SetTexture(SkyBoxSideEnum side, const std::string& filename)
{
   osg::Image *newImage = osgDB::readImageFile(filename);
   if (newImage == 0)
   {
      Log::GetInstance().LogMessage(Log::LOG_ERROR,__FUNCTION__,
         "Can't load texture file '%s'.",filename.c_str() );
   }
   mAngularMap->setImage(newImage);
   mAngularMap->dirtyTextureObject();
}

void SkyBox::AngularMapProfile::UpdateViewMatrix(const osg::Matrix& viewMat, const osg::Matrix& projMat)
{

   osg::Matrix proj;
   proj.invert(projMat);

   osg::Matrix view;
   view.invert(viewMat);

   mInverseModelViewProjMatrix->set(proj * view);
}



SkyBox::CubeMapProfile::CubeMapProfile()
{
   mGeode = new osg::Geode();
   mCubeMap = new osg::TextureCubeMap();
   mGeode->addDrawable(new dtCore::SkyBox::SkyBoxDrawable());

}


void SkyBox::CubeMapProfile::Config(osg::Group* pGroup)
{

   mCubeMap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
   mCubeMap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
   mCubeMap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
   mCubeMap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
   mCubeMap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
   mCubeMap->setUnRefImageDataAfterApply(true);

   osg::StateSet* ss = mGeode->getOrCreateStateSet();

   ss->setTextureAttributeAndModes(0, mCubeMap.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);


   osg::Depth* depth = new osg::Depth;
   depth->setFunction(osg::Depth::ALWAYS);
   depth->setRange(1.0,1.0);
   ss->setAttributeAndModes(depth, osg::StateAttribute::ON );


   ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   ss->setMode(GL_FOG,osg::StateAttribute::OFF);
   ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF );

   ss->setRenderBinDetails(-2,"RenderBin");


   dtCore::RefPtr<osg::MatrixTransform> modelview_abs = new osg::MatrixTransform;
   modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   modelview_abs->setMatrix(osg::Matrix::identity());
   modelview_abs->addChild(mGeode.get());

   dtCore::RefPtr<osg::Projection> projection = new osg::Projection;
   projection->setMatrix(osg::Matrix::ortho2D(0,1,0,1));
   projection->addChild(modelview_abs.get());


   pGroup->addChild(projection.get());


   //setup shaders

   mProgram = new osg::Program;
   dtCore::RefPtr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
   dtCore::RefPtr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);

   vertShader->setName("SkyBox cube map vertex shader");
   fragShader->setName("SkyBox cube map fragment shader");

   mProgram->addShader(vertShader.get());
   mProgram->addShader(fragShader.get());

   std::string vertFile = dtCore::FindFileInPathList("/shaders/CubeMapSkyBox.vert");
   std::string fragFile = dtCore::FindFileInPathList("/shaders/CubeMapSkyBox.frag");

   if (vertFile.empty())
   {
      LOG_ERROR("Can not find the SkyBox shader file: CubeMapSkyBox.vert");
   }

   if (fragFile.empty())
   {
      LOG_ERROR("Can not find the SkyBox shader file: CubeMapSkyBox.frag");
   }

   vertShader->loadShaderSourceFromFile(vertFile);
   fragShader->loadShaderSourceFromFile(fragFile);

   dtCore::RefPtr<osg::Uniform> tex = new osg::Uniform(osg::Uniform::SAMPLER_CUBE, "cubeMap");
   tex->set(0);
   ss->addUniform(tex.get());

   mInverseModelViewProjMatrix = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "inverseModelViewProjMatrix");
   ss->addUniform(mInverseModelViewProjMatrix.get());

   ss->setAttributeAndModes(mProgram.get(), osg::StateAttribute::ON);

   mGeode->setCullCallback(new CubeMapProfile::UpdateViewCallback(this));

}


void SkyBox::CubeMapProfile::SetTexture(SkyBoxSideEnum side, const std::string& filename)
{
   osg::Image *newImage = osgDB::readImageFile(filename);
   if (newImage == 0)
   {
      Log::GetInstance().LogMessage(Log::LOG_ERROR,__FUNCTION__,
         "Can't load texture file '%s'.",filename.c_str() );
   }

   //the cube map expects different values for texture faces
   //so this will switch the side with the side enum expected for osg
   int newSide = side;
   switch(newSide)
   {
      case SkyBox::SKYBOX_FRONT:
         {
            newSide = SkyBox::SKYBOX_TOP;
            break;
         }
      case SkyBox::SKYBOX_BACK:
         {
            newSide = SkyBox::SKYBOX_BOTTOM;
            break;
         }
      case SkyBox::SKYBOX_RIGHT:
         {
            newSide = SkyBox::SKYBOX_FRONT;
            break;
         }
      case SkyBox::SKYBOX_LEFT:
         {
            newSide = SkyBox::SKYBOX_RIGHT;
            break;
         }
      case SkyBox::SKYBOX_TOP:
         {
            newSide = SkyBox::SKYBOX_LEFT;
            break;
         }
      case SkyBox::SKYBOX_BOTTOM:
         {
            newSide = SkyBox::SKYBOX_BACK;
            break;
         }
      default:
         {
            break;
         }

   }

   mCubeMap->setImage(newSide, newImage);
   mCubeMap->dirtyTextureObject();
}


void SkyBox::CubeMapProfile::UpdateViewMatrix(const osg::Matrix& viewMat, const osg::Matrix& projMat)
{

   osg::Matrix proj = projMat;

   proj.invert(proj);

   //these should be opposite but aren't always
   //do to precision issues
   proj(3,3) = -proj(2,3);

   osg::Matrix view;
   view.invert(viewMat);

   mInverseModelViewProjMatrix->set(proj * view);
}


SkyBox::FixedFunctionProfile::FixedFunctionProfile()
{
   mGeode = new osg::Geode();
   for(int i = 0; i < 6; ++i)
   {
      mTextureList[i] = new osg::Texture2D();
   }
}


/** Private method that actually creates the SkyBox geometry.  It is assumed
 *  that the textures have already been assigned.
 */
void dtCore::SkyBox::FixedFunctionProfile::Config(osg::Group* pNode)
{
   mXform = new MoveEarthySkyWithEyePointTransform();

   mXform->setCullingActive(false);
   mXform->addChild( MakeBox() );

   //Drawing a skybox eliminates the need for clearing the color and depth buffers.
   osg::ClearNode* clearNode = new osg::ClearNode;
   clearNode->setRequiresClear(false); //Sky eliminates need for clearing
   clearNode->addChild(mXform.get());
   pNode->addChild(clearNode);

}

/** Make the box and load the textures */
osg::Node* dtCore::SkyBox::FixedFunctionProfile::MakeBox()
{

   float mX = 100.0f;
   float mY = 100.0f;
   float mZ = 100.0f;

   osg::Vec3 coords0[] = //front
   {
      osg::Vec3(-mX, mY, -mZ),
      osg::Vec3(mX, mY,-mZ),
      osg::Vec3(mX, mY, +mZ),
      osg::Vec3(-mX, mY, +mZ),
   };

   osg::Vec3 coords1[] = //right
   {
      osg::Vec3(mX, mY, -mZ),
      osg::Vec3(mX, -mY, -mZ),
      osg::Vec3(mX, -mY, mZ),
      osg::Vec3(mX, mY, mZ)
   };

   osg::Vec3 coords2[] = //back
   {
      osg::Vec3(mX, -mY, -mZ),
      osg::Vec3(-mX, -mY, -mZ),
      osg::Vec3(-mX, -mY, mZ),
      osg::Vec3(mX, -mY, mZ)
   };

   osg::Vec3 coords3[] = //left
   {
      osg::Vec3(-mX, -mY, -mZ),
      osg::Vec3(-mX, mY, -mZ),
      osg::Vec3(-mX, mY, mZ),
      osg::Vec3(-mX, -mY, mZ)
   };

   osg::Vec3 coords4[] = //top
   {
      osg::Vec3(-mX, mY, mZ),
      osg::Vec3(mX, mY, mZ),
      osg::Vec3(mX, -mY, mZ),
      osg::Vec3(-mX, -mY, mZ)
   };
   osg::Vec3 coords5[] = //bottom
   {
      osg::Vec3(-mX, mY, -mZ),
      osg::Vec3(-mX, -mY, -mZ),
      osg::Vec3(mX, -mY, -mZ),
      osg::Vec3(mX, mY, -mZ)
   };

   osg::Vec2 tCoords[] =
   {
      osg::Vec2(0,0),
      osg::Vec2(1,0),
      osg::Vec2(1,1),
      osg::Vec2(0,1)
   };
   osg::Geometry *polyGeom[6];

   osg::Vec4Array *colors = new osg::Vec4Array(1);
   (*colors)[0] = osg::Vec4(1.f, 1.f, 1.f, 1.f);

   osg::Vec3Array *vArray[6];
   vArray[0] = new osg::Vec3Array(4, coords0);
   vArray[1] = new osg::Vec3Array(4, coords1);
   vArray[2] = new osg::Vec3Array(4, coords2);
   vArray[3] = new osg::Vec3Array(4, coords3);
   vArray[4] = new osg::Vec3Array(4, coords4);
   vArray[5] = new osg::Vec3Array(4, coords5);

   for (int side=0; side<6; side++)
   {
      polyGeom[side] = new osg::Geometry();

      polyGeom[side]->setVertexArray(vArray[side]);
      polyGeom[side]->setTexCoordArray(0, new osg::Vec2Array(4, tCoords));
      polyGeom[side]->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
      polyGeom[side]->setColorArray(colors);
      polyGeom[side]->setColorBinding(osg::Geometry::BIND_OVERALL);

      osg::StateSet *dstate = new osg::StateSet;
      dstate->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
      dstate->setMode( GL_CULL_FACE, osg::StateAttribute::ON );

      // clear the depth to the far plane.
      osg::Depth* depth = new osg::Depth;
      depth->setFunction(osg::Depth::ALWAYS);
      depth->setRange(1.0,1.0);
      dstate->setAttributeAndModes(depth,osg::StateAttribute::ON );
      dstate->setMode(GL_FOG, osg::StateAttribute::OFF );
      dstate->setMode(GL_LIGHTING,osg::StateAttribute::PROTECTED |
         osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
      dstate->setRenderBinDetails(-2,"RenderBin");

      mTextureList[side]->setUnRefImageDataAfterApply(true);
      mTextureList[side]->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
      mTextureList[side]->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
      mTextureList[side]->setFilter(osg::Texture::MIN_FILTER,
         osg::Texture::LINEAR_MIPMAP_LINEAR);
      mTextureList[side]->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
      dstate->setTextureAttributeAndModes(0,mTextureList[side].get(),
         osg::StateAttribute::ON);

      polyGeom[side]->setStateSet( dstate );
      mGeode->addDrawable(polyGeom[side]);
   }
   return mGeode.get();
}



/** Pass in the filenames for the textures to be applied to the SkyBox.*/
void dtCore::SkyBox::FixedFunctionProfile::SetTexture(SkyBox::SkyBoxSideEnum side, const std::string& filename)
{
   osg::Image *newImage = osgDB::readImageFile(filename);
   if (newImage == 0)
   {
      Log::GetInstance().LogMessage(Log::LOG_ERROR,__FUNCTION__,
         "Can't load texture file '%s'.",filename.c_str() );
   }

   mTextureList[side]->setImage(newImage);
   mTextureList[side]->dirtyTextureObject();
}


//our drawable
void SkyBox::SkyBoxDrawable::drawImplementation(osg::RenderInfo & /*renderInfo*/) const
{

   glOrtho(0, 1, 0, 1, 0, 1);

   glBegin(GL_QUADS);

   glVertex2i(1, 1);
   glVertex2i(0, 1);
   glVertex2i(0, 0);
   glVertex2i(1, 0);

   glEnd();

}
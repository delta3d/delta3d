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
#include <prefix/dtcoreprefix.h>
#include <dtCore/skybox.h>
#include <dtCore/skyboxprofiles.h>

#include <dtCore/moveearthtransform.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>

#include <osg/Depth>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Image>
#include <osg/MatrixTransform>
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

////////////////////////////////////////////////////////////////////////////////
SkyBox::SkyBox(const std::string& name, RenderProfileEnum pRenderProfile)
   : EnvEffect(name)
   , mRenderProfilePreference(pRenderProfile)
   , mRenderProfile(0)
   , mConfigCallback(new SkyBox::ConfigCallback(this))
   , mInitializedTextures(false)
{
   RegisterInstance(this);

   SetOSGNode(new osg::Group());

   GetOSGNode()->setUpdateCallback(mConfigCallback.get());

   CheckHardware();

   memset(mTexPreSetList, 0, sizeof(bool) * 6);
}

////////////////////////////////////////////////////////////////////////////////
SkyBox::~SkyBox()
{
   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void SkyBox::Config()
{
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

   GetOSGNode()->removeUpdateCallback(mConfigCallback.get());
}

////////////////////////////////////////////////////////////////////////////////
void SkyBox::SetRenderProfile(RenderProfileEnum pRenderProfile)
{
   switch(pRenderProfile)
   {
      case RP_CUBE_MAP:
         {
            if(mSupportedProfiles[RP_CUBE_MAP])
            {
               mRenderProfile = new CubeMapProfile();
               return;
            }
            else
            {
               Log::GetInstance().LogMessage(Log::LOG_ERROR,__FUNCTION__,
                  "The SkyBox RenderProfile selected is not available for your hardware");
            }
         }

      case RP_ANGULAR_MAP:
         {
            if(mSupportedProfiles[RP_ANGULAR_MAP])
            {
               mRenderProfile = new AngularMapProfile();
               return;
            }
            else
            {
               Log::GetInstance().LogMessage(Log::LOG_ERROR,__FUNCTION__,
                  "The SkyBox RenderProfile selected is not available for your hardware");
            }
         }

      case RP_DEFAULT:
         {
            if(mSupportedProfiles[RP_CUBE_MAP])
            {
               mRenderProfile = new CubeMapProfile();
               return;
            }
            else
            {
               mRenderProfile = new FixedFunctionProfile();
            }

         }
         break;

      default:
         {
            mRenderProfile = new FixedFunctionProfile();
         }
   }

}

////////////////////////////////////////////////////////////////////////////////
void SkyBox::CheckHardware()
{
   //this should always be supported
   mSupportedProfiles[RP_FIXED_FUNCTION] = true;

   if(osg::isGLExtensionSupported(0, "GL_ARB_texture_cube_map") &&
      osg::isGLExtensionSupported(0, "GL_ARB_fragment_shader") &&
      osg::isGLExtensionSupported(0, "GL_ARB_vertex_shader"))
   {
      mSupportedProfiles[RP_CUBE_MAP] = true;
   }
   else
   {
      mSupportedProfiles[RP_CUBE_MAP] = false;
   }

   if (osg::isGLExtensionSupported(0, "GL_ARB_fragment_shader") &&
       osg::isGLExtensionSupported(0, "GL_ARB_vertex_shader"))
   {
        mSupportedProfiles[RP_ANGULAR_MAP] = true;
   }
   else
   {
      mSupportedProfiles[RP_ANGULAR_MAP] = false;
   }

}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void dtCore::SkyBox::SetRenderProfilePreference(RenderProfileEnum profileHint)
{
   mRenderProfilePreference = profileHint;
}

////////////////////////////////////////////////////////////////////////////////
void SkyBox::Repaint(const osg::Vec3& skyColor,
                     const osg::Vec3& fogColor,
                     double sunAngle,
                     double sunAzimuth,
                     double visibility)
{
   //need to recolor anything?
}

////////////////////////////////////////////////////////////////////////////////
void SkyBox::SkyBoxDrawable::drawImplementation(osg::RenderInfo& /*renderInfo*/) const
{
   glOrtho(0, 1, 0, 1, 0, 1);

   glBegin(GL_QUADS);

   glVertex2i(1, 1);
   glVertex2i(0, 1);
   glVertex2i(0, 0);
   glVertex2i(1, 0);

   glEnd();
}

////////////////////////////////////////////////////////////////////////////////

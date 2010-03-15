/* -*-c++-*-
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

#ifndef DELTA_SKYBOX_PROFILES
#define DELTA_SKYBOX_PROFILES

#include <dtCore/enveffect.h>
#include <dtCore/skybox.h>
#include <dtCore/refptr.h>

#include <osg/CameraNode>
#include <osg/Drawable>
#include <osg/NodeCallback>
#include <osgUtil/RenderStage>
#include <osgUtil/CullVisitor>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
   class NodeCallback;
   class Texture2D;
   class TextureCubeMap;
}
/// @endcond

namespace dtCore
{


   ///this class will use an angular map or light probe
   ///to act as a skybox
   class DT_CORE_EXPORT AngularMapProfile: public dtCore::SkyBox::RenderProfile
   {
   protected:
      class UpdateViewCallback: public osg::NodeCallback
      {
      public:
         UpdateViewCallback(AngularMapProfile* mp):mProfile(mp){}

         void operator()(osg::Node*, osg::NodeVisitor* nv)
         {
            osgUtil::CullVisitor* cullVisitor = dynamic_cast<osgUtil::CullVisitor*>(nv);
            if (cullVisitor != NULL)
            {
               osgUtil::RenderStage* rs = cullVisitor->getCurrentRenderBin()->getStage();
               osg::Camera* cn = rs->getCamera();
               if (cn)
               {
                  mProfile->UpdateViewMatrix(cn->getViewMatrix(), cn->getProjectionMatrix());
               }
            }
         }
      private:
         AngularMapProfile* mProfile;
      };

      friend class UpdateViewCallback;

   public:
      AngularMapProfile();

      void Config(osg::Group*);
      void SetTexture(dtCore::SkyBox::SkyBoxSideEnum side, const std::string& filename);

   protected:

      void UpdateViewMatrix(const osg::Matrix& viewMat, const osg::Matrix& projMat);

      dtCore::RefPtr<osg::Geode>     mGeode;
      dtCore::RefPtr<osg::Texture2D> mAngularMap;
      dtCore::RefPtr<osg::Program>   mProgram;
      dtCore::RefPtr<osg::Uniform>   mInverseModelViewProjMatrix;

   };

   ///this render profile will render the skybox as usual with the
   ///fixed function pipeline
   class DT_CORE_EXPORT FixedFunctionProfile: public dtCore::SkyBox::RenderProfile
   {

   public:
      FixedFunctionProfile();
      void Config(osg::Group* pNode);
      void SetTexture(dtCore::SkyBox::SkyBoxSideEnum side, const std::string& filename);

   protected:

      osg::Node* MakeBox();

      dtCore::RefPtr<osg::Geode> mGeode;
      dtCore::RefPtr<dtCore::MoveEarthySkyWithEyePointTransform> mXform;
      dtCore::RefPtr<osg::Texture2D> mTextureList[6];
   };

   ///this class will use a 2D ortho quad and lookup
   ///into a cubemap to find the texture value
   class DT_CORE_EXPORT CubeMapProfile: public dtCore::SkyBox::RenderProfile
   {
   protected:
      class UpdateViewCallback: public osg::NodeCallback
      {
      public:
         UpdateViewCallback(CubeMapProfile* mp):mProfile(mp){}

         void operator()(osg::Node*, osg::NodeVisitor* nv)
         {
            osgUtil::CullVisitor* cullVisitor = dynamic_cast<osgUtil::CullVisitor*>(nv);
            if (cullVisitor != NULL)
            {
               osgUtil::RenderStage* rs = cullVisitor->getCurrentRenderBin()->getStage();
               osg::Camera* cn = rs->getCamera();
               if (cn)
               {
                  mProfile->UpdateViewMatrix(cn->getViewMatrix(), cn->getProjectionMatrix());
               }
            }
         }
      private:
         CubeMapProfile* mProfile;
      };

      friend class UpdateViewCallback;

   public:
      CubeMapProfile();
      virtual ~CubeMapProfile();

      void Config(osg::Group*);
      void SetTexture(dtCore::SkyBox::SkyBoxSideEnum side, const std::string& filename);

      osg::TextureCubeMap* GetCubeTexture() { return mCubeMap.get(); }

   protected:
      void UpdateViewMatrix(const osg::Matrix& viewMat, const osg::Matrix& projMat);

      dtCore::RefPtr<osg::Geode>          mGeode;
      dtCore::RefPtr<osg::TextureCubeMap> mCubeMap;
      dtCore::RefPtr<osg::Program>        mProgram;
      dtCore::RefPtr<osg::Uniform>        mInverseModelViewProjMatrix;
   };

}

#endif // DELTA_SKYBOX_PROFILES
